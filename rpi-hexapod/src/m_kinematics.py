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


    def __ik(self, leg: HexapodLeg, target: Coords):
        # TODO: Pridat constraints

        tx = target.x
        ty = target.y
        tz = target.z

        a1 = leg.coxa_len
        a2 = leg.femur_len
        a3 = leg.tibia_len

        theta1 = math.degrees(math.atan2(ty, tx))  # [1]
        r1 = math.sqrt(tx**2 + ty**2)  # - leg.body_coxa_distance # [2]

        # if r1 > a2 + a3:
            # print("Unreachable point")
            # return None

        r2 = tz - a1  # [3]
        phi2 = math.atan2(r2, r1)  # [4]
        
        r3 = math.sqrt(r1**2 + r2**2)  # [5]
        phi1 = math.acos((a3**2 - a2**2 - r3**2) / (-2*a2*r3))  # [6]
        theta2 = math.degrees(phi2 - phi1)  # [7]

        phi3 = math.acos((r3**2 - a2**2 -a3**2) / (-2*a2*a3))  # [8]
        theta3 = math.degrees(phi3)  # [9]

        return (theta1, theta2, theta3)


    def __ik_dle_clanku(self, leg: HexapodLeg, target: Coords):
        x0 = target.x
        y0 = target.y
        z0 = target.z

        L2 = leg.femur_len
        L3 = leg.tibia_len

        theta1 = 0
        theta2 = 0
        theta3 = 0

        try:
            theta1 = math.atan2(y0, x0)
            theta2 = math.acos((-L3**2 + L2**2 + x0**2 + y0**2 + z0**2) / (2*L2*math.sqrt(x0**2 + y0**2 + z0**2))) 
            + math.atan2(z0, (math.sqrt(x0**2 + y0**2)))
            theta3 = -math.acos((x0**2 + y0**2 + z0**2 - L2**2 - L3**2) / (2*L2*L3))
        except:
            print("Invalid angle")

        # Angles update
        theta1 = math.degrees(theta1)
        theta2 = math.degrees(theta2)
        theta3 = math.degrees(theta3)

        #print("ik_dle_clanku: pred", theta1, theta2, theta3)

        #theta1 = theta1 if theta1 > 0 else 180 + theta1
        #theta2 = theta2 if theta2 > 0 else 180 + theta2
        # theta1 = map_range(theta1, -90, 90, 0, 180)
        #theta2 = map_range(theta2, -90, 90, 0, 180)
        #heta3 = map_range(theta3, -90, 90, 0, 180)
        # theta3 = theta3 if theta3 > 0 else 180 + theta3

        #print("ik_dle_clanku: po", theta1, theta2, theta3)

        return (theta1, theta2, theta3)
    
    def __ik_dalsi_pokus(self, leg: HexapodLeg, target: Coords):
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
            theta2 = - (90 - (math.degrees(gamma) + math.degrees(beta)))  # Prizpusobeni pro FK a otocit uhly

            # Base angle:
            # theta3 = math.degrees(math.atan2(x, y))  # Originalni hodnoty
            theta3 = 90 - math.degrees(math.atan2(x, y))  # Prizpusobeni pro FK
        except:
            print("Invalid angle")

        return (theta3, theta2, theta1)
    

    def inverse_kinematics(self, leg: HexapodLeg, target: Coords):
        return self.__ik_dalsi_pokus(leg, target)

