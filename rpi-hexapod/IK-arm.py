import math
import numpy as np
from matplotlib import pyplot as plt
from plot import *
# from adafruit_servokit import ServoKit

# https://technology.cpm.org/general/3dgraph/

class Coords:
    def __init__(self, x=0, y=0, z=0):
        self.x = x
        self.y = y
        self.z = z

    def coords(self):
        print(self.x, self.y, self.z)


class HexapodLeg:
    def __init__(self, leg_idx, leg_placement_coords: Coords, body_coxa_distance, coxa_len, femur_len, tibia_len):
        # Leg definition
        self.leg_idx = leg_idx
        self.leg_placement_coords = leg_placement_coords
        self.body_coxa_distance = body_coxa_distance
        self.coxa_len = coxa_len
        self.femur_len = femur_len
        self.tibia_len = tibia_len
        self.current_effector_pos = Coords(0,0,0)

    def move_to_point(self, end_point: Coords):
        pass

    def move_leg_X(self, value):
        # Move leg along X-axis
        pass

    def move_leg_Y(self, value):
        # Move leg along Y-axis
        pass

    def move_leg_Z(self, value):
        # Move leg along Z-axis
        pass


class Hexapod:
    def __init__(self):
        self.body = None
        self.legs = []

    def walk(self):
        pass


class Kinematics:
    def __init__(self):
        pass

    # FUNGUJE!!! (Opravdu)
    def calc_2D_fk(self, P0, coxa_len, femur_len, a0, a1):
        print(P0[0], P0[1])
        a0_rad = math.radians(a0)
        a1_rad = math.radians(a1)

        P1 = (P0[0] + coxa_len * math.cos(a0_rad), P0[1] + coxa_len * math.sin(a0_rad))
        print(round(P1[0], 3), round(P1[1], 3))
        P2 = (P1[0] + femur_len * math.cos(a0_rad + a1_rad), P1[1] + femur_len * math.sin(a0_rad + a1_rad))
        print(round(P2[0], 3), round(P2[1], 3))

    def forward_kinematics_3D(self, leg: HexapodLeg, base_angle, shoulder_angle, knee_angle):
        # TODO: Fix, nefunguje. Znova prepocitat.
        # print("FK Input angles:", base_angle,"°", pelvis_angle,"°", knee_angle,"°")
        # print(P0[0], P0[1])
        # print(P0[0] + d_base, P0[1])
        base_rad = math.radians(base_angle)  # natoceni cele nohy
        shoulder_rad = math.radians(shoulder_angle) # shoulder
        knee_rad = math.radians(knee_angle) # elbow

        P0 = Coords(0, 0, 10)

        # Pohyb kloubuu v ramci vertikalni osy xy
        P1 = Coords(P0.x + leg.coxa_len * math.cos(shoulder_rad), P0.y + leg.coxa_len * math.sin(shoulder_rad), P0.z)

        # print(round(P1[0], 3), round(P1[1], 3))
        P2 = Coords(P1.x + leg.femur_len * math.cos(shoulder_rad + knee_rad), P1.y + leg.femur_len * math.sin(shoulder_rad + knee_rad), P1.z)
        # print(round(P2[0], 3), round(P2[1], 3))

        # Natoceni cele nohy v ramci horizontalnich os xz
        P3 = Coords(P0.x + P2.x * math.cos(base_rad), P2.y, P2.x * math.sin(base_rad))
        print("FK Output coords:", round(P3.y, 6), round(P3.z, 6), round(P3.x, 6))

        return (round(P3.y, 3), round(P3.z, 3), round(P3.x, 3))

    def calc_2D_ik(self, femur_len, tibia_len, tx, ty):
        pass

    def ik(self, leg: HexapodLeg, target: Coords):
        # TODO: Pridat constraints
        # TODO: Pridat asi ty 2cm jeste od base po coxa
        tx = target.x
        ty = target.y
        tz = target.z

        a1 = leg.coxa_len # height
        a2 = leg.femur_len
        a3 = leg.tibia_len

        theta1 = math.degrees(math.atan2(ty, tx))  # [1]
        r1 = math.sqrt(tx**2 + ty**2) - leg.body_coxa_distance # [2]

        # if r1 > a2 + a3:
            # print("Unreachable point")
            # return None

        r2 = tz - a1  # [3]
        phi2 = math.atan2(r2, r1)  # [4]
        r3 = math.sqrt(r1**2 + r2**2)  # [5]
        phi1 = math.acos((a3**2 - a2**2 - r3**2) / (-2*a2*r3))  # [6]
        theta2 = math.degrees(phi2 - phi1)  # [7]
        phi3 = math.acos((r3**2 - a2**2 -a3**2) / (-2*a2*a3))  # [8]
        theta3 = math.degrees(phi3)  # [9]
        return (theta1, theta2, theta3)

    def ik_dle_clanku(self, leg: HexapodLeg, target: Coords):
        x0 = target.x
        y0 = target.y
        z0 = target.z

        L2 = leg.femur_len
        L3 = leg.tibia_len

        theta1 = math.atan(y0 / x0)
        theta2 = math.acos((-L3**2 + L2**2 + x0**2 + y0**2 + z0**2) / (2*L2*math.sqrt(x0**2 + y0**2 + z0**2))) + math.atan2(z0, (math.sqrt(x0**2 + y0**2)))
        theta3 = -math.acos((x0**2 + y0**2 + z0**2 - L2**2 - L3**2) / (2*L2*L3))

        return (math.degrees(theta1), math.degrees(theta2), math.degrees(theta3))



def map_range(v, a, b, c, d):
       return (v-a) / (b-a) * (d-c) + c
# a = map_range(4, 0, 10, 0, 1)



# leg = HexapodLeg(0, None, 5, 10, 6.5, 12)
leg = HexapodLeg(0, None, 0, 5, 6.5, 12)
kinematics = Kinematics()
# kit = ServoKit(channels=8)


def control_position():
    # xyz pozice na relativne normalni pozici nohou:
    init_x = 75
    init_y = 120
    init_z = 100

    increment = 10
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

        print("new x", init_x, "small_x", init_x/10)
        print("new y", init_y, "small_y", init_y/10)
        print("new z", init_z, "small_z", init_z/10)

        angles = kinematics.ik(leg, Coords(init_x/10, init_y/10, init_z/10))
        kit.servo[0].angle = abs(angles[0])
        kit.servo[1].angle = abs(angles[1])
        kit.servo[2].angle = abs(angles[2])

        print(kinematics.ik(leg, Coords(init_x/10, init_y/10, init_z/10)))


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
# control_position()

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
for y in range(5, 15):
    print(kinematics.ik(leg, Coords(10, y, 0)))
    print(kinematics.ik_dle_clanku(leg, Coords(10, y, 0)))

print()
print(kinematics.ik(leg, Coords(10, 10, -5)))
print(kinematics.ik_dle_clanku(leg, Coords(10, 10, -5)))

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


