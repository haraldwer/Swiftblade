#!/bin/bash
# sudo nano /boot/config.txt --> v3d_freq=750

exec > >(tee -a /home/sdr/picam_sender.log) 2>&1
echo " ##############"
echo "Script started at $(date)"

# Configuration
PC_IP=${1:-192.168.1.213}
PORT=${2:-6767}
WIDTH=${3:-640}
HEIGHT=${4:-480}
FPS=${5:-60}

# Detect Pi IP
PI_IP=$(hostname -I | awk '{print $1}')
echo "PI IP is $PI_IP"

echo "Starting raw stream to $PC_IP:$PORT..."
rpicam-vid -t 0 \
    --camera 0 \
    --width $WIDTH --height $HEIGHT \
    --framerate $FPS \
    --codec mjpeg \
    --bitrate 4M \
    --intra 1 \
    --inline \
    --flush \
    --nopreview \
    -o udp://$PC_IP:$PORT?pkt_size=1316 & 

PORT_2=$((PORT + 1))
echo "Starting raw stream to $PC_IP:$PORT_2..."
rpicam-vid -t 0 \
    --camera 1 \
    --width $WIDTH --height $HEIGHT \
    --framerate $FPS \
    --codec mjpeg \
    --bitrate 4M \
    --intra 1 \
    --inline \
    --flush \
    --nopreview \
    -o udp://$PC_IP:$PORT_2?pkt_size=1316