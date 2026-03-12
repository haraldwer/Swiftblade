@echo off
cd /d %~1
start "Engine" cmd /k "python -m http.server 8080"
timeout /t 1 /nobreak >nul
start http://localhost:8080/Application.html