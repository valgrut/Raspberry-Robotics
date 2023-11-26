import math

# https://technology.cpm.org/general/3dgraph/

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
def calc_3D_fk(P0, coxa_len, femur_len, d_base, a0, a1, a3):
    # print(P0[0], P0[1])
    # print(P0[0] + d_base, P0[1])
    a0_rad = math.radians(a0) # shoulder
    a1_rad = math.radians(a1) # elbow
    a3_rad = math.radians(a3)  # natoceni cele nohy

    # Pohyb kloubuu v ramci vertikalni osy xy
    P1 = (P0[0] + d_base + coxa_len * math.cos(a0_rad), P0[1] + coxa_len * math.sin(a0_rad))
    # print(round(P1[0], 3), round(P1[1], 3))
    P2 = (P1[0] + femur_len * math.cos(a0_rad + a1_rad), P1[1] + femur_len * math.sin(a0_rad + a1_rad))
    # print(round(P2[0], 3), round(P2[1], 3))

    # Natoceni cele nohy v ramci horizontalnich os xz
    x_distance = P2[0]
    base_x = P0[0]
    P3 = (base_x + x_distance * math.cos(a3_rad), P2[1], x_distance * math.sin(a3_rad))
    print("END xyz:", round(P3[0], 3), round(P3[1], 3), round(P3[2], 3))

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
    ikRadiansFemurTibia = math.acos ( ( ( pow ( femur_len, 2 ) - pow ( tibia_len, 2 ) ) + pow ( ikSW, 2 ) ) / ( 2 * femur_len * ikSW))

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

xyz = calc_3D_fk((0,0), femur_len, tibia_len, base_len, shoulder, elbow, base)
print()
# angles = calc_3D_ik2(femur_len, tibia_len, 10, 0, 10)
angles = calc_3D_ik2(femur_len, tibia_len, base_len, xyz[0], xyz[1], xyz[2])
print("coxa", angles[0], "femur", angles[1], "tibia", angles[2])
print()
xyz2 = calc_3D_fk((0,0), femur_len, tibia_len, base_len, angles[0], angles[1], angles[2])


### samostatny test
print("Samsotatne testy")
print()
angles = calc_3D_ik2(femur_len, tibia_len, base_len, 20.4, 0, 0)
print("coxa", angles[0], "femur", angles[1], "tibia", angles[2])

calc_3D_fk((0,0), femur_len, tibia_len, base_len, 0, 0, 0)
calc_3D_fk((0,0), femur_len, tibia_len, base_len, -8.1062, 77.5132, 0)


