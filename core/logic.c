#include "parking.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ────────────────────────────────────────
輔助查詢
──────────────────────────────────────── */

/* 走訪 stack，確認車牌是否在場內 */
int vehicle_in_lot(LotState *lot, const char *plate) {
    StackNode *curr = lot->lane->top;
    while (curr) {
        if (strcmp(curr->data.plate, plate) == 0) return 1;
        curr = curr->next;
    }
    return 0;
}

/* 檢查目標車牌是否在 stack 頂部（可直接出場） */
int can_exit(LotState *lot, const char *plate) {
    Vehicle *top = stack_peek(lot->lane);
    if (!top) return 0;
    return strcmp(top->plate, plate) == 0;
}

/* ────────────────────────────────────────
   入場 / 出場
   ──────────────────────────────────────── */

/* 入場：有空位直接停車；停車場滿則加入候位佇列，回傳 -1 */
int park_vehicle(LotState *lot, const char *plate) {
    if (stack_is_full(lot->lane)) {
        Vehicle v;
        strncpy(v.plate, plate, PLATE_LEN - 1);
        v.plate[PLATE_LEN - 1] = '\0';
        v.entry_time = time(NULL);
        v.row = -1;
        v.col = -1;
        queue_enqueue(lot->wait, v);
        return -1;
    }

    int row, col;
    find_empty_spot(lot, &row, &col);

    Vehicle v;
    strncpy(v.plate, plate, PLATE_LEN - 1);
    v.plate[PLATE_LEN - 1] = '\0';
    v.entry_time = time(NULL);
    v.row        = row;
    v.col        = col;

    set_spot(lot, row, col, 1, plate);
    stack_push(lot->lane, v);
    return 0;
}

/* 出場：僅允許 stack 頂部車輛出場；出場後自動讓候位第一台入場
   回傳  0：成功
   回傳 -1：車牌不在場內
   回傳 -2：車輛被其他車擋住，無法直接出場 */
int exit_vehicle(LotState *lot, const char *plate) {
    if (stack_is_empty(lot->lane))   return -1;
    if (!vehicle_in_lot(lot, plate)) return -1;
    if (!can_exit(lot, plate))       return -2;

    Vehicle v = stack_pop(lot->lane);
    set_spot(lot, v.row, v.col, 0, NULL);
    history_append(lot, v.plate, v.entry_time, time(NULL));

    /* 空出格位後，自動讓候位佇列第一台車入場 */
    if (!queue_is_empty(lot->wait)) {
        Vehicle waiting = queue_dequeue(lot->wait);
        int row, col;
        find_empty_spot(lot, &row, &col);
        waiting.entry_time = time(NULL);
        waiting.row        = row;
        waiting.col        = col;
        set_spot(lot, row, col, 1, waiting.plate);
        stack_push(lot->lane, waiting);
    }

    return 0;
}

/* ────────────────────────────────────────
   歷史查詢（Dynamic Array）
   ──────────────────────────────────────── */

/* 走訪 linked list，將符合車牌的 Record 存入 dynamic array 回傳 */
Record **search_history(LotState *lot, const char *plate, int *count) {
    int      capacity = 4;
    Record **results  = malloc(capacity * sizeof(Record *));
    *count = 0;

    Record *curr = lot->history_head;
    while (curr) {
        if (strcmp(curr->plate, plate) == 0) {
            if (*count == capacity) {
                capacity *= 2;
                results   = realloc(results, capacity * sizeof(Record *));
            }
            results[(*count)++] = curr;
        }
        curr = curr->next;
    }

    return results;
}

/* 釋放查詢結果的 dynamic array（不釋放 Record 節點，由 history linked list 管理） */
void free_search_results(Record **results) {
    free(results);
}
