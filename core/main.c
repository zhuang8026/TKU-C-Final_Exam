#include "parking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 狀態檔路徑：每次 subprocess 呼叫結束前存檔，下次啟動時讀取，確保狀態持久 */
#define STATE_FILE "lot_state.bin"

/* ────────────────────────────────────────
狀態序列化（存檔 / 讀檔）
──────────────────────────────────────── */

/* 歷史紀錄的序列化格式：去除 *next 指標，只保留資料欄位，方便寫入二進位檔 */
typedef struct {
    char   plate[PLATE_LEN];
    time_t entry_time;
    time_t exit_time;
} RecordSave;

/* 將目前停車場狀態寫入 STATE_FILE
   寫入順序：Grid → Stack → Queue → History */
static void save_state(LotState *lot) {
    FILE *f = fopen(STATE_FILE, "wb");
    if (!f) return;

    /* Grid：固定大小，逐列寫入 */
    for (int i = 0; i < ROWS; i++)
        fwrite(lot->grid[i], sizeof(ParkingSpot), COLS, f);

    /* Stack：走訪 linked list，收進暫存陣列（底 → 頂順序），再寫入
              還原時依同樣順序 push，可重建相同的 stack 狀態 */
    int        stack_size = lot->lane->size;
    Vehicle   *stack_arr  = malloc(stack_size * sizeof(Vehicle));
    StackNode *sc         = lot->lane->top;
    for (int i = stack_size - 1; i >= 0; i--) {
        stack_arr[i] = sc->data;
        sc = sc->next;
    }
    fwrite(&stack_size, sizeof(int), 1, f);
    fwrite(stack_arr, sizeof(Vehicle), stack_size, f);
    free(stack_arr);

    /* Queue：走訪 linked list，收進暫存陣列（front → rear 順序），再寫入
            還原時依同樣順序 enqueue，可重建相同的 queue 狀態 */
    int        queue_size = lot->wait->size;
    Vehicle   *queue_arr  = malloc(queue_size * sizeof(Vehicle));
    QueueNode *qc         = lot->wait->front;
    for (int i = 0; i < queue_size; i++) {
        queue_arr[i] = qc->data;
        qc = qc->next;
    }
    fwrite(&queue_size, sizeof(int), 1, f);
    fwrite(queue_arr, sizeof(Vehicle), queue_size, f);
    free(queue_arr);

    /* History Linked List：走訪 head → tail，逐筆寫入 RecordSave */
    int     history_count = lot->history_count;
    Record *rc            = lot->history_head;
    fwrite(&history_count, sizeof(int), 1, f);
    while (rc) {
        RecordSave rs;
        strncpy(rs.plate, rc->plate, PLATE_LEN - 1);
        rs.plate[PLATE_LEN - 1] = '\0';
        rs.entry_time = rc->entry_time;
        rs.exit_time  = rc->exit_time;
        fwrite(&rs, sizeof(RecordSave), 1, f);
        rc = rc->next;
    }

    fclose(f);
}

/* 從 STATE_FILE 讀取停車場狀態並還原
   若檔案不存在（首次執行），回傳全新空停車場 */
static LotState *load_state(void) {
    FILE *f = fopen(STATE_FILE, "rb");
    if (!f) return init_lot();

    LotState *lot = init_lot();

    /* Grid */
    for (int i = 0; i < ROWS; i++)
        fread(lot->grid[i], sizeof(ParkingSpot), COLS, f);

    /* Stack：依底 → 頂順序依序 push，還原入場順序 */
    int      stack_size;
    fread(&stack_size, sizeof(int), 1, f);
    Vehicle *stack_arr = malloc(stack_size * sizeof(Vehicle));
    fread(stack_arr, sizeof(Vehicle), stack_size, f);
    for (int i = 0; i < stack_size; i++)
        stack_push(lot->lane, stack_arr[i]);
    free(stack_arr);

    /* Queue：依 front → rear 順序依序 enqueue，還原等待順序 */
    int      queue_size;
    fread(&queue_size, sizeof(int), 1, f);
    Vehicle *queue_arr = malloc(queue_size * sizeof(Vehicle));
    fread(queue_arr, sizeof(Vehicle), queue_size, f);
    for (int i = 0; i < queue_size; i++)
        queue_enqueue(lot->wait, queue_arr[i]);
    free(queue_arr);

    /* History：依序呼叫 history_append，還原 linked list */
    int history_count;
    fread(&history_count, sizeof(int), 1, f);
    for (int i = 0; i < history_count; i++) {
        RecordSave rs;
        fread(&rs, sizeof(RecordSave), 1, f);
        history_append(lot, rs.plate, rs.entry_time, rs.exit_time);
    }

    fclose(f);
    return lot;
}

