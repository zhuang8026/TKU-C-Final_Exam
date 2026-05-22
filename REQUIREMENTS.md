# TKU C 語言期末報告 — 需求規格

## 核心必備技術

### 1. 函式拆分（Function Decomposition）
- 設計原則：**高內聚、低耦合**
- 每個函式只做一件事，避免過長的 main()
- 透過函式參數與回傳值傳遞資料，減少全域變數依賴

### 2. struct（結構體）
- 用途：**封裝與模組化資料**
- 將相關欄位包裝進同一個 struct，例如 `Player`、`Node`、`Board`
- 搭配指標（`struct Foo *`）操作更靈活

### 3. malloc / free（動態記憶體）
- 用途：**掌握動態記憶體生命週期**
- 配置：`malloc()` / `calloc()`
- 釋放：`free()`，避免 memory leak
- 重點：每次 malloc 都要有對應的 free

### 4. Pointer（指標）
- 用途：**精準操作記憶體位址**
- 傳址呼叫（pass by pointer）修改外部變數
- 搭配 malloc 操作動態記憶體
- 用 `->` 存取 struct 指標的成員

---

## 資料結構要求

| 類別 | 選項 | 說明 | 適合題型 / 應用情境 |
|---|---|---|
| 基本 | **dynamic array（動態陣列）** | 使用 `malloc` / `realloc` 動態擴充陣列，適合在資料筆數不固定時使用 | 不定長度清單、查詢結果暫存、儲存紀錄 |
| 基本 | **2D array（二維陣列）** | 使用二維陣列或指標陣列模擬棋盤、地圖或表格 | 五子棋、圍棋、掃雷、座位表、停車格 |
| 基本 | **linked list（鏈結串列）** | 節點之間透過指標串接，方便動態新增、刪除資料 | 歷史紀錄、車輛紀錄、排隊資料、動態插入刪除 |
| 進階 | **stack（堆疊）** | LIFO，後進先出，可用陣列或 linked list 實作 | Undo 功能、瀏覽紀錄、暫存上一筆操作 |
| 進階 | **queue（佇列）** | FIFO，先進先出，可用 circular array 或 linked list 實作 | 排隊系統、等待處理清單、車輛進場順序 |

---

## 評分重點摘要

```
必要條件：
  ✅ 函式拆分       — 不能全寫在 main()
  ✅ struct         — 至少定義並使用一個結構體
  ✅ malloc / free  — 動態配置，記得釋放
  ✅ pointer        — 指標操作或傳址

資料結構：
  ☐ dynamic array
  ☐ 2D array（棋盤）
  ☐ linked list
  ☐ stack
  ☐ queue
```

---

---

## 進階架構：Web × C 系統

```
┌─────────────────────────────┐
│      Browser（前端介面）      │  HTML / CSS / JS
└──────────────┬──────────────┘
               │ HTTP Request
               ▼
┌─────────────────────────────┐
│  Python Flask                │  後端路由與 Web 邏輯
│  （後端路由與 Web 邏輯）       │  呼叫 C 程式處理核心運算
└──────────────┬──────────────┘
               │ subprocess / ctypes / pipe
               ▼
┌─────────────────────────────┐
│  C Program                   │  核心邏輯
│  （核心邏輯：記憶體與指標運算） │  struct / malloc / pointer
└─────────────────────────────┘
```

### 各層職責

| 層次 | 技術 | 職責 |
|------|------|------|
| 前端 | Browser（HTML/JS） | 使用者介面、輸入與顯示 |
| 中間層 | Python Flask | HTTP 路由、請求處理、呼叫 C 程式 |
| 核心層 | C Program | 記憶體管理、指標運算、資料結構邏輯 |

### Flask ↔ C 溝通方式（常見選項）

- `subprocess.run()` — 最簡單，Flask 呼叫編譯好的 C 執行檔，透過 stdin/stdout 傳資料
- `ctypes` — 將 C 編譯成 `.so` 共享函式庫，Flask 直接呼叫函式
- Named pipe / socket — 適合長連線或即時通訊

---

*最後更新：2026-05-22*
