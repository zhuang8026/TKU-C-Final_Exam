#!/bin/bash
cd /home/site/wwwroot/core
gcc -o parking parking.c data_structures.c logic.c main.c -lm
cd /home/site/wwwroot/server
gunicorn --bind=0.0.0.0:8000 app:app