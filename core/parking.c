#include "parking.h"
#include <stdlib.h>
#include <string.h>

/* ────────────────────────────────────────
   內部初始化輔助函式
   ──────────────────────────────────────── */

/* 建立空的 Stack */
static Stack *init_stack(void) {
    Stack *s = malloc(sizeof(Stack));
    s->top  = NULL;
    s->size = 0;
    return s;
}

/* 建立空的 Queue */
static Queue *init_queue(void) {
    Queue *q  = malloc(sizeof(Queue));
    q->front  = NULL;
    q->rear   = NULL;
    q->size   = 0;
    return q;
}

/* ────────────────────────────────────────
   公開初始化
   ──────────────────────────────────────── */

/* 建立並初始化停車場，動態配置所有資料結構，回傳根節點指標 */
LotState *init_lot(void) {
    LotState *lot = malloc(sizeof(LotState));

    /* 雙層指標：先配置列陣列，再逐列配置欄陣列 */
    lot->grid = malloc(ROWS * sizeof(ParkingSpot *));
    for (int i = 0; i < ROWS; i++) {
        lot->grid[i] = malloc(COLS * sizeof(ParkingSpot));
        for (int j = 0; j < COLS; j++) {
            lot->grid[i][j].occupied = 0;
            memset(lot->grid[i][j].plate, 0, PLATE_LEN);
        }
    }

    lot->lane          = init_stack();
    lot->wait          = init_queue();
    lot->history_head  = NULL;
    lot->history_count = 0;

    return lot;
}

/* ────────────────────────────────────────
   內部釋放輔助函式
   ──────────────────────────────────────── */

/* 釋放 Stack 所有節點與本體 */
static void free_stack(Stack *s) {
    StackNode *curr = s->top;
    while (curr) {
        StackNode *next = curr->next;
        free(curr);
        curr = next;
    }
    free(s);
}

/* 釋放 Queue 所有節點與本體 */
static void free_queue(Queue *q) {
    QueueNode *curr = q->front;
    while (curr) {
        QueueNode *next = curr->next;
        free(curr);
        curr = next;
    }
    free(q);
}

/* 釋放歷史紀錄 Linked List 所有節點 */
static void free_history(Record *head) {
    Record *curr = head;
    while (curr) {
        Record *next = curr->next;
        free(curr);
        curr = next;
    }
}

/* ────────────────────────────────────────
   公開釋放
   ──────────────────────────────────────── */

/* 釋放停車場所有動態記憶體（grid、stack、queue、history） */
void free_lot(LotState *lot) {
    /* 逐列釋放欄陣列，再釋放列陣列本身 */
    for (int i = 0; i < ROWS; i++)
        free(lot->grid[i]);
    free(lot->grid);

    free_stack(lot->lane);
    free_queue(lot->wait);
    free_history(lot->history_head);
    free(lot);
}
