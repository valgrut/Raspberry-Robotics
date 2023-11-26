import math

def forward_kinematics(theta1, theta2, theta3, L1, L2):
    x = L2 * math.cos(theta1 + theta2 + theta3)
    y = L2 * math.sin(theta1 + theta2 + theta3)
    z = L1
    return x, y, z

# Given:
#     θ1 is the angle of rotation around the z-axis (R1).
#     θ2 is the angle at joint O1.
#     θ3 is the angle at joint O2.
#     L1 is the distance from O1 to O2.
#     L2 is the distance from O2 to c.
#     There's zero length between O1 and R1.

# Example usage:
theta1 = math.radians(0)  # Convert to radians if necessary
theta2 = math.radians(30)  # Convert to radians if necessary
theta3 = math.radians(60)  # Convert to radians if necessary
L1 = 2  # Length from O1 to O2
L2 = 3  # Length from O2 to c

x, y, z = forward_kinematics(theta1, theta2, theta3, L1, L2)
print(f"x: {x}, y: {y}, z: {z}")
