# Install all required packages and dependencies

sudo apt-get update
sudo apt-get upgrade
sudo apt-get full-upgrade
sudo reboot

sudo apt-get install python3-flask

cd /home/hexapod/Code/rpi-hexapod

# Useful Command: 
#   libcamera-still --list-cameras
#   v4l2-ctl --info -d /dev/video0 --list-formats-ext

sudo apt install -y python3-libcamera python3-kms++
sudo apt install -y python3-pyqt5 python3-prctl libatlas-base-dev ffmpeg python3-pip

pip3 install numpy --upgrade
pip3 install matplotlib

pip3 install picamera2

pip3 install --break-system-packages opencv-python
sudo apt install -y python3-opencv
sudo apt install -y opencv-data


sudo pip3 install --break-system-packages opencv-contrib-python
sudo pip3 install --break-system-packages imutils
sudo pip3 install --break-system-packages opencv-python


sudo pip3 install adafruit-circuitpython-pca9685
sudo pip3 install adafruit-circuitpython-servokit
pip3 install adafruit-circuitpython-servokit


## ======= Troubleshooting =======
# Error: OSError: libmmal.so: cannot open shared object file: No such file or directory while trying to run picamera on Raspbian
#   Duvod: picamera is not supported on 64bit system yet
#       pip3 install --break-system-packages picamera
#   Alternatives: picamera2, ffmpeg, openCV
#       pip3 install --break-system-packages picamera2
#
# Error: GStreamer warning: Embedded video playback halted; module v4l2src7 reported: Failed to allocate required memory
#   Duvod: nevim, neco s parametrama kamery asi
