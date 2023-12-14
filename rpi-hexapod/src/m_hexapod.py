from coords import Coords
from time import sleep
from adafruit_servokit import ServoKit
# Other imports are at the bottom of the file
from utils import map_range

BASE_SERVO_ID = 2
SHOULDER_SERVO_ID = 1
ELBOW_SERVO_ID = 0



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
        self.kit = ServoKit(channels=16)

    def walk(self):
        pass

    def interactive_effector_control(self, leg_id: int):
        # xyz pozice na relativne normalni pozici nohou:
        init_x = 19
        init_y = 0
        init_z = 5

        angles = None
        old_angles = None

        increment = 1
        cmd = ""
        while cmd != "e":
            cmd = input()
            if cmd == "w":
                init_x += increment
            elif cmd == "s":
                init_x -= increment

            if cmd == "a":
                init_y += increment
            elif cmd == "d":
                init_y -= increment

            if cmd == "r":
                init_z += increment
            elif cmd == "f":
                init_z -= increment

            print("new x", init_x)
            print("new y", init_y)
            print("new z", init_z)

            try:
                old_angles = angles

                angles = self.kinematics.inverse_kinematics(self.legs[leg_id], Coords(init_x, init_y, init_z))
                print("elbow, shoulder, base angles:", angles)

                self.legs[leg_id].set_angle(BASE_SERVO_ID, angles.base_angle)
                self.legs[leg_id].set_angle(SHOULDER_SERVO_ID, angles.shoulder_angle)
                self.legs[leg_id].set_angle(ELBOW_SERVO_ID, angles.elbow_angle)

            except Exception as e:
                print(e.__cause__)
                angles = old_angles


    def interactive_angle_control(self, leg_id: int):
        # init angles of the arm
        init_base_angle = 70
        init_shoulder_angle = 90
        init_elbow_angle = 90
        increment = 10

        SERVO_ANGLE_LIMIT = self.legs[leg_id].MAXIMAL_SERVO_ANGLE

        do_process = True
        commands_row = ""
        while do_process:
            commands_row = input()
            for cmd in commands_row:

                if cmd == "a":
                    init_base_angle = init_base_angle + increment if init_base_angle + increment < SERVO_ANGLE_LIMIT else SERVO_ANGLE_LIMIT
                elif cmd == "d":
                    init_base_angle = init_base_angle - increment if init_base_angle - increment > 0 else 0

                if cmd == "w":
                    init_shoulder_angle = init_shoulder_angle + increment if init_shoulder_angle + increment < SERVO_ANGLE_LIMIT else SERVO_ANGLE_LIMIT
                elif cmd == "s":
                    init_shoulder_angle = init_shoulder_angle - increment if init_shoulder_angle - increment > 0 else 0

                if cmd == "r":
                    init_elbow_angle = init_elbow_angle + increment if init_elbow_angle + increment < SERVO_ANGLE_LIMIT else SERVO_ANGLE_LIMIT
                elif cmd == "f":
                    init_elbow_angle = init_elbow_angle - increment if init_elbow_angle - increment > 0 else 0

                if cmd == "b":
                    init_base_angle = 70
                    init_shoulder_angle = 90
                    init_elbow_angle = 90
                
                if cmd == "p":
                    do_process = False

                print("angle base", init_base_angle)
                print("angle shoulder", init_shoulder_angle)
                print("angle elbow", init_elbow_angle)

                final_base_angle = init_base_angle
                final_shoulder_angle = init_shoulder_angle
                final_elbow_angle = init_elbow_angle
                # final_base_angle = map_range(init_base_angle, 0, 180, 0, 140)
                # final_shoulder_angle = map_range(init_shoulder_angle, 0, 180, 0, 140)
                # final_elbow_angle = map_range(init_elbow_angle, 0, 180, 0, 140)

                self.legs[leg_id].set_angle(BASE_SERVO_ID, init_base_angle)
                self.legs[leg_id].set_angle(SHOULDER_SERVO_ID, final_shoulder_angle)
                self.legs[leg_id].set_angle(ELBOW_SERVO_ID, final_elbow_angle)

                target_angles = ServoAngles(init_base_angle, final_shoulder_angle, final_elbow_angle)
                print(self.kinematics.forward_kinematics(self.legs[leg_id], target_angles))

                if len(commands_row) > 1:
                    sleep(0.5)



class HexapodLeg:
    def __init__(self, hexapod: Hexapod, leg_idx, coxa_len, femur_len, tibia_len):
        # Leg definition
        self.hexapod = hexapod
        self.hexapod.legs[leg_idx] = self
        self.leg_idx = leg_idx

        self.leg_placement_offset = Coords(0, 0, 3.2)
        self.coxa_len = coxa_len
        self.femur_len = femur_len
        self.tibia_len = tibia_len
        # self.current_effector_pos = Coords(0,0,0)

        self.kinematics = self.hexapod.kinematics

        self.kit = self.hexapod.kit
        self.MAXIMAL_SERVO_ANGLE = 130  # 130 gives correct ~90 degrees

        # Servos are imperfect, so Trying to set the whole 0-180 range, instead of default 0 - cca 160:
        # self.kit.servo[3 * self.leg_idx + 0].set_pulse_width_range(1000, 2000)
        # self.kit.servo[3 * self.leg_idx + 1].set_pulse_width_range(1000, 2000)
        # self.kit.servo[3 * self.leg_idx + 2].set_pulse_width_range(1000, 2000)

        # If the range is really 0-140, then set it as max to compensate the difference:
        self.kit.servo[3 * self.leg_idx + 0].actuation_range = self.MAXIMAL_SERVO_ANGLE
        self.kit.servo[3 * self.leg_idx + 1].actuation_range = self.MAXIMAL_SERVO_ANGLE
        self.kit.servo[3 * self.leg_idx + 2].actuation_range = self.MAXIMAL_SERVO_ANGLE


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
        for x in range(18, 23):
            print(self.kinematics.inverse_kinematics(self, Coords(x, 0, 10)))
            angles = self.kinematics.inverse_kinematics(self, Coords(x, 0, 10))

            #self.set_angle(self.BASE_SERVO_ID, angles[0])
            #self.set_angle(self.SHOULDER_SERVO_ID, angles[1])
            #self.set_angle(self.ELBOW_SERVO_ID, angles[2])

            #sleep(2)

    def draw_y_line(self):
        # Move leg back and forth along Y axis
        for y in range(5, 15):
            print(self.kinematics.inverse_kinematics(self, Coords(19, y, 0)))


    def draw_z_line(self):
        # Move leg back and forth along Z axis
        for z in range(0, 10):
            print(self.kinematics.inverse_kinematics(self, Coords(19, 0, z)))


    def set_angle(self, servo_id: int, angle):
        assert(servo_id < 3)
        assert(servo_id >= 0)
        #print("Angle", angle)
        self.kit.servo[3 * self.leg_idx + servo_id].angle = angle


# Prevent Circular import error
from m_kinematics import Kinematics, ServoAngles