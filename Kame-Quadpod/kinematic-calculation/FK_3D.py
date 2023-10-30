## Forward kinematics

import math
import matplotlib.pyplot as plt

# Initial values
L1 = 8
L2 = 2
# L3 = 0.5
theta1 = 0.0*math.pi
theta2 = 0.0*math.pi
theta3 = 0.0*math.pi

def FK_3D(theta1, theta2, theta_yaw):
    x1 = L1 * math.cos(theta1)
    y1 = L1 * math.cos(theta1)
    z1 = L1 * math.sin(theta1)

    x2 = x1 + L2 * math.cos(theta1 + theta2)
    y2 = y1 + L2 * math.cos(theta1 + theta2)
    z2 = z1 + L2 * math.sin(theta1 + theta2)

    x3 = x2 * math.cos(theta_yaw)
    y3 = y2 * math.sin(theta_yaw)
    z3 = z2

    return [(x1,y1,z1), (x2,y2,z2), (x3,y3,z3)]

# Calculate various points of effectors for various angles
x1_points = []
y1_points = []
z1_points = []

x2_points = []
y2_points = []
z2_points = []

x3_points = []
y3_points = []
z3_points = []
for th1 in range(0, 90, 10):
    for th2 in range(-180, 0, 20):
        for th_yaw in range(-90, 90, 10):
            p1, p2, p3 = FK_3D(th1, th2, th_yaw)

            # First link position
            x1_points.append(p1[0])
            y1_points.append(p1[1])
            z1_points.append(p1[2])
            
            x2_points.append(p2[0])
            y2_points.append(p2[1])
            z2_points.append(p2[2])

            # Effector's position
            x3_points.append(p3[0])
            y3_points.append(p3[1])
            z3_points.append(p3[2])

# Init matplotlib space
fig = plt.figure()
ax = fig.add_subplot(projection='3d')

# Plot all final points of effector
ax.scatter(0, 0, 0)
# ax.scatter(x1_points, y1_points, z1_points)
# ax.scatter(x2_points, y2_points, z2_points)
# ax.scatter(x3_points, y3_points, z3_points)

# draw single effector robot arm from above angles
# hi = 40
# ax.plot([0,x1_points[hi]], [0,y1_points[hi]], [0,z1_points[hi]])
# ax.plot([x1_points[hi], x2_points[hi]], [y1_points[hi], y2_points[hi]], [z1_points[hi], z2_points[hi]])
# ax.plot([x2_points[hi], x3_points[hi]], [y2_points[hi], y3_points[hi]], [z2_points[hi], z3_points[hi]])


p1, p2, p3 = FK_3D(45, 0, 0)
ax.plot([0,p1[0]], [0,p1[1]], [0,p1[2]])
ax.plot([p1[0],p2[0]], [p1[1],p2[1]], [p1[2],p2[2]])
# ax.plot([p2[0],p3[0]], [p2[1],p3[1]], [p2[2],p3[2]])

p1, p2, p3 = FK_3D(45, 0, 20)
ax.plot([0,p1[0]], [0,p1[1]], [0,p1[2]])
ax.plot([p1[0],p2[0]], [p1[1],p2[1]], [p1[2],p2[2]])

p1, p2, p3 = FK_3D(45, 0, 90)
ax.plot([0,p1[0]], [0,p1[1]], [0,p1[2]])
ax.plot([p1[0],p2[0]], [p1[1],p2[1]], [p1[2],p2[2]])

# Draw single hand positions
# p1, p2, p3 = FK_3D(90, 90, 100)
# ax.plot([0,p1[0]], [0,p1[1]], [0,p1[2]])
# ax.plot([p1[0],p2[0]], [p1[1],p2[1]], [p1[2],p2[2]])
# ax.plot([p2[0],p3[0]], [p2[1],p3[1]], [p2[2],p3[2]])


# Show plot
plt.show()
