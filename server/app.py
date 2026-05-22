import subprocess
import json
import os
from flask import Flask, request, jsonify
from flask_cors import CORS

app = Flask(__name__)
CORS(app)  # 允許前端跨來源請求

# C 執行檔與狀態檔的路徑，以 app.py 位置為基準往上找 core/
BASE_DIR    = os.path.dirname(os.path.abspath(__file__))
CORE_DIR    = os.path.join(BASE_DIR, '..', 'core')
PARKING_BIN = os.path.join(CORE_DIR, 'parking')

def call_parking(*args):
    """呼叫 C 執行檔，傳入指令與參數，回傳解析後的 JSON 結果。
    cwd 固定為 CORE_DIR，確保 lot_state.bin 產生在正確位置。"""
    result = subprocess.run(
        [PARKING_BIN] + list(args),
        capture_output=True,
        text=True,
        cwd=CORE_DIR
    )
    try:
        return json.loads(result.stdout)
    except (json.JSONDecodeError, ValueError):
        return {'error': 'core error', 'detail': result.stderr.strip()}

# ── Routes ──

@app.route('/status', methods=['GET'])
def status():
    """回傳目前停車場狀態：3×3 grid、停車數、候位數"""
    return jsonify(call_parking('status'))

@app.route('/park', methods=['POST'])
def park():
    """入場：body { "plate": "ABC-1234" }
    回傳 parked（停車成功）或 queued（候位）"""
    data  = request.get_json(silent=True) or {}
    plate = data.get('plate', '').strip()
    if not plate:
        return jsonify({'error': 'missing plate'}), 400
    return jsonify(call_parking('park', plate))

@app.route('/exit', methods=['POST'])
def exit_vehicle():
    """出場：body { "plate": "ABC-1234" }
    回傳 exited、not found 或 blocked（含阻擋車牌清單）"""
    data  = request.get_json(silent=True) or {}
    plate = data.get('plate', '').strip()
    if not plate:
        return jsonify({'error': 'missing plate'}), 400
    return jsonify(call_parking('exit', plate))

@app.route('/history/<plate>', methods=['GET'])
def history(plate):
    """查詢指定車牌的所有歷史紀錄，回傳陣列"""
    return jsonify(call_parking('history', plate))

if __name__ == '__main__':
    app.run(debug=True, port=5000)
