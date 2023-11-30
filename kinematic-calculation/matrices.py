import math
import numpy as np

def rotate_x(point, theta):
    c = np.cos(theta)
    s = np.sin(theta)
    Rx = np.array([
        [1, 0, 0],
        [0, c , -s],
        [0, s, c]
        ])
    rotated_point = np.dot(Rx, point)
    return rotated_point

def rotate_y(point, theta):
    c = np.cos(theta)
    s = np.sin(theta)
    Ry = np.array([
        [c, 0, s],
        [0, 1 , 0],
        [-s, 0, c]
        ])
    rotated_point = np.dot(Ry, point)
    return rotated_point

def rotate_z(point, theta):
    c = np.cos(theta)
    s = np.sin(theta)
    Rz = np.array([
        [c, -s, 0],
        [s, c , 0],
        [0, 0, 1]
        ])

    rotated_point = np.dot(Rz, point)
    return rotated_point

theta = np.radians(30)
point = np.array([10, 0, 10])
print(rotate_x(point, theta))
print(rotate_y(point, theta))
print(rotate_z(point, theta))


