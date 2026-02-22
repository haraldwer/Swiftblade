@echo off
:: nmap -sn 192.168.1.0/24
:: ssh sdr@192.168.1.118
:: scp picam_sender.sh sdr@192.168.1.118:/home/sdr/

:: Define the path to GStreamer
set "PORT=%~1"
if "%PORT%"=="" set "PORT=6767"

:: Find local IP
for /f "tokens=2 delims=:" %%a in ('ipconfig ^| findstr /c:"IPv4 Address"') do (
    set "LOCAL_IP=%%a"
)
:: Trim the leading space that ipconfig adds
set "LOCAL_IP=%LOCAL_IP:~1%"

echo Listening on %LOCAL_IP%:%PORT%...

gst-launch-1.0 -v ^
    udpsrc port=%PORT% ^
    caps="application/x-rtp,media=video,clock-rate=90000,encoding-name=H264,payload=96" ! ^
    rtph264depay ! ^
    avdec_h264 max-threads=1 ! ^
    videoconvert ! ^
    autovideosink sync=false

pause