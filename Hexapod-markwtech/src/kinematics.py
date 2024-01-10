import math

from coords import Coords
from utils import *
from dataclasses import dataclass

# Prevent Circular import error
from hexapod import HexapodLeg

@dataclass
class ServoAngles:
    base_angle :float      # Coxa
    shoulder_angle :float  # Femur
    elbow_angle :float     # Tibia


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
        """
        Forward Kinematics for 3-DoF hexapod leg.
        @input: Joint angles.
        @output: Coordinates of the end effector based on the input angles.
        """
        L1 = leg.coxa_len
        L2 = leg.femur_len
        L3 = leg.tibia_len

        # Angle offsets to match 0-180 servos ranges
        base_angle_offset = 70
        shoulder_angle_offset = 50  # offset found by my brute forcing
        elbow_angle_offset = 130  # offset found by my brute forcing

        # Apply offsets
        updated_base_angle = base_angle_offset - source_angles.base_angle
        updated_shoulder_angle = shoulder_angle_offset - source_angles.shoulder_angle
        updated_elbow_angle = elbow_angle_offset - source_angles.elbow_angle

        updated_angles = ServoAngles(updated_base_angle, updated_shoulder_angle, updated_elbow_angle)

        # Convert input angles to radians
        a0 = math.radians(updated_angles.base_angle)
        a1 = math.radians(updated_angles.shoulder_angle)
        a2 = math.radians(updated_angles.elbow_angle)

        # Calculating joint coords from the base to the end effector.
        p0 = Coords(leg.leg_placement_offset.x, leg.leg_placement_offset.y, leg.leg_placement_offset.z)
        p1 = Coords(p0.x + L1, 0, p0.z)
        p2 = Coords(p1.x + L2 * math.cos(a1), 0, p1.z + L2 * math.sin(a1))
        p3 = Coords(p2.x + L3 * math.cos(a1 + a2), 0, p2.z + L3 * math.sin(a1 + a2))
        E3 = Coords(round(p3.x * math.cos(a0), 1),
                    round(p3.x * math.sin(a0), 1),
                    round(p3.z, 1))

        # print("P0 base", p0.x, p0.y, p0.z)
        # print("P1 shou", p1.x, p1.y, p1.z)
        # print("P2 elbo", p2.x, p2.y, p2.z)
        # print("P3 end-", p3.x, p3.y, p3.z)
        # print("E3 rota", E3.x, E3.y, E3.z)

        return E3


    def calc_2D_ik(self, femur_len, tibia_len, tx, ty):
        pass


    def inverse_kinematics(self, leg: HexapodLeg, target: Coords):
        x = target.x
        y = target.y
        z = target.z

        # Avoid zero-division
        x += 0.00000001
        y += 0.00000001
        z += 0.00000001

        L1 = leg.coxa_len
        L2 = leg.femur_len
        L3 = leg.tibia_len

        # IK according to the PROVIDED!!! scheme!!!
        # LC je moje L1
        # L0 je moje Lt
        # L3 je moje L
        Lt = math.sqrt(x**2 + y**2) - L1
        L = math.sqrt(Lt**2 + z**2)

        phi_elbow = 0
        phi_shoulder = 0
        gamma_shoulder = 0

        try:
            phi_elbow = math.degrees(math.acos((L2**2 + L3**2 - L**2) / (2 * L2 * L3)))
        except Exception as e:
            print(e)
            return None

        try:
            phi_shoulder = math.degrees(math.acos((L2**2 + L**2 - L3**2) / (2 * L2 * L)))
        except Exception as e:
            print(e)
            return None

        try:
            gamma_shoulder = math.degrees(math.atan2(z, Lt))
        except Exception as e:
            print(e)
            return None

        theta_shoulder = phi_shoulder + gamma_shoulder + 50    # Original is +14 + 90
        theta_elbow = phi_elbow - 113 + 90    # Original is -113 + 90
        theta_base = math.degrees(math.atan2(x, y)) - 20   # Original is +90

        # Prevent angles to exceed maximal real value of the servos
        final_angles = self.modify_angles(ServoAngles(theta_base, theta_shoulder, theta_elbow))

        return final_angles


    def modify_angles(self, source_angles: ServoAngles) -> ServoAngles:
        # modify angles to match servo ranges
        theta_elbow = round(source_angles.elbow_angle, 3)
        theta_shoulder = round(source_angles.shoulder_angle, 3)
        theta_base = round(source_angles.base_angle, 3)

        # TODO: Zjistit, jak zkontrolovat, ze cilove (xyz) je v dosahu.
        # Kdyz neni, tak IK hazi zaporne uhly pro nedostupne xyz, a nebo uhly > 180.
        # Oba pripady nelze hodit do serv, protoze ty berou pouze 0-180.
        warn = False
        if theta_base < 0:
            theta_base = 0
            warn = True
        elif theta_base > 130:
            theta_base = 130
            warn = True

        if theta_shoulder < 0:
            theta_shoulder = 0
            warn = True
        elif theta_shoulder > 130:
            theta_shoulder = 130
            warn = True

        if theta_elbow < 0:
            theta_elbow = 0
            warn = True
        elif theta_elbow > 130:
            theta_elbow = 130
            warn = True

        if warn:
            print("Warning: Nedosazitelne x/y/z. Uhly modifikovany.")

        return ServoAngles(theta_base, theta_shoulder, theta_elbow)


