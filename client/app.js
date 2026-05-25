const API = 'http://127.0.0.1:5000';

/* ── 停車格地圖 ── */

async function fetchStatus() {
  const res  = await fetch(`${API}/status`);
  const data = await res.json();
  renderGrid(data);
}

function renderGrid(data) {
  const grid = document.getElementById('grid');
  grid.innerHTML = '';

  const rowLabels = ['A', 'B', 'C'];

  data.grid.forEach((row, rowIdx) => {
    // 行車道（第一排前不加）
    if (rowIdx > 0) {
      const lane = document.createElement('div');
      lane.className = 'lane-strip';
      const center = document.createElement('div');
      center.className = 'lane-center';
      lane.appendChild(center);
      grid.appendChild(lane);
    }

    const rowGroup = document.createElement('div');
    rowGroup.className = 'row-group';

    // 排標籤（A / B / C）
    const label = document.createElement('div');
    label.className = 'row-label';
    label.textContent = rowLabels[rowIdx] ?? (rowIdx + 1);
    rowGroup.appendChild(label);

    const spotsRow = document.createElement('div');
    spotsRow.className = 'spots-row';

    row.forEach((spot, colIdx) => {
      const div = document.createElement('div');
      div.className = 'spot ' + (spot.occupied ? 'occupied' : 'empty');

      if (spot.occupied) {
        const car = document.createElement('div');
        car.className = 'car-top';
        const badge = document.createElement('span');
        badge.className = 'plate-badge';
        badge.textContent = spot.plate;
        div.appendChild(car);
        div.appendChild(badge);
      } else {
        const num = document.createElement('span');
        num.className = 'spot-num';
        num.textContent = `${rowLabels[rowIdx] ?? (rowIdx + 1)}${colIdx + 1}`;
        div.appendChild(num);
      }

      spotsRow.appendChild(div);
    });

    rowGroup.appendChild(spotsRow);
    grid.appendChild(rowGroup);
  });

  document.getElementById('stats').textContent =
    `停車：${data.parked} / 9　　候位：${data.waiting}`;
}

/* ── 入場 ── */

async function parkVehicle() {
  const plate = document.getElementById('park-plate').value.trim();
  if (!plate) return;

  const res  = await fetch(`${API}/park`, {
    method:  'POST',
    headers: { 'Content-Type': 'application/json' },
    body:    JSON.stringify({ plate })
  });
  const data = await res.json();

  if (data.status === 'parked')
    showResult(`${data.plate} 已停入`, 'success');
  else if (data.status === 'queued')
    showResult(`${data.plate} 車位已滿，已加入候位`, 'warning');
  else
    showResult(data.error || '未知錯誤', 'error');

  fetchStatus();
}

/* ── 出場 ── */

async function exitVehicle() {
  const plate = document.getElementById('exit-plate').value.trim();
  if (!plate) return;

  const res  = await fetch(`${API}/exit`, {
    method:  'POST',
    headers: { 'Content-Type': 'application/json' },
    body:    JSON.stringify({ plate })
  });
  const data = await res.json();

  if (data.status === 'exited')
    showResult(`${data.plate} 已出場`, 'success');
  else if (data.error === 'blocked')
    showResult(`${data.plate} 無法出場，請先移出：\n${data.blocking.join('、')}`, 'warning');
  else if (data.error === 'not found')
    showResult(`${data.plate} 不在場內`, 'error');
  else
    showResult(data.error || '未知錯誤', 'error');

  fetchStatus();
}

/* ── 歷史查詢 ── */

async function queryHistory() {
  const plate = document.getElementById('history-plate').value.trim();
  if (!plate) return;

  const res  = await fetch(`${API}/history/${plate}`);
  const data = await res.json();

  if (!Array.isArray(data) || data.length === 0) {
    showResult(`${plate} 無歷史紀錄`, 'warning');
    return;
  }

  const lines = data.map((r, i) => {
    const entry = new Date(r.entry * 1000).toLocaleString();
    const exit  = new Date(r.exit  * 1000).toLocaleString();
    return `#${i + 1}  入場：${entry}\n     出場：${exit}`;
  });

  showResult(`${plate} 歷史紀錄：\n\n${lines.join('\n\n')}`, 'success');
}

/* ── 結果顯示輔助 ── */

function showResult(msg, type) {
  const el = document.getElementById('result');
  el.textContent  = msg;
  el.className    = `result ${type}`;
}

/* 頁面載入時自動取得狀態 */
fetchStatus();
