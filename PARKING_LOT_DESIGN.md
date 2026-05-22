# 停車場管理系統 — 專案設計文件

## 資料結構完整對應

| 需求 | 停車場對應 | 說明 |
|------|-----------|------|
| **2D array** | 停車格地圖 | `ParkingSpot grid[ROWS][COLS]`，視覺化每格狀態 |
| **linked list** | 車輛進出歷史紀錄 | 每筆進出記錄串成鏈結串列 |
| **dynamic array** | 場內即時車輛清單 | 用 `realloc` 動態擴充 |
| **stack** | 單排車道（LIFO） | 車道最後進的車最先出，或操作 undo |
| **queue** | 候位等待佇列 | 車位滿時排隊等候，FIFO 依序入場 |
| **struct** | `Vehicle`、`ParkingSpot`、`LotState` | 封裝車牌、時間、格位資訊 |
| **malloc / free** | 動態建立 / 釋放車輛節點 | 進場 `malloc`，出場 `free` |
| **pointer** | 四種明確使用場景（見下） | 貫穿所有資料結構操作 |
| **函式拆分** | 每項操作獨立函式 | `park_vehicle()`、`exit_vehicle()`、`find_spot()` 等 |

---

## 指標（Pointer）四個明確使用場景

```c
// 1. 雙層指標動態配置 2D 停車格地圖
ParkingSpot **grid = malloc(rows * sizeof(ParkingSpot *));
for (int i = 0; i < rows; i++)
    grid[i] = malloc(cols * sizeof(ParkingSpot));

// 2. 傳址呼叫修改停車場狀態（pass by pointer）
void park_vehicle(LotState *lot, Vehicle *v);

// 3. linked list 節點以指標串接
typedef struct Record {
    Vehicle data;
    struct Record *next;
} Record;

// 4. -> 運算子存取 struct 指標成員
Vehicle *v = malloc(sizeof(Vehicle));
v->plate = "ABC-1234";
v->entry_time = time(NULL);
```

---

## 系統功能規劃

```
入場  → 從 queue 取出等待車輛 → malloc Vehicle → 2D array 標記格位
出場  → free Vehicle → 更新 2D array → linked list 寫入歷史
候位  → queue push（車位滿時）
車道  → stack push/pop（單排車道進出）
查詢  → 掃描 dynamic array 找車牌
報表  → 走訪 linked list 輸出歷史
```

---

## 系統架構

```
┌─────────────────────────────┐
│      Browser（前端介面）      │  停車格地圖、入場/出場表單
└──────────────┬──────────────┘
               │ HTTP Request
               ▼
┌─────────────────────────────┐
│       Python Flask           │  路由處理、JSON 序列化
└──────────────┬──────────────┘
               │ subprocess / ctypes
               ▼
┌─────────────────────────────┐
│       C Program              │  所有資料結構與核心邏輯
│  malloc / pointer / struct   │
└─────────────────────────────┘
```

---

*最後更新：2026-05-22*
