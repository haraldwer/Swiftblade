#!/bin/bash
# sudo nano /boot/config.txt --> v3d_freq=750

PC_IP=${1:-10.212.14.78}
PORT=${2:-6767}
WIDTH=${3:-640}
HEIGHT=${4:-480}
FPS=${5:-30}

CAMERA="/dev/video0"

# Detect Pi IP
PI_IP=$(hostname -I | awk '{print $1}')
echo "PI IP is $PI_IP"

echo "Starting raw stream to $PC_IP:$PORT..."
rpicam-vid -t 0 \
    --width $WIDTH --height $HEIGHT \
    --framerate $FPS \
    --codec h264 \
    --bitrate 4M \
    --intra 1 \
    --inline \
    --flush \
    --tuning lowlatency \
    --nopreview \
    -o udp://$PC_IP:$PORT?pkt_size=1316

echo "Failed to start raw stream"

echo "Starting stream from USB camera $CAMERA to $PC_IP:$PORT..."
gst-launch-1.0 -v v4l2src device=$CAMERA ! \
    "video/x-raw,width=$WIDTH,height=$HEIGHT,framerate=$FPS/1" ! \
    v4l2h264enc extra-controls="controls,h264_i_frame_period=30,video_bitrate=4000000;" ! \
    "video/x-h264,level=(string)4,profile=baseline" ! \
    rtph264pay config-interval=1 pt=96 ! \
    udpsink host=$PC_IP port=$PORT sync=false async=false

