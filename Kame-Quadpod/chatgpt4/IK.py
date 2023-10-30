import math
import matplotlib.pyplot as plt

def inverse_kinematics(x, y, z, L1, L2, L3):
    """
    Compute joint angles based on the target x, y, z coordinates.
    
    Parameters:
    - x, y, z: Target foot position in the robot's coordinate system.
    - L1, L2, L3: Lengths of the Coxa, Femur, and Tibia respectively.

    Returns:
    - theta_1, theta_2, theta_3: Joint angles in radians.
    """
    
    # Coxa Angle
    theta_1 = math.atan2(z, math.sqrt(x**2 + y**2))
    
    # Planar distance to target
    d = math.sqrt(x**2 + y**2)
    
    # Distance from the end of the Coxa to the foot target
    c = math.sqrt(d**2 + (z - L1 * math.sin(theta_1))**2)
    
    # Using law of cosines to compute the angle at the Femur
    cos_theta_2 = (L2**2 + L3**2 - c**2) / (2 * L2 * L3)
    theta_2 = math.acos(cos_theta_2)
    
    # Tibia angle
    theta_3 = math.pi - theta_2
    
    return theta_1, theta_2, theta_3

# Test the function
x, y, z = 10, -10, -15  # Example target foot position
L1, L2, L3 = 5, 10, 10  # Example segment lengths
angles = inverse_kinematics(x, y, z, L1, L2, L3)
print(f"Joint Angles: Theta1 = {math.degrees(angles[0])}°, Theta2 = {math.degrees(angles[1])}°, Theta3 = {math.degrees(angles[2])}°")

# def plot_arm(x, y, z, L1, L2, L3):
#     theta_1, theta_2, theta_3 = inverse_kinematics(x, y, z, L1, L2, L3)
    
#     # Compute joint positions
#     coxa_x, coxa_y, coxa_z = 0, 0, L1 * math.sin(theta_1)
#     femur_x, femur_y, femur_z = 0, 0, coxa_z - L1 * math.cos(theta_1)
#     tibia_x, tibia_y, tibia_z = femur_x + L2 * math.sin(theta_2), femur_y, femur_z - L2 * math.cos(theta_2)
    
#     # Plot using matplotlib
#     fig = plt.figure()
#     ax = fig.add_subplot(111, projection='3d')
    
#     # Plot joints and links
#     ax.scatter([0, coxa_x, femur_x, tibia_x], 
#                [0, coxa_y, femur_y, tibia_y], 
#                [0, coxa_z, femur_z, tibia_z], c='r', marker='o')
    
#     ax.plot([0, coxa_x], [0, coxa_y], [0, coxa_z], 'b-')
#     ax.plot([coxa_x, femur_x], [coxa_y, femur_y], [coxa_z, femur_z], 'b-')
#     ax.plot([femur_x, tibia_x], [femur_y, tibia_y], [femur_z, tibia_z], 'b-')
    
#     ax.set_xlim([-L1-L2-L3, L1+L2+L3])
#     ax.set_ylim([-L1-L2-L3, L1+L2+L3])
#     ax.set_zlim([0, L1+L2+L3])
#     ax.set_xlabel('X')
#     ax.set_ylabel('Y')
#     ax.set_zlabel('Z')
    
#     plt.show()

# # Test the plotting function
# x, y, z = 10, 5, -5  # Example target foot position
# L1, L2, L3 = 5, 5, 2  # Example segment lengths
# plot_arm(x, y, z, L1, L2, L3)



