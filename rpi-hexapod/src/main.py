import math
import numpy as np
from matplotlib import pyplot as plt
from plot import *
from adafruit_servokit import ServoKit
import time
from hexapod import *
from kinematics import *

# https://technology.cpm.org/general/3dgraph/

def map_range(v, a, b, c, d):
       return (v-a) / (b-a) * (d-c) + c
# a = map_range(4, 0, 10, 0, 1)


# leg = HexapodLeg(0, None, 5, 10, 6.5, 12)
leg = HexapodLeg(0, None, 0, 5, 6.5, 12)
kinematics = Kinematics()
kit = ServoKit(channels=8)


def control_position():
    # xyz pozice na relativne normalni pozici nohou:
    init_x = 7 #75
    init_y = 1 #120
    init_z = 8 #100

    angles = None
    old_angles = None

    increment = 1 #10
    cmd = ""
    while cmd != "e":
        cmd = input()
        if cmd == "w":
            init_x -= increment
        elif cmd == "s":
            init_x += increment

        if cmd == "a":
            init_y += increment
        elif cmd == "d":
            init_y -= increment

        if cmd == "r":
            init_z += increment
        elif cmd == "f":
            init_z -= increment

        # init_x = init_x / 10
        # init_y = init_y / 10
        # init_z = init_z / 10

        print("new x", init_z, "small_x", init_z)
        print("new y", init_y, "small_y", init_y)
        print("new z", init_x, "small_z", init_x)

        # angles = kinematics.ik(leg, Coords(init_x/10, init_y/10, init_z/10))
        try:
            old_angles = angles
            angles = kinematics.ik_dle_clanku(leg, Coords(init_x, init_y, init_z))

            # kit.servo[0].angle = map_range(angles[2], -90, 90, 0, 180)
            # kit.servo[1].angle = map_range(angles[1], -90, 90, 0, 180)
            # kit.servo[2].angle = map_range(angles[0], -90, 90, 0, 180)

            kit.servo[0].angle = angles[2]
            kit.servo[1].angle = angles[1]
            kit.servo[2].angle = angles[0]

            # print(kinematics.ik(leg, Coords(init_x/10, init_y/10, init_z/10)))
            print(kinematics.ik_dle_clanku(leg, Coords(init_x, init_y, init_z)))
        except:
            print("Invalid angles")
            angles = old_angles



def control_angles():
    # Angles:
    # TODO: Chtelo by to Forward Kinematic !!! a zjistit, na
    # jakych pozcich xyz se effector pohybuje
    init_x = 70
    init_y = 120
    init_z = 100
    increment = 10
    cmd = ""
    while cmd != "e":
        cmd = input()
        if cmd == "w":
            init_x = init_x + increment if init_x + increment < 180 else 180
        elif cmd == "s":
            init_x = init_x - increment if init_x - increment > 0 else 0

        if cmd == "a":
            init_y = init_y + increment if init_y + increment < 180 else 180
        elif cmd == "d":
            init_y = init_y - increment if init_y - increment > 0 else 0

        if cmd == "r":
            init_z = init_z + increment if init_z + increment < 180 else 180
        elif cmd == "f":
            init_z = init_z - increment if init_z - increment > 0 else 0

        print("angle x", init_x)
        print("angle y", init_y)
        print("angle z", init_z)
        kit.servo[0].angle = init_z
        kit.servo[1].angle = init_x
        kit.servo[2].angle = init_y

        print(kinematics.forward_kinematics_3D(leg, init_x, init_y, init_z))


# control_angles()
control_position()

# import time

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

a0 = 100 # Base:  100 = Natazena base presne uprostred
a1 = 50  # Rameno: 50 = natazene rameno
a2 = 100 # Loket: 100 = presne 45stupnu dolu od stredni casti nohy
# TODO: toto by mely byt uhly pro 0 stupnu, tzn namapovat na range (-90, 90)
kit.servo[0].angle = a2
kit.servo[1].angle = a1
kit.servo[2].angle = a0

print("ik", kinematics.ik_dle_clanku(leg, Coords(10, 10, 0)))
print(kinematics.ik_dle_clanku(leg, Coords(10, 0, 5)))
#print(kinematics.ik_dle_clanku(leg, Coords(23.5, 0, 0)))

L1 = 5
L2 = 6.5
L3 = 12

a0 = 0 # Base
a1 = 0 # Rameno
a2 = 0 # Loket
p0 = Coords(0,0,0)
p1 = Coords(p0.x + L1, 0, p0.z)
p2 = Coords(p1.x + L2 * math.cos(a1), 0, p1.z + L2 * math.sin(a1))
p3 = Coords(p2.x + L3 * math.cos(a1 + a2), 0, p2.z + L3 * math.sin(a1 + a2))
E3 = Coords(p3.x * math.cos(a0), p3.x * math.sin(a0), p3.z)
print(E3.x, E3.y, E3.z)


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


