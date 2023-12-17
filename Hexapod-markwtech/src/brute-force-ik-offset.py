from coords import Coords
import math

# Note: FK brute force worked, but this one doesn't!

# Description of the problem:
#   Brute force method for finding the correct offsets for the elbow and shoulder angles.
#   Those offsets are required because of the real limits of the cheap servos, not corresponding to the
#   simplified scheme of the leg, for which the IK and FK was calculated previously.
#
#   The catch is that my servos have only 0-130 degrees available motion. I can assign 180, but
#   those 180 degrees will be squished into those ~130 degrees, so the angles will be off
#   and so all the angles between.
# 
#   For example I assign 90 degrees and expect perfect angle,
#   but servo will turn just to 50 degree. This caused all the frustration because both forward and
#   inverse kinematics were giving wrong results.
#
#   Note: Does NOT work correctly for some reason. Correct offset angles found for the 
#   Coords(6.1, 16.8, -5.2), but they didn't fit to other value, i.e. (18.6, 0, -8.3)

def inverse_kinematics(base_angle_offset, shoulder_angle_offset, elbow_angle_offset):
    target = Coords(6.1, 16.8, -5.2) # Expecting output angles: (0, 130, 60)
    x = target.x
    y = target.y
    z = target.z  # - 3.2  # nebo +3.2  ??

    # Avoid zero-division
    y += 0.00000001
    z += 0.00000001

    # Leg parts lengths
    L1 = 5
    L2 = 6.4
    L3 = 12

    try:
        L = math.sqrt(x**2 + y**2)
        Lt = math.sqrt((L - L1)**2 + z**2)
        gamma = math.atan2((L - L1), z)
        beta = math.acos((L2**2 + Lt**2 - L3**2) / (2*L2*Lt))
        alpha = math.acos((L2**2 + L3**2 - Lt**2) / (2*L2*L3))

        # Elbow angle:
        # theta1 = 90 - math.degrees(alpha) # Originalni hodnoty
        # theta1 = 180 - math.degrees(alpha)  # Prizpusobeni pro FK
        # theta1 = - (180 - math.degrees(alpha))  # Prizpusobeni pro FK a otocit uhly
        # theta1 = 280 - math.degrees(alpha)  # Prizpusobeni pro realne uhly pro servo motory
        theta1 =  (elbow_angle_offset - math.degrees(alpha))
        # Varianty: - / +

        # Shoulder angle:
        # theta2 = 90 - (math.degrees(gamma) + math.degrees(beta))  # Originalni hodnoty
        # theta2 = 90 - (math.degrees(gamma) + math.degrees(beta))  # Prizpusobeni pro FK
        # TODO: if theta2 < 0 then reverse sign (Chci, aby pavouk vzdy udrzoval ten klasicky tvar nohy)
        # theta2 = - (90 - (math.degrees(gamma) + math.degrees(beta)))  # Prizpusobeni pro FK a otocit uhly
        # theta2 = - (140 - (math.degrees(gamma) + math.degrees(beta)))  # Prizpusobeni pro realne uhly pro servo motory
        theta2 =  -(shoulder_angle_offset - (math.degrees(gamma) + math.degrees(beta)))
        # Varianty: - / +

        # Base angle:
        theta_base = base_angle_offset - math.degrees(math.atan2(x, y))  # Originalni hodnoty
        # TODO: if theta_base > 0 then reverse sign (Chci, aby pavouk vzdy udrzoval ten klasicky tvar nohy)
        # theta_base = 90 - math.degrees(math.atan2(x, y))  # Prizpusobeni pro FK (Aby odpovidalo FK <=> IK)
        # theta_base = 10 + math.degrees(math.atan2(x, y)) # Prizpusobeni pro realne uhly pro servo motory

        return theta_base, theta2, theta1
    
    except Exception as e:
        print(e)


################## USAGE ########################
# Try all combinations of the offsets for elbow and shoulder angles.
base_offset = 10
closest_results_cnt = 0
angle_incr = 0.1  # Decrease for better precision (10, 5, 2, 1, 0.5, 0.1)

