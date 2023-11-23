import math

# IK calculation
def calc_ik(d1, d2, tx, ty, tz):
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

d1 = 5
d2_forearm = 5
calc_ik(d1, d2_forearm, 10, 0, 0)
calc_ik(d1, d2_forearm, 0, 10, 0)
calc_ik(d1, d2_forearm, 0, 0, 10)

calc_ik(d1, d2_forearm, 0, 5, 5)
calc_ik(d1, d2_forearm, 5, 0, 5)
calc_ik(d1, d2_forearm, 5, 5, 0)
calc_ik(d1, d2_forearm, 5, 5, 5)

# Path of the hand from x=1 to x=10
for x in range(1, 10):
    calc_ik(d1, d2_forearm, x, 0, 0)
