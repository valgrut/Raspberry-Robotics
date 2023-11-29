import math
from matplotlib import pyplot as plt

# https://technology.cpm.org/general/3dgraph/

class Coords:
    def __init__(self, x=0, y=0, z=0):
        self.x = x
        self.y = y
        self.z = z

    def coords(self):
        print(self.x, self.y, self.z)

class HexapodLeg():
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

    def move_leg_Y()
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

    def forward_kinematics_3D(self, leg: HexapodLeg, base_angle, shoulder_angle, knee_angle):
        # print("FK Input angles:", base_angle,"°", pelvis_angle,"°", knee_angle,"°")
        # print(P0[0], P0[1])
        # print(P0[0] + d_base, P0[1])
        base_rad = math.radians(base_angle)  # natoceni cele nohy
        shoulder_rad = math.radians(shoulder_angle) # shoulder
        knee_rad = math.radians(knee_angle) # elbow

        P0 = leg.leg_placement_coords

        # Pohyb kloubuu v ramci vertikalni osy xy
        P1 = Coords((P0.x + d_base + coxa_len * math.cos(shoulder_rad), P0.y + coxa_len * math.sin(shoulder_rad), P0.z)

        # print(round(P1[0], 3), round(P1[1], 3))
        P2 = Coords(P1.x + femur_len * math.cos(shoulder_rad + knee_rad), P1.y + femur_len * math.sin(shoulder_rad + knee_rad), P1.z)
        # print(round(P2[0], 3), round(P2[1], 3))

        # Natoceni cele nohy v ramci horizontalnich os xz
        P3 = (P0.x + P2.x * math.cos(base_rad), P2.y, P2.x * math.sin(base_rad))
        print("FK Output coords:", round(P3[0], 6), round(P3[1], 6), round(P3[2], 6))

        # plt.rcParams["figure.figsize"] = [7.50, 3.50]
        # plt.rcParams["figure.autolayout"] = True
        # fig = plt.figure()
        # ax = fig.add_subplot(projection="3d")
        # ax.set_xlabel('x')
        # ax.set_ylabel('y')
        # ax.set_zlabel('z')
        # x, y, z = [P0[0]+d_base, P0[0], P1[0], P2[0], P3[0]], [P0[1],P0[1], P1[1], P2[1], P3[1]], [P0[2], P0[2], P1[2], P2[2], P3[2]]
        # ax.scatter(x, y, z, c='red', s=100)
        # ax.plot(x, y, z, color='black')
        # plt.show()

        return (round(P3.x, 3), round(P3.y, 3), round(P3.z, 3))

    def inverse_kinematics(self):
        pass


# FUNGUJE!!! (Opravdu)
def calc_2D_fk(P0, coxa_len, femur_len, a0, a1):
    print(P0[0], P0[1])
    a0_rad = math.radians(a0)
    a1_rad = math.radians(a1)

    P1 = (P0[0] + coxa_len * math.cos(a0_rad), P0[1] + coxa_len * math.sin(a0_rad))
    print(round(P1[0], 3), round(P1[1], 3))
    P2 = (P1[0] + femur_len * math.cos(a0_rad + a1_rad), P1[1] + femur_len * math.sin(a0_rad + a1_rad))
    print(round(P2[0], 3), round(P2[1], 3))


# FUNGUJE!!! (Opravdu)
def calc_3D_fk(P0, coxa_len, femur_len, d_base, pelvis_angle, knee_angle, base_angle):
    print("FK Input angles:", base_angle,"°", pelvis_angle,"°", knee_angle,"°")
    # print(P0[0], P0[1])
    # print(P0[0] + d_base, P0[1])
    pelvis_rad = math.radians(pelvis_angle) # shoulder
    knee_rad = math.radians(knee_angle) # elbow
    base_rad = math.radians(base_angle)  # natoceni cele nohy

    # Pohyb kloubuu v ramci vertikalni osy xy
    P1 = (P0[0] + d_base + coxa_len * math.cos(pelvis_rad), P0[1] + coxa_len * math.sin(pelvis_rad), P0[2])
    # print(round(P1[0], 3), round(P1[1], 3))
    P2 = (P1[0] + femur_len * math.cos(pelvis_rad + knee_rad), P1[1] + femur_len * math.sin(pelvis_rad + knee_rad), P1[2])
    # print(round(P2[0], 3), round(P2[1], 3))

    # Natoceni cele nohy v ramci horizontalnich os xz
    x_distance = P2[0]
    base_x = P0[0]
    P3 = (base_x + x_distance * math.cos(base_rad), P2[1], x_distance * math.sin(base_rad))
    print("FK Output coords:", round(P3[0], 6), round(P3[1], 6), round(P3[2], 6))

    # plt.rcParams["figure.figsize"] = [7.50, 3.50]
    # plt.rcParams["figure.autolayout"] = True
    # fig = plt.figure()
    # ax = fig.add_subplot(projection="3d")
    # ax.set_xlabel('x')
    # ax.set_ylabel('y')
    # ax.set_zlabel('z')
    # x, y, z = [P0[0]+d_base, P0[0], P1[0], P2[0], P3[0]], [P0[1],P0[1], P1[1], P2[1], P3[1]], [P0[2], P0[2], P1[2], P2[2], P3[2]]
    # ax.scatter(x, y, z, c='red', s=100)
    # ax.plot(x, y, z, color='black')
    # plt.show()

    return (round(P3[0], 3), round(P3[1], 3), round(P3[2], 3))

# Test
# calc_3D_fk((0, 10), 5, 3, d_base=2, a0=45, a1=10, a3=180)

def calc_2D_ik(femur_len, tibia_len, tx, ty):
    pass

# def calc_3D_ik(femur_len, tibia_len, tx, ty, tz):
#     # TODO: Zakomponovat d_base
#     # if (femur_len + tibia_len) >= tx or (femur_len + tibia_len) >= ty or (femur_len + tibia_len) >= tz:
#         # print("Invalid values")

#     r = math.sqrt(tx**2 + ty**2 + tz**2)
#     base = math.atan2(ty,tx)
#     acos_value = (r**2 - femur_len**2 - tibia_len**2) / (2 * femur_len * tibia_len)
#     # print(acos_value)
#     elbow = -math.acos(acos_value)
#     shoulder = math.asin(tz/r) + math.atan2((tibia_len * math.sin(elbow)), (femur_len + (tibia_len * math.cos(elbow))))

#     print("tx, ty, tz", tx, ty, tz)
#     print("r", r)
#     print("base", math.degrees(base))
#     print("elbow", math.degrees(elbow))
#     print("shoulder", math.degrees(shoulder))
#     print()


def calc_3D_ik3(femur_len, tibia_len, coxa_len, tx, ty, tz):
    # TODO: Brat v uvahu pocatecni bod pro vypocet (napr P0 jak u FK, kde y=10)
    # TODO: Zakomponovat d_base
    base = 0
    shoulder = 0
    elbow = 0

    l0 = math.sqrt(tx**2 + tz**2)
    # tz += 2

    # top view (XZ plane)
    base_rad = math.atan2(tz, tx)
    base = round(math.degrees(base_rad))
    print("base", base) #OK

    # side view (XZ-Y plane)
    # shoulder_rad = math.acos((l0**2 + femur_len**2 - tibia_len**2)/(2 * l0 * femur_len))
    # elbow_rad = math.acos((tibia_len**2 + femur_len**2 - l0**2)/(2 * femur_len * tibia_len))

    tx -= coxa_len
    # ty += 10

    # l0 = math.sqrt(tx**2 + ty**2)
    elbow_rad = math.acos((tx**2 + ty**2 - femur_len**2 - tibia_len**2) / (2*femur_len*tibia_len))
    elbow = math.degrees(elbow_rad)
    shoulder_rad = math.atan2(tx, ty) - math.asin((tibia_len * math.sin(elbow)) / (math.sqrt(tx**2 + ty**2)))
    # shoulder_rad = math.atan2(tx, ty) - math.atan2(tibia_len*math.sin(elbow), femur_len + tibia_len*math.cos(elbow))
    # shoulder_rad = (-ty * tibia_len * math.sin(elbow) + tx * (femur_len + tibia_len * math.cos(elbow))) / (tx*tibia_len* math.sin(elbow) + ty * (femur_len + tibia_len * math.cos(elbow)))
    shoulder = math.degrees(shoulder_rad)

    print("shoulder", shoulder)
    print("elbow", elbow)

    return (base, shoulder, elbow)
    # return (base, shoulder_rad, elbow_rad)
    # return (base_rad, shoulder_rad, elbow_rad)



def calc_3D_ik2(femur_len, tibia_len, coxa_len, tx, ty, tz):
    # top view (XZ plane)
    base_rad = math.atan2(tz, tx)
    base = round(math.degrees(base_rad))
    # print("base", base) #OK

    # Distance between the Coxa and Ground Contact
    height_offset = math.sqrt(tx**2 + ty**2)

    # ikSW - Length between Femur axis and Tibia (prepona 'c' mezi femurem a tibia)
    ikSW = math.sqrt((height_offset - coxa_len)**2 + tz**2)

    # ikRadiansFemurTibiaGround - Angle between Femur and Tibia line and the ground in radians
    ikRadiansFemurTibiaGround = math.atan2(height_offset - coxa_len, tz)

    # ikRadiansFemurTibia - Angle of the line Femur and Tibia with respect to the Femur in radians
    ikRadiansFemurTibia = math.acos(((femur_len**2 - tibia_len**2) + ikSW**2) / (2 * femur_len * ikSW))

    # ikCoxaAngle in degrees
    initCoxaAngle = 0
    coxaAngle = math.atan2(ty, tx) * 180 / math.pi +initCoxaAngle

    # ikFemurAngle in degrees
    femurAngle = - (ikRadiansFemurTibiaGround + ikRadiansFemurTibia ) * 180 / math.pi  + 90

    # ikTibiaAngle in degrees
    tibiaAngle = - (90 -(((math.acos((femur_len**2 + tibia_len**2 - ikSW**2 ) / ( 2 * femur_len * tibia_len ) ) ) * 180 ) / math.pi ) )

    # return (base, shoulder, elbow)
    return (coxaAngle, femurAngle, tibiaAngle)
    # return (base, shoulder_rad, elbow_rad)
    # return (base_rad, shoulder_rad, elbow_rad)



# ============= TEST ==============================
# Test:
# calc_3D_ik(femur_len, tibia_len, 10, 0, 0)
# calc_3D_ik(femur_len, tibia_len, 0, 10, 0)
# calc_3D_ik(femur_len, tibia_len, 0, 0, 10)

# calc_3D_ik(femur_len, tibia_len, 0, 5, 5)
# calc_3D_ik(femur_len, tibia_len, 5, 0, 5)
# calc_3D_ik(femur_len, tibia_len, 5, 5, 0)
# calc_3D_ik(femur_len, tibia_len, 5, 5, 5)

# Draw a line - Path of the hand from x=1 to x=10
# for x in range(-5, 5):
    # calc_3D_ik(femur_len, tibia_len, x, 5, 0)

base_len = 2  # 2 cm
femur_len = 6.5
tibia_len = 12

base = 20
shoulder = 20
elbow = -50

print("base", base)
print("shoulder", shoulder)
print("elbow", elbow)
print()

# xyz = calc_3D_fk((0,0), femur_len, tibia_len, base_len, shoulder, elbow, base)
# print()
# # angles = calc_3D_ik2(femur_len, tibia_len, 10, 0, 10)
# angles = calc_3D_ik2(femur_len, tibia_len, base_len, xyz[0], xyz[1], xyz[2])
# print("coxa", angles[0], "femur", angles[1], "tibia", angles[2])
# print()
# xyz2 = calc_3D_fk((0,0), femur_len, tibia_len, base_len, angles[0], angles[1], angles[2])


### samostatny test
print("Samsotatne testy")
print()
angles = calc_3D_ik2(femur_len, tibia_len, base_len, 20.5, 0, 0)
print("coxa", angles[0], "femur", angles[1], "tibia", angles[2])

# calc_3D_fk((0,0,0), femur_len, tibia_len, base_len, 0, 0, 0)
# calc_3D_fk((0,10,0), femur_len, tibia_len, base_len, 160, 90, 0) # Klasicke postaveni nohy (160,90,0)
calc_3D_fk((0,10,0), femur_len, tibia_len, base_len, 90, 190, 0)
# angles = calc_3D_ik2(femur_len, tibia_len, base_len, -8.212244, 0.946819, 0)
# print("coxa", angles[0], "femur", angles[1], "tibia", angles[2])


init_base = 10
init_tibia = 10
init_femur = 10
cmd = ""
while cmd != "e":
    cmd = input()
    if cmd == "x":
        init_base += 1
    elif cmd == "z":
        init_base -= 1

    if cmd == "s":
        init_tibia += 1
    elif cmd == "a":
        init_tibia -= 1

    if cmd == "w":
        init_femur += 1
    elif cmd == "q":
        init_femur -= 1

    # calc_3D_fk((0,10,0), femur_len, tibia_len, base_len, init_femur, init_tibia, init_base)
    angles = calc_3D_ik2(femur_len, tibia_len, base_len, init_base, init_tibia, init_femur)
    print("coxa", angles[0], "femur", angles[1], "tibia", angles[2])


