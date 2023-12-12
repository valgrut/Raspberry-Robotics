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


    def forward_kinematics(self, leg: HexapodLeg, source_angles: ServoAngles) -> Coords:
        L1 = leg.coxa_len  # 5
        L2 = leg.femur_len  # 6.5
        L3 = leg.tibia_len  # 12

        # Update angles to match the actual servo ranges (0-180 degrees)
        updated_angles: ServoAngles = self.modify_angles(source_angles)
        source_angles = updated_angles

        # Goniometrical fcs(sin, cos, ...) takes radians as an input, not the degrees!
        a0 = math.radians(source_angles.base_angle)  # 45
        a1 = math.radians(source_angles.shoulder_angle)  # 0
        a2 = math.radians(source_angles.elbow_angle)  # 20

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
        x = target.x + 23.5
        y = target.y
        z = target.z + 7 #Konec nohy ma v Rest pozici z-souradnici zhruba o 7cm niz, nez je telo.

        if y == 0:
            y = 0.0000001

        L1 = leg.coxa_len
        L2 = leg.femur_len
        L3 = leg.tibia_len

        try:
            L = math.sqrt(x**2 + y**2)
            Lt = math.sqrt((L - L1)**2 + z**2)
            gamma = math.atan((L - L1) / z)
            beta = math.acos((L3**2 - L2**2 - Lt**2) / (-2*L2*Lt))
            alpha = math.acos((Lt**2 - L2**2 - L3**2) / (-2*L2*L3))

            # Elbow angle:
            theta1 = 90 - math.degrees(alpha) # Originalni hodnoty
            # theta1 = 180 - math.degrees(alpha)  # Prizpusobeni pro FK
            # theta1 = - (180 - math.degrees(alpha))  # Prizpusobeni pro FK a otocit uhly
            # theta1 = 280 - math.degrees(alpha)  # Prizpusobeni pro realne uhly pro servo motory

            # Shoulder angle:
            theta2 = 90 - (math.degrees(gamma) + math.degrees(alpha))  # Originalni hodnoty
            # theta2 = 90 - (math.degrees(gamma) + math.degrees(beta))  # Prizpusobeni pro FK
            # TODO: if theta2 < 0 then reverse sign (Chci, aby pavouk vzdy udrzoval ten klasicky tvar nohy)
            # theta2 = - (90 - (math.degrees(gamma) + math.degrees(beta)))  # Prizpusobeni pro FK a otocit uhly
            # theta2 = - (140 - (math.degrees(gamma) + math.degrees(beta)))  # Prizpusobeni pro realne uhly pro servo motory

            # Base angle:
            theta_base = math.degrees(math.atan(x / y))  # Originalni hodnoty
            # TODO: if theta_base > 0 then reverse sign (Chci, aby pavouk vzdy udrzoval ten klasicky tvar nohy)
            # theta_base = 90 - math.degrees(math.atan2(x, y))  # Prizpusobeni pro FK (Aby odpovidalo FK <=> IK)
            # theta_base = 10 + math.degrees(math.atan2(x, y)) # Prizpusobeni pro realne uhly pro servo motory
            
            updated_angles: ServoAngles = self.modify_angles(ServoAngles(theta_base, theta2, theta1))

            return updated_angles
        
        except Exception as e:
            print(e)

            #return ServoAngles(0, 0, 0)
    
    def modify_angles(self, source_angles: ServoAngles) -> ServoAngles:
        # modify angles to match servo ranges
        theta_elbow = round(source_angles.elbow_angle, 3)
        theta_shoulder = round(source_angles.shoulder_angle, 3)
        theta_base = round(source_angles.base_angle, 3)

        # theta_elbow = map_range(theta_elbow, -360, 360, 0, 180)
        # theta_shoulder = map_range(theta_shoulder, -90, 90, 0, 180)
        # theta_base = map_range(theta_base, -360, 360, 0, 180)

        #theta_elbow = abs(theta_elbow)
        #theta_shoulder = abs(theta_shoulder)
        #theta_base = abs(theta_base)

        theta_elbow_mod = 180
        theta_shoulder_mod = 50
        theta_base_mod = 0

        theta_elbow = theta_elbow_mod + theta_elbow
        theta_shoulder = theta_shoulder_mod + theta_shoulder
        theta_base = theta_base_mod + theta_base

        # Because IK gives one of the 2 possible solutions, we have to catch the bad one.
        if theta_shoulder < 0:
            theta_shoulder += 123.114
            theta_elbow = 180 - theta_elbow

        if theta_shoulder < theta_elbow:
            theta_shoulder = 180 - theta_shoulder
            theta_elbow = 180 - theta_elbow


        # TODO: Zjistit, jak zkontrolovat, ze cilove (xyz) je v dosahu.
        # Kdyz neni, tak IK hazi zaporne uhly pro nedostupne xyz, a nebo uhly > 180.
        # Oba pripady nelze hodit do serv, protoze ty berou pouze 0-180.
        # warn = False
        # if theta_base < 0:
        #     theta_base = 0
        #     warn = True
        # elif theta_base > 180:
        #     theta_base = 180
        #     warn = True

        # if theta_shoulder < 0:
        #     theta_shoulder = 0
        #     warn = True
        # elif theta_shoulder > 180:
        #     theta_shoulder = 180
        #     warn = True

        # if theta_elbow < 0:
        #     theta_elbow = 0
        #     warn = True
        # elif theta_elbow > 180:
        #     theta_elbow = 180
        #     warn = True
        
        # if warn:
        #     print("Warning: Nedosazitelne x/y/z. Uhly modifikovany.")

        return ServoAngles(theta_base, theta_shoulder, theta_elbow)
        
    