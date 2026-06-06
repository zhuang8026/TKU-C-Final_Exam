# 停車場管理系統 — Parking Lot Management System

淡江大學 C 語言期末專案
以 C 語言實作停車場核心邏輯，透過 Python Flask 提供 HTTP API，前端以純 HTML/CSS/JavaScript 呈現即時畫面。

## Demo 網址

[https://brave-field-08b4ef100.7.azurestaticapps.net/](https://brave-field-08b4ef100.7.azurestaticapps.net/)

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

以下為本專案開發過程中與 Claude AI 的對話摘要，依開發順序記錄。每條 Prompt 皆已潤稿為可被 AI 直接理解與執行的明確指令。

| # | Prompt 內容 | 說明 |
|---|-------------|------|
| 1 | 請分析這張需求截圖，提取出所有 C 語言期末專案的評分項目，整理成清單格式，明確指出每項需求所考核的語法概念（如 struct、pointer、malloc/free、2D array、dynamic array、linked list、stack、queue、函式拆分） | 從需求截圖提取 9 項 C 語言評分標準 |
| 2 | 這是規劃中的系統架構：瀏覽器（HTML/CSS/JS）→ Python Flask → subprocess → C 執行檔 → 二進位狀態檔。請確認此三層架構的可行性，並說明各層的職責、資料流方向與跨層溝通方式 | 確認 Browser → Flask → C 的三層架構 |
| 3 | 依照前述 9 項 C 語言評分標準，請推薦 3-5 個可同時涵蓋所有需求的專案或遊戲類型，並針對每個方案分析優缺點、實作難度與需求對應度 | 探索符合所有需求的專案類型 |
| 4 | 選定「停車場管理系統」後，發現原規劃缺少 pointer 應用。請補強指標的使用設計，例如以雙層指標 `ParkingSpot **grid` 實作 2D 動態陣列、函式參數以位址傳遞、`->` 運算子存取 struct 成員等 | 補充 pointer 的使用設計（雙層指標 grid） |
| 5 | 請將前述討論結果整理為設計文件（REQUIREMENTS.md、DESIGN.md），內容須包含：9 項需求對應表、各 struct 欄位設計、Stack / Queue / Linked List 的節點結構、API 規格、資料流程圖 | 建立 REQUIREMENTS.md 等設計文件 |
| 6 | 在開始實作前，請規劃 core（C 核心邏輯）/ server（Flask 後端）/ client（前端畫面）三個目錄的職責分工，並列出各目錄應包含的檔案名稱、彼此的呼叫關係與溝通介面（CLI 參數 + JSON 輸出） | 確認目錄分層與職責劃分 |
| 7 | 請建立 core/parking.h，定義所有 struct（Vehicle、ParkingSpot、StackNode、Stack、QueueNode、Queue、Record、LotState），並宣告所有對外函式（init_lot、free_lot、park_vehicle、exit_vehicle、search_history 等）與 include guard | 建立 parking.h，定義所有 struct |
| 8 | 請建立 core/parking.c，實作 `init_lot()`（動態配置雙層指標 grid、初始化 Stack 與 Queue、設定初始狀態）與 `free_lot()`（依序釋放 grid 每一格、Stack 所有節點、Queue 所有節點、history 鏈結串列），確保無記憶體洩漏 | 建立 parking.c，實作 init_lot / free_lot |
| 9 | 請建立 core/ds.c（暫定名），實作通用資料結構操作：Stack 的 push/pop/peek、Queue 的 enqueue/dequeue、Grid 的存取與更新、History 鏈結串列的 append。所有節點皆以 malloc 動態配置 | 建立 data_structures.c，實作 Stack / Queue / Grid / History |
| 10 | 請評估 ds.c 命名是否符合可讀性原則。「ds」過於簡短且不直觀，請建議更明確的命名以反映檔案職責 | 將 ds.c 重新命名為 data_structures.c |
| 11 | 請評估將所有業務邏輯（入場、出場、查詢）都放在 parking.c 的設計是否合理。若違反單一職責原則，請建議是否要拆出 logic.c 來分離「資料結構操作」與「業務邏輯」，並說明拆分的優點 | 決定拆出 logic.c，分離業務邏輯與資料結構 |
| 12 | 請建立 core/logic.c，實作三個核心業務邏輯：`park_vehicle()`（有空位直接停入，否則加入候位佇列）、`exit_vehicle()`（僅限 Stack 頂端可離開，被擋住則回傳需先移出的車牌清單，出場後自動填入候位車輛）、`search_history()`（以 realloc 實作可擴張的查詢結果動態陣列） | 建立 logic.c，實作 park_vehicle / exit_vehicle / search_history |
| 13 | 請更新 core/README.md 說明前四層成果，並建立 core/main.c：實作 CLI 介面（解析 argv 指令如 `state`、`park <plate>`、`exit <plate>`、`history <plate>`）、以 JSON 格式輸出結果給 Flask、實作 lot_state.bin 的序列化（fwrite）與反序列化（fread） | 建立 main.c，實作 CLI 介面與 lot_state.bin 持久化 |
| 14 | 請說明 lot_state.bin 這個檔案的用途與必要性。為何 subprocess 架構下需要持久化？（每次 Flask 呼叫 subprocess 都會建立新行程，記憶體中的 grid/Stack/Queue 會消失），並說明序列化時如何處理動態配置的指標 | 說明 subprocess 每次建立新行程導致狀態消失的問題與解法 |
| 15 | 請建立 server/app.py，使用 Flask + flask-cors 實作 4 條 HTTP 路由：`GET /state`（取得停車場狀態）、`POST /park`（入場）、`POST /exit`（出場）、`GET /history`（歷史查詢）。每條路由透過 subprocess.run 呼叫 core/parking 執行檔，解析 stdout 的 JSON 後回傳給前端 | 建立 server/app.py，實作 4 條 HTTP 路由 |
| 16 | 請逐項驗證目前所有功能：(1) C 是否能正確編譯 (2) 4 條 API 路由是否回傳正確 JSON (3) lot_state.bin 是否正確持久化 (4) 入場滿位是否進入候位佇列 (5) 出場是否正確判斷阻擋 (6) 候位車輛是否自動入場 | 逐項驗證所有 API 路由與 C 執行結果 |
| 17 | 目前專案中哪些檔案不應該上傳到 GitHub？請建立 .gitignore，至少排除：C 編譯產物（parking 執行檔、*.o）、狀態檔（lot_state.bin）、Python 虛擬環境（venv/）、Python 快取（__pycache__/、*.pyc）、IDE 設定（.vscode/、.idea/） | 建立 .gitignore（C 執行檔、lot_state.bin、venv、__pycache__） |
| 18 | 請建立 client/ 目錄下的三個檔案：(1) index.html：3×3 停車格地圖區、入場/出場/查詢操作面板 (2) style.css：停車格樣式（佔用/空位顏色區分）、響應式排版 (3) app.js：使用 fetch API 呼叫 Flask 四條路由、動態渲染停車格與候位佇列、處理使用者操作回饋 | 建立 client/ 三檔案，完成停車場地圖與操作面板 |
| 19 | 請進行全面驗收：(1) C 編譯無 warning (2) 所有 API 路由回傳格式正確 (3) 前端 fetch 路徑與 CORS 設定無誤 (4) 狀態持久化跨次呼叫仍正確 (5) 候位佇列與阻擋邏輯符合 LIFO 規則 (6) 歷史查詢支援多筆紀錄 (7) 無記憶體洩漏 | 全面檢查編譯、功能測試、API 路由、前端邏輯 |
| 20 | 請在專案根目錄建立 README.md，內容須包含：專案簡介、功能介紹表格、技術棧表格、C 語言 9 項需求對應表、系統架構 ASCII 圖、目錄結構、四階段啟動步驟（編譯 / 安裝套件 / 啟動 Flask / 開啟前端）、Claude AI Prompt 使用紀錄 | 建立本文件，對整個專案進行完整說明 |
| 21 | 請在 README.md 的專案簡介下方新增 Demo 網址區塊，使用 Markdown 連結格式：[https://brave-field-08b4ef100.7.azurestaticapps.net/](https://brave-field-08b4ef100.7.azurestaticapps.net/)，方便讀者直接點擊體驗 | 新增 Azure Static Web Apps 部署的 Demo 連結 |
| 22 | 請分析並潤稿「Claude AI Prompt 使用紀錄」表格中所有 Prompt 內容，將原本簡短的口語化指令改寫為可讓 AI 直接理解並執行的明確指令，每條應包含：具體的動作（建立/實作/驗證）、目標檔案、實作細節、預期輸出 | 重新潤稿所有 Prompt，提升可重現性與可讀性 |
