#!/bin/bash
gunicorn --bind=0.0.0.0:8000 --chdir /home/site/wwwroot/server app:app
