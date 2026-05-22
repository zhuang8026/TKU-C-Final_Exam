#include "parking.h"
#include <stdlib.h>
#include <string.h>

/* ────────────────────────────────────────
Stack 操作
──────────────────────────────────────── */

int stack_is_empty(Stack *s) {
    return s->size == 0;
}

/* 停車場最多 ROWS * COLS 台車 */
int stack_is_full(Stack *s) {
    return s->size == ROWS * COLS;
}

/* 新車入場：配置節點，推入 stack 頂部 */
int stack_push(Stack *s, Vehicle v) {
    if (stack_is_full(s)) return -1;
    StackNode *node = malloc(sizeof(StackNode));
    node->data = v;
    node->next = s->top;
    s->top     = node;
    s->size++;
    return 0;
}

/* 頂部車輛出場：彈出節點並釋放，回傳車輛資料 */
Vehicle stack_pop(Stack *s) {
    StackNode *node = s->top;
    Vehicle    v    = node->data;
    s->top          = node->next;
    free(node);
    s->size--;
    return v;
}

/* 查看頂部車輛但不移除，空則回傳 NULL */
Vehicle *stack_peek(Stack *s) {
    if (stack_is_empty(s)) return NULL;
    return &s->top->data;
}

/* ────────────────────────────────────────
Queue 操作
──────────────────────────────────────── */

int queue_is_empty(Queue *q) {
    return q->size == 0;
}

/* 新車加入候位：配置節點，接在 rear 後方 */
void queue_enqueue(Queue *q, Vehicle v) {
    QueueNode *node = malloc(sizeof(QueueNode));
    node->data = v;
    node->next = NULL;
    if (queue_is_empty(q))
        q->front = q->rear = node;
    else {
        q->rear->next = node;
        q->rear       = node;
    }
    q->size++;
}

/* 取出最早等待的車：移除 front 節點，回傳車輛資料 */
Vehicle queue_dequeue(Queue *q) {
    QueueNode *node = q->front;
    Vehicle    v    = node->data;
    q->front        = node->next;
    if (!q->front) q->rear = NULL;
    free(node);
    q->size--;
    return v;
}

/* ────────────────────────────────────────
Grid 操作
──────────────────────────────────────── */

/* 掃描 grid，回傳第一個空格的列欄位置；找到回傳 1，全滿回傳 0 */
int find_empty_spot(LotState *lot, int *row, int *col) {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            if (!lot->grid[i][j].occupied) {
                *row = i;
                *col = j;
                return 1;
            }
    return 0;
}

/* 標記指定格位為佔用或空閒，並更新車牌資訊 */
void set_spot(LotState *lot, int row, int col, int occupied, const char *plate) {
    lot->grid[row][col].occupied = occupied;
    if (plate) {
        strncpy(lot->grid[row][col].plate, plate, PLATE_LEN - 1);
        lot->grid[row][col].plate[PLATE_LEN - 1] = '\0';
    } else {
        memset(lot->grid[row][col].plate, 0, PLATE_LEN);
    }
}

/* ────────────────────────────────────────
Linked List 操作（History）
──────────────────────────────────────── */

/* 出場時呼叫：配置新節點，附加至 linked list 尾端 */
void history_append(LotState *lot, const char *plate,
                    time_t entry_time, time_t exit_time) {
    Record *rec = malloc(sizeof(Record));
    strncpy(rec->plate, plate, PLATE_LEN - 1);
    rec->plate[PLATE_LEN - 1] = '\0';
    rec->entry_time = entry_time;
    rec->exit_time  = exit_time;
    rec->next       = NULL;

    if (!lot->history_head) {
        lot->history_head = rec;
    } else {
        Record *curr = lot->history_head;
        while (curr->next) curr = curr->next;
        curr->next = rec;
    }
    lot->history_count++;
}
