from coords import Coords
from time import sleep
from adafruit_servokit import ServoKit
# Other imports are at the bottom of the file


BASE_SERVO_ID = 0
SHOULDER_SERVO_ID = 1
ELBOW_SERVO_ID = 2



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
        self.kit = None
        #self.kit = ServoKit(channels=8)

    def walk(self):
        pass

    def interactive_effector_control(self, leg_id: int):
        # xyz pozice na relativne normalni pozici nohou:
        init_x = 7
        init_y = 1
        init_z = 8

        angles = None
        old_angles = None

        increment = 1
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

                angles = self.kinematics.inverse_kinematics(self.legs[leg_id], Coords(init_x, init_y, init_z))
                print("elbow, shoulder, base angles:", angles)

                ## kit.servo[0].angle = map_range(angles[2], -90, 90, 0, 180)
                ## kit.servo[1].angle = map_range(angles[1], -90, 90, 0, 180)
                ## kit.servo[2].angle = map_range(angles[0], -90, 90, 0, 180)

                # self.legs[leg_id].set_angle(BASE_SERVO_ID, angles[2])
                # self.legs[leg_id].set_angle(SHOULDER_SERVO_ID, angles[1])
                # self.legs[leg_id].set_angle(ELBOW_SERVO_ID, angles[0])

            except:
                print("Invalid angles")
                angles = old_angles


    def interactive_angle_control(self, leg_id: int):
        # init angles of the arm
        init_base_angle = 100
        init_shoulder_angle = 50
        init_elbow_angle = 100
        increment = 10

        # TODO: zjistit, na jakych pozcich xyz se effector pohybuje

        cmd = ""
        while cmd != "e":
            cmd = input()
            if cmd == "a":
                init_base_angle = init_base_angle + increment if init_base_angle + increment < 180 else 180
            elif cmd == "d":
                init_base_angle = init_base_angle - increment if init_base_angle - increment > 0 else 0

            if cmd == "w":
                init_shoulder_angle = init_shoulder_angle + increment if init_shoulder_angle + increment < 180 else 180
            elif cmd == "s":
                init_shoulder_angle = init_shoulder_angle - increment if init_shoulder_angle - increment > 0 else 0

            if cmd == "r":
                init_elbow_angle = init_elbow_angle + increment if init_elbow_angle + increment < 180 else 180
            elif cmd == "f":
                init_elbow_angle = init_elbow_angle - increment if init_elbow_angle - increment > 0 else 0

            print("angle base", init_base_angle)
            print("angle shoulder", init_shoulder_angle)
            print("angle elbow", init_elbow_angle)

            self.kit.servo[2].angle = init_base_angle
            self.kit.servo[1].angle = init_shoulder_angle
            self.kit.servo[0].angle = init_elbow_angle

            # self.set_angle(self.BASE_SERVO_ID, init_elbow_angle)
            # self.set_angle(self.SHOULDER_SERVO_ID, init_shoulder_angle)
            # self.set_angle(self.ELBOW_SERVO_ID, init_base_angle)

            print(self.kinematics.forward_kinematics(self.legs[leg_id], init_base_angle, init_shoulder_angle, init_elbow_angle))



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
        assert(servo_id < 3)
        assert(servo_id >= 0)
        print("Angle", angle)
        self.kit.servo[3 * self.leg_idx + servo_id].angle = angle


# Prevent Circular import error
from m_kinematics import Kinematics