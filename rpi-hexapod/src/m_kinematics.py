import math

from coords import Coords
from utils import *

# Prevent Circular import error
from m_hexapod import HexapodLeg


class ServoAngles:
    def __init__(self, base_angle, shoulder_angle, elbow_angle):
        self.base_angle = base_angle         # Coxa
        self.shoulder_angle = shoulder_angle # Femur  
        self.elbow_angle = elbow_angle       # Tibia

    def __eq__(self, other): 
        if not isinstance(other, ServoAngles):
            # don't attempt to compare against unrelated types
            return NotImplemented

        return self.base_angle == other.base_angle and self.shoulder_angle == other.shoulder_angle and self.elbow_angle == other.elbow_angle

    def __repr__(self):
        return "ServoAngles()"
    
    def __str__(self):
        return f"({self.base_angle}, {self.shoulder_angle}, {self.elbow_angle})"

    def print(self):
        print(self.base_angle, self.shoulder_angle, self.elbow_angle)
        print()



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
            theta1 = 90 - math.degrees(alpha) # Originalni hodnoty
            # theta1 = 180 - math.degrees(alpha)  # Prizpusobeni pro FK
            # theta1 = - (180 - math.degrees(alpha))  # Prizpusobeni pro FK a otocit uhly
            # theta1 = 280 - math.degrees(alpha)  # Prizpusobeni pro realne uhly pro servo motory

            # Shoulder angle:
            theta2 = 90 - (math.degrees(gamma) + math.degrees(beta))  # Originalni hodnoty
            # theta2 = 90 - (math.degrees(gamma) + math.degrees(beta))  # Prizpusobeni pro FK
            # TODO: if theta2 < 0 then reverse sign (Chci, aby pavouk vzdy udrzoval ten klasicky tvar nohy)
            # theta2 = - (90 - (math.degrees(gamma) + math.degrees(beta)))  # Prizpusobeni pro FK a otocit uhly
            # theta2 = - (140 - (math.degrees(gamma) + math.degrees(beta)))  # Prizpusobeni pro realne uhly pro servo motory

            # Base angle:
            theta_base = math.degrees(math.atan2(x, y))  # Originalni hodnoty
            # TODO: if theta_base > 0 then reverse sign (Chci, aby pavouk vzdy udrzoval ten klasicky tvar nohy)
            # theta_base = 90 - math.degrees(math.atan2(x, y))  # Prizpusobeni pro FK (Aby odpovidalo FK <=> IK)
            # theta_base = 10 + math.degrees(math.atan2(x, y)) # Prizpusobeni pro realne uhly pro servo motory
            
            theta_base, theta2, theta1 = self.modify_angles(theta1, theta2, theta_base, 0,0,-90)

            return (theta_base, theta2, theta1)
        
        except:
            print("Invalid angle")

        return (0, 0, 0)
    
    def modify_angles(self, theta1, theta2, theta_base, theta1_mod, theta2_mod, theta_base_mod):
        # modify angles to match servo ranges
        theta1 = round(theta1, 3)
        theta2 = round(theta2, 3)
        theta_base = round(theta_base, 3)

        # theta1 = map_range(theta1, -360, 360, 0, 180)
        # theta2 = map_range(theta2, -90, 90, 0, 180)
        # theta_base = map_range(theta_base, -360, 360, 0, 180)

        #theta1 = abs(theta1)
        #theta2 = abs(theta2)
        #theta_base = abs(theta_base)

        theta1 = theta1_mod + theta1
        theta2 = theta2_mod + theta2
        theta_base = theta_base_mod + theta_base

        return (theta_base, theta2, theta1)
        
    