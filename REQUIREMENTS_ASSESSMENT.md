# 九項需求評估

| 需求 | 評估 | 落地方式 |
|------|------|---------|
| **struct** | ✅ 完全自然 | `Vehicle`、`ParkingSpot`、`LotState`、`StackNode`、`QueueNode`、`Record` |
| **malloc / free** | ✅ 完全自然 | Stack / Queue / LinkedList 每個節點動態配置；入場 `malloc`，出場 `free` |
| **pointer** | ✅ 完全自然 | 雙層指標建 grid、傳址呼叫、`*next` 串接、`->` 取成員 |
| **2D array** | ✅ 自然 | `ParkingSpot grid[3][3]`，視覺化停車格地圖 |
| **stack** | ✅ 完全自然 | 整個停車場 LIFO，對應單排車道，`stack_push` / `stack_pop` |
| **queue** | ✅ 自然 | 候位佇列，車位滿時 FIFO 排隊，`queue_enqueue` / `queue_dequeue` |
| **linked list** | ✅ 完全自然 | 歷史紀錄鏈結串列，每次進出寫一筆 `Record` 節點 |
| **dynamic array** | ✅ 有設計理由 | 查詢歷史時結果數量未知，走訪 linked list 將符合紀錄收集進 `realloc` 動態擴充的陣列 |
| **函式拆分** | ✅ 完全自然 | 每個操作獨立函式，`main()` 只做流程控制 |

---

## 備註

**Dynamic array 說明：**
不是主資料結構，是查詢功能的結果緩衝層。使用者輸入車牌後，走訪 linked list 將符合的紀錄收集進動態陣列，因結果數量事先未知，需 `realloc` 動態擴充，用後即 `free`。

---

*最後更新：2026-05-22*
