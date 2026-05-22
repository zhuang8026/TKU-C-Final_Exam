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

## 啟動方式

```bash
# 確認 core/parking 已編譯
cd core && gcc -o parking parking.c data_structures.c logic.c main.c

# 啟動 Flask
cd server && python app.py
```
