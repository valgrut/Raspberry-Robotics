import math
import numpy as np
from matplotlib import pyplot as plt
from plot import *
from tests import Tests
import time
from m_hexapod import *
from m_kinematics import *
from utils import *


kinematics = Kinematics()
#kit = ServoKit(channels=8)

hexapod = Hexapod()
leg = HexapodLeg(hexapod, 0, 5.5, 6.4, 12)


# Defaultni uhly, pri kterych je noha uplne natazena
#[2] = 100 # Base:  100 = Natazena base presne uprostred
#[1] = 50  # Rameno: 50 = natazene rameno
#[0] = 100 # Loket: 100 = presne 45stupnu dolu od stredni casti nohy
# TODO: V IK proste pro (0,0,0) odecist/pricist takove hodnoty, aby natazena noha byla a0=100, a1=50, ?a2=100?


###### Experiments and testing ######
test = Tests()
test.run_fk_tests()

# leg.draw_z_line()
# leg.draw_y_line()
# leg.draw_x_line()


#angles = kinematics.inverse_kinematics(leg, Coords(23.5, 0, 0))
# leg.set_angle(0, angles[0])
# leg.set_angle(1, angles[0])
# leg.set_angle(2, angles[0])

# leg.set_angle(0, 100)  # 0 = elbow
# leg.set_angle(1, 50)  # 1 = shoulder
# leg.set_angle(2, 100)  # 2 = base

#hexapod.interactive_effector_control(0)