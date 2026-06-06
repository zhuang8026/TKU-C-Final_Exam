#!/bin/bash
set -e

# 自我定位：不論 Oryx 把 app 解壓到 /tmp/<hash>/ 或 /home/site/wwwroot/ 都能用
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# C 執行檔加上執行權限（artifact 上傳會丟失權限位）
chmod +x "$APP_ROOT/core/parking" 2>/dev/null || true

# Oryx 把 venv 解壓到 /tmp/<hash>/antenv，找出來啟用
VENV=$(find /tmp -maxdepth 2 -name antenv -type d 2>/dev/null | head -n1)
if [ -n "$VENV" ]; then
  source "$VENV/bin/activate"
elif [ -d "$APP_ROOT/antenv" ]; then
  source "$APP_ROOT/antenv/bin/activate"
fi

cd "$SCRIPT_DIR"
exec gunicorn --bind=0.0.0.0:8000 --timeout 600 app:app
