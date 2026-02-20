#!/bin/bash
# picam.sh
# Self-contained script to run dual-camera RTSP server on boot

# ssh sdr@192.168.1.86
# scp ~/Dev/Swiftblade/picam.sh sdr@192.168.1.83:/home/sdr/
# sudo nano /boot/firmware/config.txt --> Add gpu_mem=192

# Install GStreamer + RTSP server if missing
#sudo apt update
#sudo apt install -y \
#  gstreamer1.0-tools \
#  gstreamer1.0-plugins-base \
#  gstreamer1.0-plugins-good \
#  gstreamer1.0-plugins-bad \
#  gstreamer1.0-plugins-ugly \
#  gstreamer1.0-rtsp \
#  gir1.2-gst-rtsp-server-1.0 \
#  libgstrtspserver-1.0-0 \
#  python3-gi \
#  python3-gst-1.0

PORT=${1:-8554}
WIDTH=${2:-320}
HEIGHT=${3:-240}
FPS=${4:-30}
BITRATE=${5:-500}  # in kbps

# Detect Pi IP
PI_IP=$(hostname -I | awk '{print $1}')

# Temporary Python RTSP server script
TMP_SCRIPT=$(mktemp /tmp/stereo_picam.XXXX.py)
cat <<EOF > $TMP_SCRIPT
#!/usr/bin/env python3
import gi
gi.require_version('Gst', '1.0')
gi.require_version('GstRtspServer', '1.0')
from gi.repository import Gst, GstRtspServer, GLib

WIDTH = $WIDTH
HEIGHT = $HEIGHT
FPS = $FPS
BITRATE = $BITRATE

Gst.init(None)
server = GstRtspServer.RTSPServer()

def add_camera(device, mount_point):
    factory = GstRtspServer.RTSPMediaFactory()
    launch_str = (
        f"v4l2src device={device} ! "
        f"videoconvert ! "
        f"video/x-raw,format=I420,width={WIDTH},height={HEIGHT},framerate={FPS}/1 ! "
        f"x264enc bitrate={BITRATE} tune=zerolatency speed-preset=ultrafast ! "
        f"rtph264pay name=pay0 pt=96 config-interval=1 mtu=1200"
    )
    factory.set_launch(launch_str)
    factory.set_shared(True)
    server.get_mount_points().add_factory(mount_point, factory)

add_camera("/dev/video0", "/cam")

server.attach(None)
loop = GLib.MainLoop()
loop.run()
EOF

chmod +x $TMP_SCRIPT

# Launch the server
echo "Starting RTSP server on port $PORT..."
echo "Camera: rtsp://$PI_IP:$PORT/cam"
python3 $TMP_SCRIPT