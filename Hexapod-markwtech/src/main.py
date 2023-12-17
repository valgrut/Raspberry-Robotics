import math
from plot import *
from tests import Tests
import time
from m_hexapod import *
from m_kinematics import *
from utils import *

hexapod = Hexapod()

shared_legs_config = None
leg_FL = HexapodLeg(hexapod, 0, Coords(0, 0, 0), 5.1, 6.5, 12.1)
leg_FR = HexapodLeg(hexapod, 1, Coords(0, 0, 0), 5.1, 6.5, 12.1)


# Testing
#test = Tests()
#test.run_tests(leg_FL)


###### Experiments and testing ######
# kinematics = Kinematics()
#leg.draw_z_line()
# leg.draw_y_line()
while True:
    leg_FL.draw_y_line(10, -10, increment=0.1, speed=0.01)
    leg_FL.draw_y_line(-10, 10, increment=0.1, speed=0.01)
    leg_FL.draw_x_line(14, 22, increment=0.1, speed=0.01)
    leg_FL.draw_x_line(22, 14, increment=0.1, speed=0.01)
    leg_FL.draw_z_line(0, 10, increment=0.1, speed=0.01)
    leg_FL.draw_z_line(10, 0, increment=0.1, speed=0.01)


# hexapod.interactive_effector_control(0)
# hexapod.interactive_angle_control(0)