# server — Python Flask 後端

## 檔案

- `app.py` — Flask 應用程式，HTTP 路由與 subprocess 呼叫 C 執行檔

## API 路由

| 方法 | 路由 | 說明 |
|------|------|------|
| GET | `/status` | 停車場目前狀態（grid、停車數、候位數） |
| POST | `/park` | 入場，body：`{"plate": "ABC-1234"}` |
| POST | `/exit` | 出場，body：`{"plate": "ABC-1234"}` |
| GET | `/history/<plate>` | 查詢指定車牌的歷史紀錄 |

## 運作方式

每次收到請求，`call_parking()` 以 subprocess 呼叫 `core/parking` 執行檔，
並固定以 `core/` 為工作目錄，確保 `lot_state.bin` 產生在正確位置。

## 依賴套件

| 套件 | 用途 |
|------|------|
| `flask` | Web 框架，處理 HTTP 路由 |
| `flask-cors` | 允許前端跨來源請求（CORS）；前端與 Flask 不同 port 時必須安裝，否則瀏覽器會擋掉所有請求 |

```bash
pip install flask flask-cors
```

## 安裝與啟動

**步驟一：編譯 C 核心**
```bash
cd core
gcc -o parking parking.c data_structures.c logic.c main.c
```

**步驟二：安裝 Python 套件**
```bash
cd server
pip install flask flask-cors
```

**步驟三：啟動 Flask**
```bash
cd server
python app.py
```

啟動後服務運行於 `http://localhost:5000`
