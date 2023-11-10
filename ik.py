import math
import numpy as np
from matplotlib import pyplot as plt

class bodyJoint:
    def __init__(self):
        self.x = 0
        self.y = 0

L1 = 5
L2 = 4

theta1 = 5  # Degrees
theta2 = 90  # Degrees

theta1_rad = theta1 * math.pi/180
theta2_rad = theta2 * math.pi/180
print(theta1_rad, theta2_rad)

x1 = L1 * math.cos(theta1_rad)
y1 = L1 * math.sin(theta1_rad)

x2 = x1 + L2 * math.cos(theta1_rad + theta2_rad)
y2 = y1 + L2 * math.sin(theta1_rad + theta2_rad)

print(x2, y2)

data = np.array([
    [0, 0],
    [x1, y1],
    [x2, y2],
])
x, y = data.T
plt.xlim(-10, 10)
plt.ylim(-10, 10)
plt.scatter(x,y)

plt.plot(x[0:2], y[0:2], 'ro-')
plt.plot(x[1:3], y[1:3], 'ro-')

plt.show()