while base_offset <= 30:
    shoulder_offset = -300
    while shoulder_offset <= 300:
        elbow_offset = -300
        while elbow_offset <= 300:
            base_angle, shoulder_angle, elbow_angle = inverse_kinematics(base_offset, shoulder_offset, elbow_offset)
            
            elbow_angle = round(elbow_angle, 3)
            shoulder_angle = round(shoulder_angle, 3)
            base_angle = round(base_angle, 3)

            #print("(",base_offset, shoulder_offset, elbow_offset, ") => (", base_angle, shoulder_angle, elbow_angle, ") / Expected (0, 130, 60)")

            # We are finding such offsets to add to the input angles that will cause to end-effector to end up
            # in the given coordinate range closest to the estimated one (by eye).
            # if 0 <= base_angle < 2 and 128 < shoulder_angle <= 130 and 58 < elbow_angle <= 60:
            # if 0 <= base_angle < 2 and 120 < shoulder_angle <= 130 and 40 < elbow_angle <= 70:
            if 0 <= base_angle < 0.5 and 129.4 < shoulder_angle <= 130 and 59.4 < elbow_angle <= 60:
                # Shorten the values
                base_angle = round(base_angle, 1)
                shoulder_angle = round(shoulder_angle, 1)
                elbow_angle = round(elbow_angle, 1)

                print("Possible correct angles offset combination:")
                print(base_angle, shoulder_angle, elbow_angle)
                print("FOUND base offset:", base_offset)
                print("FOUND shoulder offset:", shoulder_offset)
                print("FOUND elbow offset", elbow_offset)
                print()
                closest_results_cnt += 1
            
            elbow_offset += angle_incr
            continue
        shoulder_offset += angle_incr
        continue
    base_offset += angle_incr
    continue

print("Closest results:", closest_results_cnt)


## Found offset: (20, 42, 152) for my setup for coords (6.1, 16.8, -5.2) on angles from FK: (0, 130, 60)
# !!! Does NOT work for Coords(18.6, 0, -8.3) # Should give 70, 110, 100

# Test of the found values:
def inverse_kinematics_2(target: Coords):
    x = target.x
    y = target.y
    z = target.z  # - 3.2  # nebo +3.2  ??

    # Avoid zero-division
    y += 0.00000001
    z += 0.00000001

    # Leg parts lengths
    L1 = 5
    L2 = 6.4
    L3 = 12

    try:
        L = math.sqrt(x**2 + y**2)
        Lt = math.sqrt((L - L1)**2 + z**2)
        gamma = math.atan2((L - L1), z)
        beta = math.acos((L2**2 + Lt**2 - L3**2) / (2*L2*Lt))
        alpha = math.acos((L2**2 + L3**2 - Lt**2) / (2*L2*L3))

        # Elbow angle:
        elbow_angle = 152.89 - math.degrees(alpha)

        # Shoulder angle:
        shoulder_angle = - (41.7 - (math.degrees(gamma) + math.degrees(beta)))

        # Base angle:
        base_angle = 20.4 - math.degrees(math.atan2(x, y))  # Originalni hodnoty
        # TODO: if theta_base > 0 then reverse sign (Chci, aby pavouk vzdy udrzoval ten klasicky tvar nohy)
        # theta_base = 90 - math.degrees(math.atan2(x, y))  # Prizpusobeni pro FK (Aby odpovidalo FK <=> IK)
        # theta_base = 10 + math.degrees(math.atan2(x, y)) # Prizpusobeni pro realne uhly pro servo motory

        elbow_angle = round(elbow_angle, 3)
        shoulder_angle = round(shoulder_angle, 3)
        base_angle = round(base_angle, 3)

        return base_angle, shoulder_angle, elbow_angle
    
    except Exception as e:
        print(e)

print(inverse_kinematics_2(Coords(23.1, 0, 0)))
print(inverse_kinematics_2(Coords(9.0, -15.5, -5.2))) #OK
print(inverse_kinematics_2(Coords(18.6, 0, -8.3))) # 70, 110, 100