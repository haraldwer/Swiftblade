#!/bin/bash
# sudo nano /boot/config.txt --> v3d_freq=750

exec > >(tee -a /home/sdr/picam_sender.log) 2>&1
echo "Script started at $(date)"

# Configuration
PC_IP=${1:-192.168.1.213}
PORT=${2:-6767}
WIDTH=${3:-640}
HEIGHT=${4:-480}
FPS=${5:-30}

# Detect Pi IP
PI_IP=$(hostname -I | awk '{print $1}')
echo "PI IP is $PI_IP"

# Global variables
STREAM_PIDS=()
VALID_CAMERAS=()
PREVIOUS_CAMERAS=()

# Cleanup function
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

# Function to detect valid cameras
detect_cameras() {
    local cameras=()
    for device in /dev/video*; do
        if [ -e "$device" ] && [ -r "$device" ]; then
            if v4l2-ctl --device="$device" --all 2>/dev/null | grep -q "Video Capture"; then
                cameras+=("$device")
            fi
        fi
    done
    echo "${cameras[@]}"
}

# Function to start stream for a camera
start_stream() {
    local camera=$1
    local index=$2
    local port=$((PORT + index))
    local stream_name="cam$index"
    
    echo "Starting stream $stream_name from $camera to $PC_IP:$port..."
    gst-launch-1.0 -v v4l2src device=$camera ! \
        "video/x-raw,width=$WIDTH,height=$HEIGHT,framerate=$FPS/1" ! \
        v4l2h264enc extra-controls="controls,h264_i_frame_period=30,video_bitrate=4000000;" ! \
        "video/x-h264,level=(string)4,profile=baseline" ! \
        rtph264pay config-interval=1 pt=96 ! \
        udpsink host=$PC_IP port=$port sync=false async=false > /dev/null &

    echo $!  # Return PID
}

# Function to update camera streams
update_streams() {
    local current_cameras=($(detect_cameras))
    
    # Check for new cameras
    for i in "${!current_cameras[@]}"; do
        local camera="${current_cameras[$i]}"
        local found=false
        
        for j in "${!VALID_CAMERAS[@]}"; do
            if [ "${VALID_CAMERAS[$j]}" = "$camera" ]; then
                found=true
                break
            fi
        done
        
        if [ "$found" = false ]; then
            # New camera detected
            echo "New camera detected: $camera"
            local new_pid=$(start_stream "$camera" ${#VALID_CAMERAS[@]})
            STREAM_PIDS+=($new_pid)
            VALID_CAMERAS+=("$camera")
        fi
    done
    
    # Check for removed cameras
    for i in "${!VALID_CAMERAS[@]}"; do
        local camera="${VALID_CAMERAS[$i]}"
        local found=false
        
        for j in "${!current_cameras[@]}"; do
            if [ "${current_cameras[$j]}" = "$camera" ]; then
                found=true
                break
            fi
        done
        
        if [ "$found" = false ]; then
            # Camera removed
            echo "Camera removed: $camera"
            # Remove from arrays (simplified approach)
            VALID_CAMERAS=("${VALID_CAMERAS[@]/$camera}")
        fi
    done
    
    # Update previous cameras
    PREVIOUS_CAMERAS=("${current_cameras[@]}")
}

# Main execution
trap cleanup SIGINT SIGTERM

# Initial camera detection
echo "Detecting cameras..."
VALID_CAMERAS=($(detect_cameras))

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

# Start initial streams
echo "Starting initial streams..."
for i in "${!VALID_CAMERAS[@]}"; do
    pid=$(start_stream "${VALID_CAMERAS[$i]}" $i)
    STREAM_PIDS+=($pid)
done

echo "All streams started. Access cameras at:"
for i in "${!VALID_CAMERAS[@]}"; do
    echo "  $PC_IP:$(($PORT + i)) - cam$i"
done

# Monitor for camera changes
echo "Monitoring for camera changes... (Press Ctrl+C to stop)"
PREVIOUS_CAMERAS=("${VALID_CAMERAS[@]}")

while true; do
    sleep 2
    update_streams
done

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
