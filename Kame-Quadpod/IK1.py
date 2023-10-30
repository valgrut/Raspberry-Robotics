import math

def inverse_kinematics(x, y, z, L1, L2):
    theta1 = math.atan2(y, x)
    theta3 = math.acos((x**2 + y**2 + (z - L1)**2 - L2**2) / (2 * L2 * math.sqrt(x**2 + y**2 + (z - L1)**2)))
    theta2 = math.atan2(math.sqrt(x**2 + y**2), z - L1) - math.atan2(L2 * math.sin(theta3), L2 * math.cos(theta3) + L1)
    return theta1, theta2, theta3

# Example usage:
x = 0
y = 0
z = 5

L1 = 2  # Length from O1 to O2
L2 = 3  # Length from O2 to c

theta1, theta2, theta3 = inverse_kinematics(x, y, z, L1, L2)
print(f"Theta 1: {math.degrees(theta1)} degrees (Rotation around z)")
print(f"Theta 2: {math.degrees(theta2)} degrees (Spodni joint)")
print(f"Theta 3: {math.degrees(theta3)} degrees (horni joint)")
