# 停車場管理系統 — 專案設計文件

## 專案規格確認

| 項目 | 決定 |
|------|------|
| 停車場大小 | 固定 3×3（共 9 格） |
| 出入口結構 | 整個停車場為單排車道（Stack，LIFO） |
| 收費功能 | 不需要 |
| 系統功能 | 入場、出場、查詢、歷史記錄 |
| 出場邏輯 | 僅允許頂部車輛出場；若目標車輛不在頂部，顯示需先移出哪些車 |
| Flask ↔ C | subprocess（Flask 呼叫編譯好的 C 執行檔，stdin/stdout 傳資料） |

---

## 九項需求完整對應

| 需求 | 落地方式 | 具體職責 |
|------|---------|---------|
| **struct** | `Vehicle`、`ParkingSpot`、`LotState`、`StackNode`、`QueueNode`、`Record` | 封裝所有資料，模組化系統狀態 |
| **malloc / free** | Stack / Queue / LinkedList 每個節點動態配置 | 入場 `malloc` 節點，出場 / 清除時 `free` |
| **pointer** | 四個明確場景（見下方） | 雙層指標、傳址呼叫、串列指標、`->` 運算子 |
| **2D array** | `ParkingSpot grid[3][3]` | 視覺化停車格地圖，顯示哪格有車、哪格空位 |
| **stack** | `LotState.lane`（linked-list 實作） | 整個停車場的進出順序（LIFO），對應單排車道 |
| **queue** | `LotState.wait`（linked-list 實作） | 車位滿時的候位佇列，FIFO 依序入場 |
| **linked list** | `LotState.history`（`Record *head`） | 每次入場 / 出場寫入一筆節點，永久保存歷史 |
| **dynamic array** | 查詢功能的結果緩衝（`Record **results`） | 使用者輸入車牌 → 走訪 linked list → 符合的紀錄存入 dynamic array 回傳 |
| **函式拆分** | 每項操作各自獨立函式 | 高內聚、低耦合，`main()` 只負責流程控制 |

---

## Pointer 四個明確場景

```
① ParkingSpot **grid
   → 雙層指標，動態配置 2D 停車格地圖的每一列

② park_vehicle(LotState *lot, StackNode *node)
   → 傳址呼叫，直接修改 lot 內部狀態，不複製整個結構

③ struct Record { Vehicle data; struct Record *next; }
   → linked list 節點以 *next 指標串接

④ vehicle->plate、node->data.entry_time
   → -> 運算子存取 struct 指標的成員
```

---

## Dynamic Array 設計說明

```
用途：歷史紀錄查詢結果緩衝

流程：
  使用者輸入車牌（例如 "ABC-1234"）
       ↓
  走訪 linked list（history）比對每筆 Record
       ↓
  符合的 Record 指標存入 dynamic array
  （初始大小 4，每次滿了 realloc 擴充 ×2）
       ↓
  回傳 dynamic array 給 Flask 序列化成 JSON

理由：
  Linked list 筆數不固定，查詢結果數量事先未知
  → 需要 realloc 動態擴充，不能用固定陣列
```

---

## 函式拆分規劃

```
初始化 / 釋放
  init_lot()            → 建立 LotState，配置 grid，初始化所有結構
  free_lot(lot)         → 釋放所有動態記憶體（grid、stack、queue、history）

停車格操作
  find_empty_spot(lot, *row, *col)  → 掃描 grid，找到第一個空格
  set_spot(lot, row, col, status)   → 標記格位為佔用或空閒

車輛操作
  park_vehicle(lot, plate)          → 入場主流程
  exit_vehicle(lot, plate)          → 出場主流程（含頂部檢查）
  can_exit(lot, plate)              → 檢查目標車是否在 stack 頂部

資料結構操作
  stack_push(stack, node)           → 車輛入場推入 stack
  stack_pop(stack)                  → 車輛出場彈出 stack
  stack_peek(stack)                 → 查看頂部車輛（不彈出）
  queue_enqueue(queue, node)        → 車位滿時加入候位
  queue_dequeue(queue)              → 車位空出時取出第一位
  history_append(lot, plate, type)  → 寫入一筆歷史紀錄到 linked list

查詢
  search_history(lot, plate)        → 走訪 linked list，結果存入 dynamic array
  free_results(results, count)      → 釋放查詢結果的 dynamic array
```

---

## 系統資料流

```
入場
  有空位 → queue 有車？先取 queue 第一台 → stack_push → 標記 grid
  無空位 → 新車加入 queue

出場
  使用者輸入車牌
    → can_exit() 檢查是否為 stack 頂部
    → YES：stack_pop → 清除 grid → history_append → 若 queue 有車則自動入場
    → NO ：回傳「請先移出 [車牌列表]」

查詢（歷史紀錄）
  使用者輸入車牌
    → search_history() 走訪 linked list
    → 符合結果存入 dynamic array
    → 回傳 JSON

顯示
  回傳 grid[3][3] 狀態給 Flask → Browser 渲染停車格地圖
```

---

## 系統架構

```
┌──────────────────────────────┐
│      Browser（前端介面）       │  停車格地圖、入場/出場/查詢表單
└──────────────┬───────────────┘
               │ HTTP Request（JSON）
               ▼
┌──────────────────────────────┐
│       Python Flask            │  路由處理、呼叫 C、JSON 序列化
└──────────────┬───────────────┘
               │ subprocess（stdin/stdout）
               ▼
┌──────────────────────────────┐
│       C Program               │  全部九項資料結構與核心邏輯
└──────────────────────────────┘
```

---

*最後更新：2026-05-22*
