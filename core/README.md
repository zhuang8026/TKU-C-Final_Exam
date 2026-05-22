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
| 第四層 | 組合資料結構操作，實現系統功能：入場（`park_vehicle`）、出場（`exit_vehicle`）、歷史查詢（`search_history`）；出場後自動讓候位車輛入場 |
| 第五層 | CLI 介面（`main.c`）：解析 park / exit / status / history 指令，呼叫業務邏輯，輸出 JSON；啟動時讀取 `lot_state.bin`，結束前存檔 |

## 編譯與執行

```bash
# 編譯（在 core/ 目錄下執行）
gcc -o parking parking.c data_structures.c logic.c main.c

# 手動測試指令
./parking status
./parking park ABC-1234
./parking exit ABC-1234
./parking history ABC-1234
```

## 狀態持久化：lot_state.bin

C 程式每次被 Flask 透過 subprocess 呼叫後即結束，記憶體中的所有資料會消失。
`lot_state.bin` 是用來解決這個問題的二進位狀態檔：

- 每次程式結束前，`save_state()` 將 Grid、Stack、Queue、History 序列化寫入此檔
- 每次程式啟動時，`load_state()` 讀取此檔還原上次狀態
- 若檔案不存在（首次執行），自動建立全新空停車場

**注意事項**

- 此檔案會出現在執行 `./parking` 時的當前工作目錄
- Flask 呼叫 C 執行檔時，需確保每次從相同目錄執行，否則會讀不到正確狀態
- 建議由 Flask 透過環境變數 `LOT_STATE_FILE` 傳入絕對路徑，避免路徑不一致問題


