import math
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def plot_robot_arm(theta_R1, theta_O1, theta_O2, L1, L2):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
 
    # Define joint positions
    x_points = [
        0, 
        L1 * math.cos(theta_O1), 
        L1 * math.cos(theta_O1) + L2 * math.cos(theta_O1 + theta_O2)
    ]
    y_points = [
        0, 
        L1 * math.sin(theta_O1), 
        L1 * math.sin(theta_O1) + L2 * math.sin(theta_O1 + theta_O2)
    ]
    z_points = [
        0, 
        0, 
        0
    ]
    
    # Rotate points around z-axis by theta_R1
    x_rotated = math.cos(theta_R1) * np.array(x_points[1:]) - math.sin(theta_R1) * np.array(y_points[1:])
    y_rotated = math.sin(theta_R1) * np.array(x_points[1:]) + math.cos(theta_R1) * np.array(y_points[1:])
    
    x_points[1:] = x_rotated
    y_points[1:] = y_rotated
    # Define joint positions
    # x_points = [
    #     0,
    #     L1 * math.cos(theta_R1) * math.cos(theta_O1),
    #     L1 * math.cos(theta_R1) * math.cos(theta_O1 + theta_O2)
    # ]
    # y_points = [
    #     0,
    #     L1 * math.cos(theta_R1) * math.sin(theta_O1),
    #     L1 * math.cos(theta_R1) * math.sin(theta_O1 + theta_O2)
    # ]
    # z_points = [
    #     0,
    #     -L1 * math.sin(theta_R1),
    #     -L1 * math.sin(theta_R1)
    # ]

    # x_points = [0, L1 * math.cos(theta1), L1 * math.cos(theta1) + L2 * math.cos(theta1 + theta2)]
    # y_points = [0, L1 * math.sin(theta1), L1 * math.sin(theta1) + L2 * math.sin(theta1 + theta2)]
    # z_points = [0, 0, 0]
    
    # Plot joints
    ax.scatter(x_points, y_points, z_points, c='r', marker='o', s=100)
    
    # Plot links
    ax.plot(x_points, y_points, z_points, c='b')
    
    ax.set_xticks([-10, -8, -6, -4, -2, 0,2,4,6,8,10])
    ax.set_yticks([-10, -8, -6, -4, -2, 0,2,4,6,8,10])
    ax.set_zticks([0,2,4,6,8,10])

    # Set axis labels
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    
    plt.show()

# Example usage:
theta1 = math.radians(90)  # z-axis rotator
theta2 = math.radians(45)  # spodni joint
theta3 = math.radians(45)   # horni joint
L1 = 5  # Length from O1 to O2
L2 = 2  # Length from O2 to c

plot_robot_arm(theta1, theta2, theta3, L1, L2)
