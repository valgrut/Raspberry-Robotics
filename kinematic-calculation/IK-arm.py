import math
import numpy as np
from matplotlib import pyplot as plt
from plot import *

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
        # print("FK Input angles:", base_angle,"°", pelvis_angle,"°", knee_angle,"°")
        # print(P0[0], P0[1])
        # print(P0[0] + d_base, P0[1])
        base_rad = math.radians(base_angle)  # natoceni cele nohy
        shoulder_rad = math.radians(shoulder_angle) # shoulder
        knee_rad = math.radians(knee_angle) # elbow

        P0 = Coords(0, 0, 10)

        # Pohyb kloubuu v ramci vertikalni osy xy
        P1 = Coords(P0.x + coxa_len * math.cos(shoulder_rad), P0.y + coxa_len * math.sin(shoulder_rad), P0.z)

        # print(round(P1[0], 3), round(P1[1], 3))
        P2 = Coords(P1.x + femur_len * math.cos(shoulder_rad + knee_rad), P1.y + femur_len * math.sin(shoulder_rad + knee_rad), P1.z)
        # print(round(P2[0], 3), round(P2[1], 3))

        # Natoceni cele nohy v ramci horizontalnich os xz
        P3 = (P0.x + P2.x * math.cos(base_rad), P2.y, P2.x * math.sin(base_rad))
        print("FK Output coords:", round(P3[0], 6), round(P3[1], 6), round(P3[2], 6))

        return (round(P3.x, 3), round(P3.y, 3), round(P3.z, 3))

    def inverse_kinematics(self, leg: HexapodLeg, target: Coords):
        pass

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

        if r1 > a2 + a3:
            print("Unreachable point")
            return None

        r2 = tz - a1  # [3]
        phi2 = math.atan2(r2, r1)  # [4]
        r3 = math.sqrt(r1**2 + r2**2)  # [5]
        phi1 = math.acos((a3**2 - a2**2 - r3**2) / (-2*a2*r3))  # [6]
        theta2 = math.degrees(phi2 - phi1)  # [7]
        phi3 = math.acos((r3**2 - a2**2 -a3**2) / (-2*a2*a3))  # [8]
        theta3 = math.degrees(phi3)  # [9]
        return (theta1, theta2, theta3)


leg = HexapodLeg(0, None, 2, 10, 6.5, 12)
kinematics = Kinematics()

# init_x = 15
# init_y = 0
# init_z = 10
# cmd = ""
# while cmd != "e":
#     cmd = input()
#     if cmd == "w":
#         init_x += 1
#     elif cmd == "s":
#         init_x -= 1

#     if cmd == "a":
#         init_y += 1
#     elif cmd == "d":
#         init_y -= 1

#     if cmd == "r":
#         init_z += 1
#     elif cmd == "f":
#         init_z -= 1

#     print(kinematics.ik(leg, Coords(init_x, init_y, init_z)))


for x in range(10, 20):
    print(kinematics.ik(leg, Coords(x, 0, 10)))
# for y in range(5, 10):
#     print(kinematics.ik(leg, Coords(5, y, 10)))
# for z in range(10, 20):
#     print(kinematics.ik(leg, Coords(0, 0, z)))


plotter = Plotter()
print("kruh")
for x in np.arange(10, 20, 0.5):
    plotter.add_point((10, 5*math.cos(x), 5*math.sin(x)))
    # plotter.add_point(kinematics.ik(leg, Coords(10, 5*math.cos(x), 5*math.sin(x))))

plotter.plot_points()
plotter.empty_points()

# print("cara")
# for x in np.arange(10, 17, 0.5):
#     print(kinematics.ik(leg, Coords(x, 0, 10)))
#     plotter.add_point(kinematics.ik(leg, Coords(x, 0, 10)))
# plotter.plot_points()


