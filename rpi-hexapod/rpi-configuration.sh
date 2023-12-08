# Install all required packages and dependencies

SUDO=''
SUDO='sudo' # Comment/uncomment as needed.

# Useful Command:
#   libcamera-still --list-cameras
#   v4l2-ctl --info -d /dev/video0 --list-formats-ext

install_common_packages() {
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get full-upgrade
    sudo reboot

    sudo apt-get install vim

    # sudo apt-get install --reinstall libraspberrypi0 libraspberrypi-dev libraspberrypi-doc libraspberrypi-bin

    # libcamera works!!!!!
    #   libcamera-still -o test.jpg
    # https://www.raspberrypi.com/documentation/computers/camera_software.html#getting-started
    # https://datasheets.raspberrypi.com/camera/picamera2-manual.pdf
    #
    # Use apt to install picamera2, becase pip causes combatibility problems!!!
    sudo apt install -y python3-picamera2
    sudo apt install -y python3-libcamera python3-kms++
    sudo apt install -y python3-prctl libatlas-base-dev ffmpeg python3-pip
    sudo apt install -y python3-pyqt5 python3-opengl # only if you want GUI features

    # OpenCV2 is not required for picamera2, but can be handy to process data
    sudo apt install -y python3-opencv
    sudo apt install -y opencv-data
    pip3 install tflite-runtime
    sudo apt install -y ffmpeg

    # GStream
    # sudo apt-get install libx264-dev libjpeg-dev
    # sudo apt-get install libgstreamer1.0-dev \
    #     libgstreamer-plugins-base1.0-dev \
    #     libgstreamer-plugins-bad1.0-dev \
    #     gstreamer1.0-plugins-ugly \
    #     gstreamer1.0-tools \
    #     gstreamer1.0-gl \
    #     gstreamer1.0-gtk3
    # if you have Qt5 install this plugin
    # sudo apt-get install gstreamer1.0-qt5

    sudo apt-get install python3-flask

    # cd /home/hexapod/Code/rpi-hexapod

    $SUDO pip3 install --break-system-packages numpy --upgrade
    $SUDO pip3 install --break-system-packages matplotlib

    $SUDO pip3 install --break-system-packages adafruit-circuitpython-pca9685
    $SUDO pip3 install --break-system-packages adafruit-circuitpython-servokit
}

install_64bit_packages() {
    install_common_packages()
}

install_32bit_packages() {
    install_common_packages()

    # $SUDO pip3 install --break-system-packages picamera

    # $SUDO pip3 install --break-system-packages opencv-contrib-python
    # $SUDO pip3 install --break-system-packages imutils
    # $SUDO pip3 install --break-system-packages opencv-python
}

if [[ "$1" == "32bit" ]]; then
    install_32bit_packages()
elif [[ "$1" == "64bit" ]]; then
    install_64bit_packages()
fi

## ======= Troubleshooting =======
# Error: OSError: libmmal.so: cannot open shared object file: No such file or directory while trying to run picamera on Raspbian
#   Duvod: picamera is not supported on 64bit system yet
#       pip3 install --break-system-packages picamera
#   Alternatives: picamera2, ffmpeg, openCV
#       pip3 install --break-system-packages picamera2
#
# Error: GStreamer warning: Embedded video playback halted; module v4l2src7 reported: Failed to allocate required memory
#   Duvod: nevim, neco s parametrama kamery asi
