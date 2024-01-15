import math
from plot import *
from tests import Tests
import time
from hexapod import Hexapod, HexapodLeg, SharedLegParameters
from coords import Coords


hexapod = Hexapod()

shared_legs_params = SharedLegParameters(5.1, 6.5, 12.1, 130)
leg_FL = HexapodLeg(hexapod, 0, Coords(0, 0, 0), shared_legs_params)
leg_FL.set_inversed()

# TODO: Nastavit pro FL, FR, BL, BR nohy 45 stupnu offset, abych mohl normalne pouzivat x a y souradnice
# a nemusel slozite hledat, jake jsou xy souradnice, kdyz chci nohu dat dopredu. takhle to bude proste y.
#
# melo by to byt, ze IK spocita uhly, a ja tomu proste pridam/odectu 45 stupnu pro base angle.

leg_FR = HexapodLeg(hexapod, 1, Coords(0, 0, 0), shared_legs_params)
# leg_ML = HexapodLeg(hexapod, 2, Coords(0, 0, 0), shared_legs_params)
# leg_ML.set_inversed()
# leg_MR = HexapodLeg(hexapod, 3, Coords(0, 0, 0), shared_legs_params)
# leg_BL = HexapodLeg(hexapod, 4, Coords(0, 0, 0), shared_legs_params)
# leg_BL.set_inversed()
# leg_BR = HexapodLeg(hexapod, 5, Coords(0, 0, 0), shared_legs_params)


# Testing
#test = Tests()
#test.run_tests(leg_FL)


###### Experiments and testing ######
# kinematics = Kinematics()
#leg.draw_z_line()
# leg.draw_y_line()

# while True:
#     leg_FL.draw_y_line(10, -10, increment=0.1, speed=0.01)
#     leg_FL.draw_y_line(-10, 10, increment=0.1, speed=0.01)
#     leg_FL.draw_x_line(14, 22, increment=0.1, speed=0.01)
#     leg_FL.draw_x_line(22, 14, increment=0.1, speed=0.01)
#     leg_FL.draw_z_line(0, 10, increment=0.1, speed=0.01)
#     leg_FL.draw_z_line(10, 0, increment=0.1, speed=0.01)


#hexapod.interactive_effector_control(0)
# hexapod.interactive_angle_control(0)

start_coord = Coords(6, 17, -5)
target_coord = Coords(17, 10, -5)
leg_FL.move_to_point(start_coord)
while True:
    leg_FL.swing_from_point_to_point(start_coord, target_coord, num_of_points=10, max_step_height=7, increment=0.1, speed=0.1)
    time.sleep(0.05)
    leg_FL.stance_from_point_to_point(target_coord, start_coord, num_of_points=10, increment=0.1, speed=0.1)
    time.sleep(0.05)

    leg_FR.swing_from_point_to_point(start_coord, target_coord, num_of_points=10, max_step_height=7, increment=0.1, speed=0.1)
    time.sleep(0.05)
    leg_FR.stance_from_point_to_point(target_coord, start_coord, num_of_points=10, increment=0.1, speed=0.1)
    time.sleep(0.05)
