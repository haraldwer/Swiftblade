#!/bin/bash
# sudo nano /boot/config.txt --> v3d_freq=750

PC_IP=${1:-192.168.1.226}
PORT=${2:-6767}
WIDTH=${3:-640}
HEIGHT=${4:-480}
FPS=${5:-30}

# Detect Pi IP
PI_IP=$(hostname -I | awk '{print $1}')
echo "PI IP is $PI_IP"

cleanup() {
    echo -e "\nStopping all streams..."
    for pid in "${STREAM_PIDS[@]}"; do
        if ps -p $pid > /dev/null 2>&1; then
            kill $pid 2>/dev/null
        fi
    done
    echo "All streams stopped."
    exit 0
}

STREAM_PIDS=()
trap cleanup SIGINT SIGTERM

VALID_CAMERAS=()
for device in /dev/video*; do
    if [ -e "$device" ] && [ -r "$device" ]; then
        VALID_CAMERAS+=("$device")
    fi
done

if [ ${#VALID_CAMERAS[@]} -eq 0 ]; then
    echo "No valid cameras found!"
    echo "Available devices:"
    ls -l /dev/video* 2>/dev/null || echo "No video devices found"
    exit 1
fi

echo "Found ${#VALID_CAMERAS[@]} valid camera(s):"
for i in "${!VALID_CAMERAS[@]}"; do
    echo "  Camera $i: ${VALID_CAMERAS[$i]}"
done

# Stream each valid camera
for i in "${!VALID_CAMERAS[@]}"; do
    CAMERA="${VALID_CAMERAS[$i]}"
    STREAM_PORT=$((PORT + i))
    STREAM_NAME="cam$i"
    
    echo "Starting stream $STREAM_NAME from $CAMERA to $PC_IP:$STREAM_PORT..."
    
    gst-launch-1.0 -v v4l2src device=$CAMERA ! \
        "video/x-raw,width=$WIDTH,height=$HEIGHT,framerate=$FPS/1" ! \
        v4l2h264enc extra-controls="controls,h264_i_frame_period=30,video_bitrate=4000000;" ! \
        "video/x-h264,level=(string)4,profile=baseline" ! \
        rtph264pay config-interval=1 pt=96 ! \
        udpsink host=$PC_IP port=$STREAM_PORT sync=false async=false > /dev/null &

    # Store the PID
    STREAM_PIDS+=($!)

    # Add small delay between starting streams
    sleep 0.5
done

echo "All streams started. Access cameras at:"
for i in "${!VALID_CAMERAS[@]}"; do
    echo "  $PC_IP:$(($PORT + i)) - cam$i"
done
echo "Press Ctrl+C to stop all streams"
wait

exit

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
