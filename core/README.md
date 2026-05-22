# core — C 語言核心邏輯

## 檔案

- `parking.h` — 所有 struct 定義與函式宣告
- `parking.c` — 停車場初始化、記憶體釋放
- `main.c` — CLI 入口，解析指令，輸出 JSON

## 實作層次

- 第一層：Struct 定義與函式宣告（`parking.h`）
- 第二層：初始化與記憶體釋放（`init_lot`、`free_lot`）
- 第三層：基礎資料結構操作（Stack、Queue、Linked List、Grid）
- 第四層：業務邏輯（入場、出場、查詢、歷史記錄）
- 第五層：CLI 介面，解析指令，呼叫業務邏輯，輸出 JSON（`main.c`）
