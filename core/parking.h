#ifndef PARKING_H
#define PARKING_H

#include <time.h>

/* 停車場固定大小：3 列 × 3 欄，共 9 格 */
#define ROWS      3
#define COLS      3
#define PLATE_LEN 20   /* 車牌字串最大長度（含 '\0'） */

/* ────────────────────────────────────────
   Structs
   ──────────────────────────────────────── */

/* 一台車的資訊：車牌、入場時間、停在哪一格 */
typedef struct {
    char   plate[PLATE_LEN];  /* 車牌號碼 */
    time_t entry_time;        /* 入場時間（Unix timestamp） */
    int    row;               /* 停車格列 */
    int    col;               /* 停車格欄 */
} Vehicle;

/* 2D grid 的單一格位：是否被佔用、停了哪台車 */
typedef struct {
    int  occupied;            /* 0 = 空位，1 = 已佔用 */
    char plate[PLATE_LEN];    /* 佔用中的車牌（空位時為空字串） */
} ParkingSpot;

/* Stack 節點：存一台車，並指向下一個節點（linked-list 實作 stack） */
typedef struct StackNode {
    Vehicle           data;   /* 車輛資料 */
    struct StackNode *next;   /* 指向下方節點（較早入場的車） */
} StackNode;

/* Stack 本體：整個停車場的進出結構（LIFO，單排車道） */
typedef struct {
    StackNode *top;           /* 指向最後入場的車（可直接出場的車） */
    int        size;          /* 目前停車數量 */
} Stack;

/* Queue 節點：存一台等待入場的車，並指向下一個節點 */
typedef struct QueueNode {
    Vehicle           data;   /* 等待中的車輛資料 */
    struct QueueNode *next;   /* 指向下一位等待的車 */
} QueueNode;

/* Queue 本體：候位佇列（FIFO，車位滿時在外等待） */
typedef struct {
    QueueNode *front;         /* 指向最早等待的車（下一個入場） */
    QueueNode *rear;          /* 指向最後加入的車（新加入時接在此後） */
    int        size;          /* 目前等待數量 */
} Queue;

/* 歷史紀錄節點：一筆完整的停車紀錄（出場時寫入） */
typedef struct Record {
    char          plate[PLATE_LEN];  /* 車牌號碼 */
    time_t        entry_time;        /* 入場時間 */
    time_t        exit_time;         /* 出場時間 */
    struct Record *next;             /* 指向下一筆歷史紀錄 */
} Record;

/* 停車場整體狀態：所有資料結構的根節點 */
typedef struct {
    ParkingSpot **grid;         /* 2D 停車格地圖（雙層指標，動態配置） */
    Stack        *lane;         /* 停車場主結構（Stack，LIFO） */
    Queue        *wait;         /* 候位佇列（Queue，FIFO） */
    Record       *history_head; /* 歷史紀錄鏈結串列的頭節點 */
    int           history_count;/* 歷史紀錄總筆數 */
} LotState;

/* ────────────────────────────────────────
   Function declarations
   ──────────────────────────────────────── */

/* 建立並初始化停車場，動態配置所有資料結構，回傳根節點指標 */
LotState *init_lot(void);

/* 釋放停車場所有動態記憶體（grid、stack、queue、history） */
void free_lot(LotState *lot);

/* ── Stack ── */
int      stack_is_empty(Stack *s);
int      stack_is_full(Stack *s);
int      stack_push(Stack *s, Vehicle v);  /* 成功回傳 0，已滿回傳 -1 */
Vehicle  stack_pop(Stack *s);              /* 呼叫前須確認 stack 非空 */
Vehicle *stack_peek(Stack *s);             /* 回傳頂部指標，空則回傳 NULL */

/* ── Queue ── */
int     queue_is_empty(Queue *q);
void    queue_enqueue(Queue *q, Vehicle v);
Vehicle queue_dequeue(Queue *q);           /* 呼叫前須確認 queue 非空 */

/* ── Grid ── */
int  find_empty_spot(LotState *lot, int *row, int *col); /* 找到回傳 1，滿回傳 0 */
void set_spot(LotState *lot, int row, int col, int occupied, const char *plate);

/* ── Linked List (History) ── */
void history_append(LotState *lot, const char *plate, time_t entry_time, time_t exit_time);

/* ── Business Logic ── */
int      vehicle_in_lot(LotState *lot, const char *plate);        /* 車輛是否在場內 */
int      can_exit(LotState *lot, const char *plate);              /* 是否在 stack 頂部可出場 */
int      park_vehicle(LotState *lot, const char *plate);          /* 入場；滿則加入候位，回傳 -1 */
int      exit_vehicle(LotState *lot, const char *plate);          /* 出場；不在頂部回傳 -2 */
Record **search_history(LotState *lot, const char *plate, int *count); /* 查詢歷史，回傳 dynamic array */
void     free_search_results(Record **results);                   /* 釋放查詢結果陣列 */

#endif
