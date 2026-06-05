#!/bin/bash
set -e

# C 執行檔加上執行權限（artifact 上傳會丟失權限位）
chmod +x /home/site/wwwroot/core/parking 2>/dev/null || true

# Oryx 把 venv 解壓到 /tmp/<hash>/antenv，找出來啟用
VENV=$(find /tmp -maxdepth 2 -name antenv -type d 2>/dev/null | head -n1)
if [ -n "$VENV" ]; then
  source "$VENV/bin/activate"
elif [ -d /home/site/wwwroot/antenv ]; then
  source /home/site/wwwroot/antenv/bin/activate
fi

cd /home/site/wwwroot/server
exec gunicorn --bind=0.0.0.0:8000 --timeout 600 app:app
