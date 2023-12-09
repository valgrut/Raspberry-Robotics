import math
import numpy as np
from matplotlib import pyplot as plt
from plot import *
from tests import Tests
import time
from m_hexapod import *
from m_kinematics import *
from utils import *

# https://technology.cpm.org/general/3dgraph/


kinematics = Kinematics()
kit = ServoKit(channels=8)

hexapod = Hexapod()
leg = HexapodLeg(hexapod, 0, 5, 6.5, 12)

###### Experiments and testing ######
test = Tests()
test.run_fk_tests()


########
# for x in range(10, 20):
#     print(kinematics.ik(leg, Coords(x, 0, 10)))
#     angles = kinematics.ik(leg, Coords(x, 0, 10))
#     kit.servo[0].angle = angles[0]
#     kit.servo[1].angle = angles[1]
#     kit.servo[2].angle = angles[2]
#     time.sleep(10)

########
# for y in range(5, 15):
#     print(kinematics.ik(leg, Coords(10, y, 0)))
#     print(kinematics.ik_dle_clanku(leg, Coords(10, y, 0)))

# print()
# print(kinematics.ik(leg, Coords(10, 10, -5)))
# print(kinematics.ik_dle_clanku(leg, Coords(10, 10, -5)))

#a0 = 100 # Base:  100 = Natazena base presne uprostred
#a1 = 50  # Rameno: 50 = natazene rameno
#a2 = 100 # Loket: 100 = presne 45stupnu dolu od stredni casti nohy
# TODO: toto by mely byt uhly pro 0 stupnu, tzn namapovat na range (-90, 90)
#kit.servo[0].angle = a2
#kit.servo[1].angle = a1
#kit.servo[2].angle = a0

# print("ik", kinematics.ik_dle_clanku(leg, Coords(10, 10, 0)))
# print(kinematics.ik_dle_clanku(leg, Coords(10, 0, 5)))
#print(kinematics.ik_dle_clanku(leg, Coords(23.5, 0, 0)))



########
# for z in range(10, 20):
#     print(kinematics.ik(leg, Coords(0, 0, z)))

########
# plotter = Plotter()
# print("kruh")
# for x in np.arange(10, 20, 0.5):
    # plotter.add_point((10, 5*math.cos(x), 5*math.sin(x)))
    # plotter.add_point(kinematics.ik(leg, Coords(10, 5*math.cos(x), 5*math.sin(x))))
# plotter.plot_points()
# plotter.empty_points()

#########
# print("cara")
# for x in np.arange(10, 17, 0.5):
#     print(kinematics.ik(leg, Coords(x, 0, 10)))
#     plotter.add_point(kinematics.ik(leg, Coords(x, 0, 10)))
# plotter.plot_points()

# kit.servo[0].angle =


