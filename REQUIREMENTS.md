# TKU C 語言期末報告 — 需求規格

## 核心必備技術（全部都要展示）

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

## 資料結構要求（擇一實作）

| 選項 | 說明 | 適合題型 |
|------|------|----------|
| **dynamic array（動態陣列）** | 用 malloc/realloc 動態擴充陣列 | 不定長度清單、儲存紀錄 |
| **2D array（棋盤設計）** | 二維陣列或指標陣列模擬棋盤 | 五子棋、圍棋、掃雷等 |
| **linked list** | 節點以指標串接的鏈結串列 | 佇列、歷史記錄、動態插入刪除 |

---

## 加分項目（Bonus）

- **stack（堆疊）**：LIFO，可用陣列或 linked list 實作
- **queue（佇列）**：FIFO，可用陣列（circular）或 linked list 實作
- 實作其中一種或兩種皆可獲得加分

---

## 評分重點摘要

```
必要條件（全部需展示）：
  ✅ 函式拆分       — 不能全寫在 main()
  ✅ struct         — 至少定義並使用一個結構體
  ✅ malloc / free  — 動態配置，記得釋放
  ✅ pointer        — 指標操作或傳址

資料結構（擇一）：
  ☐ dynamic array
  ☐ 2D array（棋盤）
  ☐ linked list

加分（選做）：
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
