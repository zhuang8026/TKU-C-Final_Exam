# core — C 語言核心邏輯

## 檔案

| 檔案 | 負責內容 |
|------|---------|
| `parking.h` | 所有 struct 定義與函式宣告 |
| `parking.c` | 停車場初始化與記憶體釋放 |
| `data_structures.c` | 資料結構基礎操作（Stack / Queue / Grid / Linked List） |
| `logic.c` | 業務邏輯（入場、出場、歷史查詢） |
| `main.c` | CLI 入口，解析指令，輸出 JSON |

## 實作層次

| 層次 | 說明 |
|------|------|
| 第一層 | 定義所有資料型別（struct） |
| 第二層 | 動態配置與釋放所有資料結構（`init_lot` / `free_lot`） |
| 第三層 | 各資料結構的基礎操作，供第四層業務邏輯呼叫（Stack / Queue / Grid / Linked List） |
| 第四層 | 組合資料結構操作，實現系統功能：入場（`park_vehicle`）、出場（`exit_vehicle`）、歷史查詢（`search_history`） |
| 第五層 | CLI 介面，解析指令，呼叫業務邏輯，輸出 JSON（`main.c`） |
