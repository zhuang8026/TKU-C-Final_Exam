# 停車場管理系統 — Parking Lot Management System

淡江大學 C 語言期末專案
以 C 語言實作停車場核心邏輯，透過 Python Flask 提供 HTTP API，前端以純 HTML/CSS/JavaScript 呈現即時畫面。

---

## 功能介紹

| 功能 | 說明 |
|------|------|
| 停車格地圖 | 3×3 格停車場，即時顯示每格的佔用狀態與車牌 |
| 入場 | 輸入車牌，有空位直接停入；車位滿則自動加入候位佇列 |
| 出場 | 僅限最後入場的車輛可直接離開（LIFO 單排車道）；若被擋住則顯示需先移出的車牌清單 |
| 候位自動入場 | 車輛出場後，候位佇列中的第一台車自動填入空出的格位 |
| 歷史查詢 | 輸入車牌，顯示所有入場與出場時間紀錄 |

---

## 使用技術

| 層次 | 技術 | 說明 |
|------|------|------|
| 核心邏輯 | C 語言 | struct、pointer、malloc/free、2D array、Stack、Queue、Linked List、Dynamic Array |
| 後端 API | Python 3 + Flask | 接收 HTTP 請求，透過 subprocess 呼叫 C 執行檔 |
| 跨來源支援 | flask-cors | 允許前端與後端不同 port 的請求 |
| 前端畫面 | HTML / CSS / JavaScript | fetch API 呼叫 Flask，動態渲染停車格地圖 |
| 狀態持久化 | 二進位檔 `lot_state.bin` | 每次 subprocess 執行結束前序列化寫檔，下次啟動讀取還原 |

### C 語言 9 項需求對應

| 需求 | 對應實作 |
|------|---------|
| 函式拆分 | 四個 .c 檔各司其職（parking.c / data_structures.c / logic.c / main.c） |
| struct | `Vehicle`、`ParkingSpot`、`Stack`、`Queue`、`Record`、`LotState` |
| malloc / free | `init_lot` 配置、`free_lot` 釋放，所有節點皆動態配置 |
| pointer | 雙層指標 `ParkingSpot **grid`、函式參數傳址、`->` 運算子 |
| dynamic array | `search_history` 以 realloc 實作可擴張的查詢結果緩衝區 |
| 2D array | `grid[ROWS][COLS]` 以雙層指標動態配置 |
| linked list | `Record *history_head` 歷史紀錄鏈結串列 |
| stack | 以 linked list 實作的 LIFO 停車道（`Stack` + `StackNode`） |
| queue | 以 linked list 實作的 FIFO 候位佇列（`Queue` + `QueueNode`） |

---

## 系統架構

```
瀏覽器 (HTML/CSS/JS)
    │  HTTP fetch
    ▼
Python Flask (server/app.py)  ←→  subprocess
    │
    ▼
C 執行檔 (core/parking)
    │  讀寫
    ▼
lot_state.bin（二進位狀態檔）
```

### 目錄結構

```
TKU-C-Final_Exam_v2/
├── core/                   # C 語言核心
│   ├── parking.h           # struct 定義與函式宣告
│   ├── parking.c           # 初始化與記憶體釋放
│   ├── data_structures.c   # Stack / Queue / Grid / Linked List 操作
│   ├── logic.c             # 業務邏輯（入場 / 出場 / 歷史查詢）
│   ├── main.c              # CLI 入口，解析指令，輸出 JSON
│   └── README.md
├── server/                 # Python Flask 後端
│   ├── app.py              # HTTP 路由
│   └── README.md
├── client/                 # 前端畫面
│   ├── index.html
│   ├── style.css
│   ├── app.js
│   └── README.md
├── .gitignore
└── README.md               # 總文件說明
```

---

## 專案啟動方式

### 步驟一：編譯 C 核心

```bash
cd core
gcc -o parking parking.c data_structures.c logic.c main.c
```

### 步驟二：安裝 Python 套件

```bash
cd server
python3 -m venv venv
source venv/bin/activate
pip install flask flask-cors
```

### 步驟三：啟動 Flask

```bash
cd server
source venv/bin/activate
python app.py
```

Flask 啟動後監聽於 `http://127.0.0.1:5000`

### 步驟四：開啟前端

直接用瀏覽器開啟 `client/index.html`（無需 Web Server）。

---

## Claude AI Prompt 使用紀錄

以下為本專案開發過程中與 Claude AI 的對話摘要，依開發順序記錄。

| # | Prompt 內容 | 說明 |
|---|-------------|------|
| 1 | 分析圖片中所有的需求 | 從需求截圖提取 9 項 C 語言評分標準 |
| 2 | 這是未來的架構（附架構圖） | 確認 Browser → Flask → C 的三層架構 |
| 3 | 依照目前的需求，做什麼專案或者遊戲，比較適合？ | 探索符合所有需求的專案類型 |
| 4 | 停車場管理，你沒有規劃到指標 | 補充 pointer 的使用設計（雙層指標 grid） |
| 5 | 這是專案需求，另外建立文件 | 建立 REQUIREMENTS.md 等設計文件 |
| 6 | 實作之前，要記得區分 core / server / client | 確認目錄分層與職責劃分 |
| 7 | 開始實作第一層（struct 定義） | 建立 parking.h，定義所有 struct |
| 8 | 進去第二層（init/free） | 建立 parking.c，實作 init_lot / free_lot |
| 9 | 繼續第三層（資料結構操作） | 建立 data_structures.c，實作 Stack / Queue / Grid / History |
| 10 | ds.c 為什麼這樣命名？有更好命名嗎？ | 將 ds.c 重新命名為 data_structures.c |
| 11 | 把所有的邏輯都放在 parking.c 是好的嗎？ | 決定拆出 logic.c，分離業務邏輯與資料結構 |
| 12 | 繼續第四層（業務邏輯） | 建立 logic.c，實作 park_vehicle / exit_vehicle / search_history |
| 13 | 更新 README，繼續第五層 | 建立 main.c，實作 CLI 介面與 lot_state.bin 持久化 |
| 14 | lot_state.bin 文件是什麼？怎麼樣才會出現這個問題？ | 說明 subprocess 每次建立新行程導致狀態消失的問題與解法 |
| 15 | 繼續第六層（Flask） | 建立 server/app.py，實作 4 條 HTTP 路由 |
| 16 | 先測試到目前是否有問題 | 逐項驗證所有 API 路由與 C 執行結果 |
| 17 | 到目前位置，哪些文件是不用上 github？ | 建立 .gitignore（C 執行檔、lot_state.bin、venv、__pycache__） |
| 18 | 開始第七層（前端） | 建立 client/ 三檔案，完成停車場地圖與操作面板 |
| 19 | 檢查到目前為止有沒有什麼問題 | 全面檢查編譯、功能測試、API 路由、前端邏輯 |
| 20 | 在最外層建立 readme（本次） | 建立本文件，對整個專案進行完整說明 |
