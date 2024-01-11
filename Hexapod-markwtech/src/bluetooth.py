#import evdev
from evdev import InputDevice, categorize, ecodes

# sudo apt-get install joystick
#
# Xbox-controller MAC: 14:CB:65:B4:63:A4
#
# How to connect controller:
#  sudo bluetoothctl
#  power on
#  agent on
#  default-agent
#  scan on  # find MAC of the controller
#  #Turn on Xbox controller and for 3 sec hold pairing button '(((' on the front
#  remove <MACASDDRESS> #IF failing (Failed to pair: org.bluez.Error.ConnectionAttemptFailed)
#  trust <MACADDRESS>
#  pair <MACADDRESS>
#  connect <MACADDRESS>
#
# Graphical -> Bluetooth -> XBox Controller -> connect

# ISSUE: if connection switching between yes/no/yes/no...
#  https://github.com/atar-axis/xpadneo
#  cd /tmp/
#  git clone https://github.com/atar-axis/xpadneo.git
#  cd xpadneo
#  sudo apt-get install dkms raspberrypi-kernel-headers
#  ./install.sh
#
# Then
#   sudo vim /etc/modprobe.d/bluetooth.conf
#   options bluetooth disable_ertm=Y
#       nebo???
#   options bluetooth disable\_ertm=Y
# 
# echo '1' > /sys/module/bluetooth/parameters/disable_ertm


# Test:
#  sudo jstest /dev/input/js0

#creates object 'gamepad' to store the data
#you can call it whatever you like
gamepad = InputDevice('/dev/input/event2')

#prints out device info at start
print(gamepad)

#evdev takes care of polling the controller in a loop
for event in gamepad.read_loop():
    print(categorize(event))
