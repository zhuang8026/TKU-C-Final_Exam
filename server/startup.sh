#!/bin/bash
chmod +x /home/site/wwwroot/core/parking
cd /home/site/wwwroot/server
exec gunicorn --bind=0.0.0.0:8000 app:app
