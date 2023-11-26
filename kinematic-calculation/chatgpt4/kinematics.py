import numpy as np
import matplotlib.pyplot as plt

def forward_kinematics(theta, alpha, beta, L1, L2):
    x = L1 * np.sin(theta) * np.cos(alpha) + L2 * np.sin(theta) * np.cos(alpha + beta)
    y = L1 * np.cos(theta) * np.cos(alpha) + L2 * np.cos(theta) * np.cos(alpha + beta)
    z = L1 * np.sin(alpha) + L2 * np.sin(alpha + beta)
    return x, y, z

def plot_arm_3d(theta, alpha, beta, L1, L2):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    
    base = np.array([0, 0, 0])
    o1 = base + np.array([L1*np.sin(theta)*np.cos(alpha), L1*np.cos(theta)*np.cos(alpha), L1*np.sin(alpha)])
    end_effector = o1 + np.array([L2*np.sin(theta)*np.cos(alpha+beta), L2*np.cos(theta)*np.cos(alpha+beta), L2*np.sin(alpha+beta)])
    
    ax.plot([base[0], o1[0]], [base[1], o1[1]], [base[2], o1[2]], 'ro-')
    ax.plot([o1[0], end_effector[0]], [o1[1], end_effector[1]], [o1[2], end_effector[2]], 'bo-')
    
    ax.set_xticks([-10, -8, -6, -4, -2, 0,2,4,6,8,10])
    ax.set_yticks([-10, -8, -6, -4, -2, 0,2,4,6,8,10])
    ax.set_zticks([0,2,4,6,8,10])


    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('Robotic Arm in 3D')
    plt.show()

# Test
theta, alpha, beta, L1, L2 = 90, 45, 45, 6, 2
plot_arm_3d(theta, alpha, beta, L1, L2)
