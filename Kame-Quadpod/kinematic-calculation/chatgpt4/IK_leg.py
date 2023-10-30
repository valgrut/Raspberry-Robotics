import math
import matplotlib.pyplot as plt

body_height = 5  # initial joint height

def forward_kinematics(theta_1, theta_2, theta_3, L1, L2, L3):
    U_x = L1 * math.cos(theta_1) + L2 * math.cos(theta_1) * math.sin(theta_2) + L3 * math.cos(theta_1) * math.sin(theta_2 + theta_3)
    U_y = L1 * math.sin(theta_1) + L2 * math.sin(theta_1) * math.sin(theta_2) + L3 * math.sin(theta_1) * math.sin(theta_2 + theta_3)
    U_z = body_height - (L2 * math.cos(theta_2) + L3 * math.cos(theta_2 + theta_3))

    return U_x, U_y, U_z
    # # J1 position
    # j1_x = L1 * math.cos(theta_1)
    # j1_y = L1 * math.sin(theta_1)
    # j1_z = body_height

    # # J2 position
    # j2_x = j1_x
    # j2_y = j1_y
    # j2_z = j1_z - L2 * math.sin(theta_2)

    # # J3 position
    # j3_x = j2_x
    # j3_y = j2_y
    # j3_z = j2_z - L3 * math.sin(theta_3)

    # # U position (U is essentially the same as J3 in this setup)
    # U_x, U_y, U_z = j3_x, j3_y, j3_z

    # return U_x, U_y, U_z


def inverse_kinematics(x, y, z, L1, L2, L3):
    # Given the x, y, z position of the end effector, this function will return the joint angles required.
    
    # Calculate theta1 using arctan2
    theta_1 = math.atan2(y, x)
    
    # Calculate the reach in the Z direction
    h = 5 - z   # body height is 5
    
    # Using Pythagoras theorem and laws of cosines
    # d is the length from J1 to the projected end point on the X-Y plane
    d = math.sqrt(x**2 + y**2) - L1
    
    # Calculate the distances between J1 and U in 2D
    r = math.sqrt(d**2 + h**2)

    if r > (L2 + L3) or r < abs(L2 - L3):
        raise ValueError("Target point not reachable.")
    
    # Using the law of cosines to find theta_3
    cos_theta_3 = (L2**2 + L3**2 - r**2) / (2 * L2 * L3)
    theta_3 = math.acos(cos_theta_3)

    # Using the law of cosines to find theta_2
    cos_theta_2 = (L2**2 + r**2 - L3**2) / (2 * L2 * r)
    theta_2 = math.atan2(h, d) - math.acos(cos_theta_2)

    return theta_1, theta_2, theta_3

def plot_leg(x, y, z, L1, L2, L3):
    body_height = 5
    theta_1, theta_2, theta_3 = inverse_kinematics(x, y, z, L1, L2, L3)

    # Compute joint positions
    j1_x = L1 * math.cos(theta_1)
    j1_y = L1 * math.sin(theta_1)
    j1_z = body_height

    j2_x = j1_x + L2 * math.sin(theta_2) * math.cos(theta_1)
    j2_y = j1_y + L2 * math.sin(theta_2) * math.sin(theta_1)
    j2_z = j1_z - L2 * math.cos(theta_2)

    foot_x = j2_x
    foot_y = j2_y
    foot_z = j2_z - L3  # Subtracting L3 since it's always pointing downward
    
    # Plot using matplotlib
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    ax.scatter([0, j1_x, j2_x, foot_x], 
               [0, j1_y, j2_y, foot_y], 
               [body_height, j1_z, j2_z, foot_z], c='r', marker='o')

    ax.plot([0, j1_x], [0, j1_y], [body_height, j1_z], 'b-')
    ax.plot([j1_x, j2_x], [j1_y, j2_y], [j1_z, j2_z], 'b-')
    ax.plot([j2_x, foot_x], [j2_y, foot_y], [j2_z, foot_z], 'b-')

    ax.set_xlim([-L1-L2-L3, L1+L2+L3])
    ax.set_ylim([-L1-L2-L3, L1+L2+L3])
    ax.set_zlim([0, body_height + L1])

    ax.set_xticks([-10, -8, -6, -4, -2, 0,2,4,6,8,10])
    ax.set_yticks([-10, -8, -6, -4, -2, 0,2,4,6,8,10])
    ax.set_zticks([0,2,4,6,8,10])


    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    
    plt.show()


# Test the function
# x, y, z = 10, 10, -5  # Example target foot position
# L1, L2, L3 = 1, 5, 5  # Example segment lengths


# Test the leg placement at height near the body
x, y, z = 10, 15, 0  # Example target foot position (z=0 since it's the floor level)
L1, L2, L3 = 5, 10, 10  # Example segment lengths

angles = inverse_kinematics(x, y, z, L1, L2, L3)
print(f"Joint Angles: Theta1 = {math.degrees(angles[0])}°, Alpha = {math.degrees(angles[1])}°, Beta = {math.degrees(angles[2])}°")
# plot_leg(x, y, z, L1, L2, L3)

theta_1, theta_2, theta_3 = math.radians(56.309), math.radians(-24.7594), math.radians(88.48)  # some example angles
U_x, U_y, U_z = forward_kinematics(theta_1, theta_2, theta_3, L1, L2, L3)
print(f"End effector position: X:{U_x}, Y:{U_y}, Z:{U_z}")
