#ifndef PARKING_H
#define PARKING_H

#include <time.h>

#define ROWS      3
#define COLS      3
#define PLATE_LEN 20

/* ── Structs ── */

typedef struct {
    char   plate[PLATE_LEN];
    time_t entry_time;
    int    row;
    int    col;
} Vehicle;

typedef struct {
    int  occupied;
    char plate[PLATE_LEN];
} ParkingSpot;

typedef struct StackNode {
    Vehicle           data;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
    int        size;
} Stack;

typedef struct QueueNode {
    Vehicle           data;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int        size;
} Queue;

typedef struct Record {
    char          plate[PLATE_LEN];
    time_t        entry_time;
    time_t        exit_time;
    struct Record *next;
} Record;

typedef struct {
    ParkingSpot **grid;
    Stack        *lane;
    Queue        *wait;
    Record       *history_head;
    int           history_count;
} LotState;

/* ── Function declarations ── */

LotState *init_lot(void);
void      free_lot(LotState *lot);

#endif
