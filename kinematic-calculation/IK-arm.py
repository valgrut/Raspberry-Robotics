import math

# IK calculation
def calc_3D_ik(d1, d2, tx, ty, tz):
    # if (d1 + d2) >= tx or (d1 + d2) >= ty or (d1 + d2) >= tz:
        # print("Invalid values")

    r = math.sqrt(tx**2 + ty**2 + tz**2)
    base = math.atan2(ty,tx)
    acos_value = (r**2 - d1**2 - d2_forearm**2) / (2 * d1 * d2_forearm)
    # print(acos_value)
    elbow = -math.acos(acos_value)
    shoulder = math.asin(tz/r) + math.atan2((d2_forearm * math.sin(elbow)), (d1 + (d2_forearm * math.cos(elbow))))

    print("tx, ty, tz", tx, ty, tz)
    print("r", r)
    print("base", math.degrees(base))
    print("elbow", math.degrees(elbow))
    print("shoulder", math.degrees(shoulder))
    print()

d1 = 6.5
d2_forearm = 12
calc_3D_ik(d1, d2_forearm, 10, 0, 0)
calc_3D_ik(d1, d2_forearm, 0, 10, 0)
calc_3D_ik(d1, d2_forearm, 0, 0, 10)

calc_3D_ik(d1, d2_forearm, 0, 5, 5)
calc_3D_ik(d1, d2_forearm, 5, 0, 5)
calc_3D_ik(d1, d2_forearm, 5, 5, 0)
calc_3D_ik(d1, d2_forearm, 5, 5, 5)

# Draw a line
# Path of the hand from x=1 to x=10
# for x in range(-5, 5):
    # calc_3D_ik(d1, d2_forearm, x, 5, 0)

print(">.......................")

# FUNGUJE!!! (Opravdu)
def calc_2D_fk(P0, d0, d1, a0, a1):
    print(P0[0], P0[1])
    a0_rad = math.radians(a0)
    a1_rad = math.radians(a1)

    P1 = (P0[0] + d0 * math.cos(a0_rad), P0[1] + d0 * math.sin(a0_rad))
    print(round(P1[0], 3), round(P1[1], 3))
    P2 = (P1[0] + d1 * math.cos(a0_rad + a1_rad), P1[1] + d1 * math.sin(a0_rad + a1_rad))
    print(round(P2[0], 3), round(P2[1], 3))


# FUNGUJE!!! (Opravdu)
def calc_3D_fk(P0, d0, d1, d_base, a0, a1, a3):
    print(P0[0], P0[1])
    print(P0[0] + d_base, P0[1])
    a0_rad = math.radians(a0)
    a1_rad = math.radians(a1)
    a3_rad = math.radians(a3)  # natoceni cele nohy

    # Pohyb kloubuu v ramci vertikalni osy xy
    P1 = (P0[0] + d_base + d0 * math.cos(a0_rad), P0[1] + d0 * math.sin(a0_rad))
    print(round(P1[0], 3), round(P1[1], 3))
    P2 = (P1[0] + d1 * math.cos(a0_rad + a1_rad), P1[1] + d1 * math.sin(a0_rad + a1_rad))
    print(round(P2[0], 3), round(P2[1], 3))

    # Natoceni cele nohy v ramci horizontalnich os xz
    x_distance = P2[0]
    base_x = P0[0]
    P3 = (base_x + x_distance * math.cos(a3_rad), P2[1], x_distance * math.sin(a3_rad))
    print("END", round(P3[0], 3), round(P3[1], 3), round(P3[2], 3))


calc_3D_fk((0, 10), 5, 3, d_base=2, a0=45, a1=-10, a3=90)

