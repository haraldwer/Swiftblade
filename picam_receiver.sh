#!/bin/bash

# On PC:
# nmap -sn 192.168.1.0/24
# scp picam_sender.sh sdr@192.168.1.118:/home/sdr/
# scp picam_sender.sh sdr@192.168.1.118:/home/sdr/.config/autostart/picam_sender.sh
# ssh sdr@192.168.1.118

# On PI
# tail -f picam_sender.log
# cat picam_sender.log


# Set default starting port
START_PORT=${1:-6767}
# Number of ports to check
PORT_COUNT=${2:-2}

# Get local IP address
LOCAL_IP=$(ip addr show | grep 'inet ' | grep -v '127.0.0.1' | awk '{print $2}' | cut -d'/' -f1 | head -1)

# Function to clean up GStreamer processes on exit
cleanup() {
    echo "Cleaning up..."
    pkill -f gst-launch-1.0
    exit 0
}

# Trap Ctrl+C to clean up
trap cleanup SIGINT

# Loop through ports
for ((port=START_PORT; port<START_PORT+PORT_COUNT; port++)); do
    ffplay -i udp://@:$port -fflags nobuffer &

    #echo "Listening on $LOCAL_IP:$port..."
    #gst-launch-1.0 -v \
    #    udpsrc port=$port \
    #    caps="application/x-rtp,media=video,clock-rate=90000,encoding-name=H264,payload=96" ! \
    #    rtph264depay ! \
    #    avdec_h264 max-threads=1 ! \
    #    videoconvert ! \
    #    autovideosink sync=false &
done

# Wait for all background processes
wait