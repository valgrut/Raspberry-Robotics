from coords import Coords
from time import sleep
from adafruit_servokit import ServoKit

class Hexapod:
    def __init__(self):
        self.body = None
        self.legs = [None, None, None, None, None, None]
        # 0 = Front left
        # 1 = Front Right
        # 2 = Middle Left
        # 3 = Middle Right
        # 4 = Back Left
        # 5 = Back Right

        self.kinematics = Kinematics()
        self.kit = ServoKit(channels=8)

    def walk(self):
        pass


class HexapodLeg:
    def __init__(self, hexapod: Hexapod, leg_idx, coxa_len, femur_len, tibia_len):
        # Leg definition
        self.hexapod = hexapod
        self.hexapod.legs[leg_idx] = self
        self.leg_idx = leg_idx

        # self.leg_placement_coords = leg_placement_coords
        #self.body_coxa_distance = body_coxa_distance
        self.coxa_len = coxa_len
        self.femur_len = femur_len
        self.tibia_len = tibia_len
        self.current_effector_pos = Coords(0,0,0)

        self.kinematics = self.hexapod.kinematics
        self.kit = self.hexapod.kit

        # Konstants
        self.BASE_SERVO_ID = 3 * self.leg_idx + 0
        self.SHOULDER_SERVO_ID = 3 * self.leg_idx + 1
        self.ELBOW_SERVO_ID = 3 * self.leg_idx + 2


    def move_to_point(self, end_point: Coords):
        pass

    def move_leg_X(self, value):
        # Move leg along X-axis
        pass

    def move_leg_Y(self, value):
        # Move leg along Y-axis
        pass

    def move_leg_Z(self, value):
        # Move leg along Z-axis
        pass

    
    def draw_x_line(self):
        # Move leg back and forth along X axis
        for x in range(10, 20):
            print(self.kinematics.inverse_kinematics(self, Coords(x, 0, 10)))
            angles = self.kinematics.inverse_kinematics(self, Coords(x, 0, 10))

            #self.set_angle(self.BASE_SERVO_ID, angles[0])
            #self.set_angle(self.SHOULDER_SERVO_ID, angles[1])
            #self.set_angle(self.ELBOW_SERVO_ID, angles[2])

            #sleep(2)


    def draw_y_line(self):
        # Move leg back and forth along Y axis
        for y in range(5, 15):
            print(self.kinematics.inverse_kinematics(self, Coords(10, y, 0)))


    def draw_z_line(self):
        # Move leg back and forth along Z axis
        for z in range(0, 10):
            print(self.kinematics.inverse_kinematics(self, Coords(10, 0, z)))



    def set_angle(self, servo_id: int, angle):
        self.kit.servo[servo_id].angle = angle

    def interactive_effector_control(self, leg_id: int):
        # xyz pozice na relativne normalni pozici nohou:
        init_x = 7 #75
        init_y = 1 #120
        init_z = 8 #100

        angles = None
        old_angles = None

        increment = 1 #10
        cmd = ""
        while cmd != "e":
            cmd = input()
            if cmd == "w":
                init_x -= increment
            elif cmd == "s":
                init_x += increment

            if cmd == "a":
                init_y += increment
            elif cmd == "d":
                init_y -= increment

            if cmd == "r":
                init_z += increment
            elif cmd == "f":
                init_z -= increment

            print("new x", init_z)
            print("new y", init_y)
            print("new z", init_x)

            try:
                old_angles = angles
                angles = self.kinematics.inverse_kinematics(self, Coords(init_x, init_y, init_z))

                # kit.servo[0].angle = map_range(angles[2], -90, 90, 0, 180)
                # kit.servo[1].angle = map_range(angles[1], -90, 90, 0, 180)
                # kit.servo[2].angle = map_range(angles[0], -90, 90, 0, 180)

                self.set_angle(self.BASE_SERVO_ID, angles[2])
                self.set_angle(self.SHOULDER_SERVO_ID, angles[1])
                self.set_angle(self.ELBOW_SERVO_ID, angles[0])

                print(self.kinematics.inverse_kinematics(self, Coords(init_x, init_y, init_z)))
            except:
                print("Invalid angles")
                angles = old_angles


    def interactive_angle_control(self, leg_id: int):
        # Angles:
        # TODO: Chtelo by to Forward Kinematic !!! a zjistit, na
        # jakych pozcich xyz se effector pohybuje
        init_base_angle = 70
        init_shoulder_angle = 120
        init_elbow_angle = 100
        increment = 10

        cmd = ""
        while cmd != "e":
            cmd = input()
            if cmd == "w":
                init_base_angle = init_base_angle + increment if init_base_angle + increment < 180 else 180
            elif cmd == "s":
                init_base_angle = init_base_angle - increment if init_base_angle - increment > 0 else 0

            if cmd == "a":
                init_shoulder_angle = init_shoulder_angle + increment if init_shoulder_angle + increment < 180 else 180
            elif cmd == "d":
                init_shoulder_angle = init_shoulder_angle - increment if init_shoulder_angle - increment > 0 else 0

            if cmd == "r":
                init_elbow_angle = init_elbow_angle + increment if init_elbow_angle + increment < 180 else 180
            elif cmd == "f":
                init_elbow_angle = init_elbow_angle - increment if init_elbow_angle - increment > 0 else 0

            print("angle x", init_base_angle)
            print("angle y", init_shoulder_angle)
            print("angle z", init_elbow_angle)

            self.kit.servo[1].angle = init_base_angle
            self.kit.servo[0].angle = init_shoulder_angle
            self.kit.servo[2].angle = init_elbow_angle

            # self.set_angle(self.BASE_SERVO_ID, init_elbow_angle)
            # self.set_angle(self.SHOULDER_SERVO_ID, init_shoulder_angle)
            # self.set_angle(self.ELBOW_SERVO_ID, init_base_angle)

            print(self.kinematics.forward_kinematics(self, init_base_angle, init_shoulder_angle, init_elbow_angle))

# Prevent Circular import error
from m_kinematics import Kinematics