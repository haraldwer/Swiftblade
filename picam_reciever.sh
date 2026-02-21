#!/bin/bash
# nmap -sn 192.168.1.0/24
# ssh sdr@192.168.1.118
# scp picam_sender.sh sdr@192.168.1.118:/home/sdr/

# Set default port
PORT=${1:-6767}

# Get local IP address
LOCAL_IP=$(ip addr show | grep 'inet ' | grep -v '127.0.0.1' | awk '{print $2}' | cut -d'/' -f1 | head -1)

# Print listening information
echo "Listening on $LOCAL_IP:$PORT..."

# Run GStreamer pipeline
gst-launch-1.0 -v \
    udpsrc port=$PORT \
    caps="application/x-rtp,media=video,clock-rate=90000,encoding-name=H264,payload=96" ! \
    rtph264depay ! \
    avdec_h264 max-threads=1 ! \
    videoconvert ! \
    autovideosink sync=false