/* ────────────────────────────────────────
JSON 輸出輔助函式
──────────────────────────────────────── */

/* 輸出停車場目前狀態：3×3 grid 每格的佔用情形，以及停車數與候位數 */
static void print_status(LotState *lot) {
    printf("{\"grid\":[");
    for (int i = 0; i < ROWS; i++) {
        printf("[");
        for (int j = 0; j < COLS; j++) {
            printf("{\"occupied\":%s,\"plate\":\"%s\"}",
                lot->grid[i][j].occupied ? "true" : "false",
                lot->grid[i][j].plate);
            if (j < COLS - 1) printf(",");
        }
        printf("]");
        if (i < ROWS - 1) printf(",");
    }
    printf("],\"parked\":%d,\"waiting\":%d}\n", lot->lane->size, lot->wait->size);
}

/* 出場被擋住時，走訪 stack 頂部到目標車之間的所有車牌，列入 blocking 陣列回傳 */
static void print_blocked(LotState *lot, const char *plate) {
    printf("{\"error\":\"blocked\",\"plate\":\"%s\",\"blocking\":[", plate);
    StackNode *curr  = lot->lane->top;
    int        first = 1;
    while (curr && strcmp(curr->data.plate, plate) != 0) {
        if (!first) printf(",");
        printf("\"%s\"", curr->data.plate);
        first = 0;
        curr  = curr->next;
    }
    printf("]}\n");
}

/* ────────────────────────────────────────
main：解析指令，呼叫業務邏輯，輸出 JSON
用法：   ./parking <command> [plate]
        park    <plate>  入場（滿則加入候位佇列）
        exit    <plate>  出場（僅限頂部車輛；被擋則回傳阻擋清單）
        status           停車場目前狀態（grid + 統計）
        history <plate>  查詢指定車牌的所有歷史紀錄
──────────────────────────────────────── */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("{\"error\":\"no command\"}\n");
        return 1;
    }

    LotState   *lot = load_state();  /* 讀取上次儲存的狀態 */
    const char *cmd = argv[1];

    if (strcmp(cmd, "park") == 0) {
        if (argc < 3) { printf("{\"error\":\"missing plate\"}\n"); goto done; }
        int r = park_vehicle(lot, argv[2]);
        if (r == 0)
            printf("{\"status\":\"parked\",\"plate\":\"%s\"}\n", argv[2]);
        else
            /* r == -1：停車場滿，車輛已加入候位佇列 */
            printf("{\"status\":\"queued\",\"plate\":\"%s\"}\n", argv[2]);

    } else if (strcmp(cmd, "exit") == 0) {
        if (argc < 3) { printf("{\"error\":\"missing plate\"}\n"); goto done; }
        int r = exit_vehicle(lot, argv[2]);
        if (r == 0)
            printf("{\"status\":\"exited\",\"plate\":\"%s\"}\n", argv[2]);
        else if (r == -1)
            /* 車牌不在場內 */
            printf("{\"error\":\"not found\",\"plate\":\"%s\"}\n", argv[2]);
        else
            /* r == -2：車輛被上方其他車擋住，回傳需先移出的車牌清單 */
            print_blocked(lot, argv[2]);

    } else if (strcmp(cmd, "status") == 0) {
        print_status(lot);

    } else if (strcmp(cmd, "history") == 0) {
        if (argc < 3) { printf("{\"error\":\"missing plate\"}\n"); goto done; }
        int      count;
        /* search_history 回傳 dynamic array，用完須呼叫 free_search_results 釋放 */
        Record **results = search_history(lot, argv[2], &count);
        printf("[");
        for (int i = 0; i < count; i++) {
            printf("{\"plate\":\"%s\",\"entry\":%ld,\"exit\":%ld}",
                    results[i]->plate,
                    (long)results[i]->entry_time,
                    (long)results[i]->exit_time);
            if (i < count - 1) printf(",");
        }
        printf("]\n");
        free_search_results(results);

    } else {
        printf("{\"error\":\"unknown command\"}\n");
    }

done:
    save_state(lot);  /* 將最新狀態寫回檔案，供下次 subprocess 呼叫讀取 */
    free_lot(lot);
    return 0;
}
