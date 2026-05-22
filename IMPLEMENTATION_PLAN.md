# 實作流程規劃

## 原則：由下往上，資料結構先於業務邏輯

---

## 第一層：Struct 定義 + Header
- 定義所有型別：`Vehicle`、`ParkingSpot`、`StackNode`、`Stack`、`QueueNode`、`Queue`、`Record`、`LotState`
- 產出：`parking.h`

## 第二層：Init / Free
- `init_lot()` — 建立 LotState，動態配置 grid（雙層指標）、Stack、Queue
- `free_lot()` — 釋放所有動態記憶體
- 產出：`parking.c`（初始化部分）

## 第三層：基礎資料結構操作（各自獨立，可個別測試）
- Stack：`stack_push()` / `stack_pop()` / `stack_peek()`
- Queue：`queue_enqueue()` / `queue_dequeue()`
- Linked list：`history_append()` / `history_traverse()`
- 2D grid：`find_empty_spot()` / `set_spot()`
- 產出：`parking.c`（資料結構操作部分）

## 第四層：業務邏輯（組合第三層積木）
- `park_vehicle()` — 入場主流程
- `exit_vehicle()` — 出場主流程（含頂部檢查）
- `can_exit()` — 檢查目標車是否在 stack 頂部
- `search_history()` — 走訪 linked list，結果存入 dynamic array
- 產出：`parking.c`（業務邏輯部分）

## 第五層：CLI 介面（subprocess 入口）
- 解析指令與參數，呼叫對應業務邏輯，輸出 JSON
- 產出：`main.c`

## 第六層：Flask
- HTTP 路由，呼叫 C 執行檔，序列化 JSON 回傳
- 產出：`app.py`

## 第七層：Frontend
- 渲染停車格地圖，串接入場 / 出場 / 查詢表單
- 產出：`index.html` + JS

---

*最後更新：2026-05-22*
