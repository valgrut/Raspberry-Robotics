import math
import numpy as np
from matplotlib import pyplot as plt
from plot import *
from tests import Tests
import time
from m_hexapod import *
from m_kinematics import *
from utils import *

hexapod = Hexapod()
leg = HexapodLeg(hexapod, 0, 5, 6.4, 12)
leg2 = HexapodLeg(hexapod, 1, 5, 6.4, 12)

# Defaultni uhly, pri kterych je noha uplne natazena
#[2] = 100 # Base:  100 = Natazena base presne uprostred
#[1] = 50  # Rameno: 50 = natazene rameno
#[0] = 100 # Loket: 100 = presne 45stupnu dolu od stredni casti nohy


# Testing
test = Tests()
test.run_tests()


###### Experiments and testing ######
# kinematics = Kinematics()
#leg.draw_z_line()
# leg.draw_y_line()
#leg.draw_x_line()


#angles = kinematics.inverse_kinematics(leg, Coords(23.5, 0, 0))
# leg.set_angle(0, angles[0])
# leg.set_angle(1, angles[0])
# leg.set_angle(2, angles[0])

# leg.set_angle(0, 100)  # 0 = elbow
# leg.set_angle(1, 50)  # 1 = shoulder
# leg.set_angle(2, 100)  # 2 = base

# hexapod.interactive_effector_control(0)
hexapod.interactive_angle_control(0)