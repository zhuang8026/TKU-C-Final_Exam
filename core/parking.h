#ifndef PARKING_H
#define PARKING_H

#include <time.h>

/* 停車場固定大小：3 列 × 3 欄，共 9 格 */
#define ROWS      3     /* 列 */
#define COLS      3     /* 欄 */
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

#endif
