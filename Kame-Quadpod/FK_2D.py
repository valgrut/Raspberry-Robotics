## Forward kinematics

import math
import matplotlib.pyplot as plt

# Initial values
L1 = 2
L2 = 2
theta1 = 0.0*math.pi
theta2 = 0.0*math.pi

def FK_2D(theta1, theta2):
    x1 = L1 * math.cos(theta1)
    y1 = L1 * math.sin(theta1)

    x = x1 + L2 * math.cos(theta1 + theta2)
    y = y1 + L2 * math.sin(theta1 + theta2)

    return [(x1,y1), (x,y)]

# Calculate various points of effectors for various angles
x1_points = []
y1_points = []
x_points = []
y_points = []
for th1 in range(-180, 180, 30):
    for th2 in range(-90, 90, 30):
        new_point = FK_2D(th1, th2)

        # First link position
        x1_points.append(new_point[0][0])
        y1_points.append(new_point[0][1])
        
        # Effector's position
        x_points.append(new_point[1][0])
        y_points.append(new_point[1][1])

# Plot all final points of effector
plt.plot(x1_points, y1_points, 'ro')
plt.plot(x_points, y_points, 'ro')

# draw single effector robot arm
hands_i = 20
plt.plot([0, x1_points[hands_i]], [0, y1_points[hands_i]], 'k-', lw=2)
plt.plot([x1_points[hands_i], x_points[hands_i]], [y1_points[hands_i], y_points[hands_i]], 'k-', lw=2)

# Show plot
plt.show()
