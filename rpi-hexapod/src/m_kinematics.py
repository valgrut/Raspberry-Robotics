import math

from coords import Coords
from utils import *

# Prevent Circular import error
from m_hexapod import HexapodLeg

class Kinematics:
    def __init__(self):
        pass

    # FUNGUJE!!! (Opravdu)
    def calc_2D_fk(self, P0, coxa_len, femur_len, a0, a1):
        print(P0[0], P0[1])
        a0_rad = math.radians(a0)
        a1_rad = math.radians(a1)

        P1 = (P0[0] + coxa_len * math.cos(a0_rad), P0[1] + coxa_len * math.sin(a0_rad))
        print(round(P1[0], 3), round(P1[1], 3))
        P2 = (P1[0] + femur_len * math.cos(a0_rad + a1_rad), P1[1] + femur_len * math.sin(a0_rad + a1_rad))
        print(round(P2[0], 3), round(P2[1], 3))


    def forward_kinematics(self, leg: HexapodLeg, base_angle, shoulder_angle, knee_angle) -> Coords:
        L1 = leg.coxa_len  # 5
        L2 = leg.femur_len  # 6.5
        L3 = leg.tibia_len  # 12

        # Goniometrical fcs(sin, cos, ...) takes radians as an input, not the degrees!
        a0 = math.radians(base_angle)  # 45
        a1 = math.radians(shoulder_angle)  # 0
        a2 = math.radians(knee_angle)  # 20

        p0 = Coords(0,0,0)
        p1 = Coords(p0.x + L1, 0, p0.z)
        p2 = Coords(p1.x + L2 * math.cos(a1), 0, p1.z + L2 * math.sin(a1))
        p3 = Coords(p2.x + L3 * math.cos(a1 + a2), 0, p2.z + L3 * math.sin(a1 + a2))
        E3 = Coords(round(p3.x * math.cos(a0), 3), 
                    round(p3.x * math.sin(a0), 3), 
                    round(p3.z, 3))

        # print(p0.x, p0.y, p0.z)
        # print(p1.x, p1.y, p1.z)
        # print(p2.x, p2.y, p2.z)
        # print(p3.x, p3.y, p3.z)
        # print(E3.x, E3.y, E3.z)

        return E3


    def calc_2D_ik(self, femur_len, tibia_len, tx, ty):
        pass


    def inverse_kinematics(self, leg: HexapodLeg, target: Coords):
        x = target.x
        y = target.y
        z = target.z

        L1 = leg.coxa_len
        L2 = leg.femur_len
        L3 = leg.tibia_len

        try:
            L = math.sqrt(x**2 + y**2)
            Lt = math.sqrt((L - L1)**2 + z**2)
            gamma = math.atan2((L - L1), z)
            beta = math.acos((L3**2 - L2**2 - Lt**2) / (-2*L2*Lt))
            alpha = math.acos((Lt**2 - L2**2 - L3**2) / (-2*L2*L3))

            # Elbow angle:
            # theta1 = 90 - math.degrees(alpha) # Originalni hodnoty
            # theta1 = 180 - math.degrees(alpha)  # Prizpusobeni pro FK
            theta1 = - (180 - math.degrees(alpha))  # Prizpusobeni pro FK a otocit uhly

            # Shoulder angle:
            # theta2 = 90 - (math.degrees(gamma) + math.degrees(beta))  # Originalni hodnoty
            theta2 = 90 - (math.degrees(gamma) + math.degrees(beta))  # Prizpusobeni pro FK
            # TODO: if theta2 < 0 then reverse sign (Chci, aby pavouk vzdy udrzoval ten klasicky tvar nohy)
            theta2 = - (90 - (math.degrees(gamma) + math.degrees(beta)))  # Prizpusobeni pro FK a otocit uhly

            # Base angle:
            # theta3 = math.degrees(math.atan2(x, y))  # Originalni hodnoty
            # TODO: if theta3 > 0 then reverse sign (Chci, aby pavouk vzdy udrzoval ten klasicky tvar nohy)
            theta3 = 90 - math.degrees(math.atan2(x, y))  # Prizpusobeni pro FK
            
            return (theta3, theta2, theta1)
        
        except:
            print("Invalid angle")

        return (0, 0, 0)
        
    