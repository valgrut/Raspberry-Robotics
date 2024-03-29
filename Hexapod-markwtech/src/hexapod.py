from coords import Coords
from time import sleep
from adafruit_servokit import ServoKit
# Other imports are at the bottom of the file
from utils import map_range
from gate_engine import GateEngine
import math

BASE_SERVO_ID = 2
SHOULDER_SERVO_ID = 1
ELBOW_SERVO_ID = 0

class SharedLegParameters:
    def __init__(self, coxa_len=0.0, femur_len=0.0, tibia_len=0.0, max_angle=180):
        self.coxa_len = coxa_len
        self.femur_len = femur_len
        self.tibia_len = tibia_len
        self.MAX_ANGLE = max_angle
        # TODO: dokoncit toto a pouzit pri inicializaci nohou.

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

        self.movement_mode = None #Ripple, Wave, Side,

    def command(self, command):
        # This should contain all commands available for the hexapod.
        # The commands are independent of the input device (Bluetooth / Flask web server).

        # Something like:
        
        # Controller: for example: X button
        if command == "SET_RIPPLE_MODE":
            self.movement_mode = "RIPPLE_MODE"
        
        # Controller: for example: Y button
        elif command == "SET_WAVE_MODE":
            self.movement_mode = "RIPPLE_MODE"
        
        # Controller: for example: A button
        elif command == "SET_TRIPOD_MODE":
            self.movement_mode = "TRIPOD_MODE"
        
        if command == "CAPTURE_PHOTO":
            pass

        if command == "START_RECORDING":
            if self.is_recording is False:
                self.is_recording = True
        
        elif command == "STOP_RECORDING":
            if self.is_recording is True:
                self.is_recording = False
        
        


    def walk(self):
        pass

    def interactive_effector_control(self, leg_id: int):
        # xyz pozice na relativne normalni pozici nohou:
        init_x = 19
        init_y = 0
        init_z = 5

        angles = None
        old_angles = None

        # TODO: Pygame: while w is pressed ...
        increment = 1
        cmd = ""
        while cmd != "e":
            cmd = input()
            if cmd == "w":
                init_x += increment
            elif cmd == "s":
                init_x -= increment

            if cmd == "a":
                init_y -= increment
            elif cmd == "d":
                init_y += increment

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
        init_base_angle = 90
        init_shoulder_angle = 0
        init_elbow_angle = 100
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
                    init_base_angle = 90
                    init_shoulder_angle = 0
                    init_elbow_angle = 100
                
                if cmd == "p":
                    do_process = False

                print("angle base", init_base_angle)
                print("angle shoulder", init_shoulder_angle)
                print("angle elbow", init_elbow_angle)

                self.legs[leg_id].set_angle(BASE_SERVO_ID, init_base_angle)
                self.legs[leg_id].set_angle(SHOULDER_SERVO_ID, init_shoulder_angle)
                self.legs[leg_id].set_angle(ELBOW_SERVO_ID, init_elbow_angle)

                # # Front right leg:
                # self.legs[leg_id + 1].set_angle(BASE_SERVO_ID, init_base_angle)
                # self.legs[leg_id + 1].set_angle(SHOULDER_SERVO_ID, init_shoulder_angle)
                # self.legs[leg_id + 1].set_angle(ELBOW_SERVO_ID, init_elbow_angle)

                target_angles = ServoAngles(init_base_angle, init_shoulder_angle, init_elbow_angle)
                print(self.kinematics.forward_kinematics(self.legs[leg_id], target_angles))

                if len(commands_row) > 1:
                    sleep(0.5)



class HexapodLeg:
    def __init__(self, hexapod: Hexapod, leg_idx, placement_offset, shared_params: SharedLegParameters):
        # Leg definition
        self.hexapod = hexapod
        self.hexapod.legs[leg_idx] = self
        self.leg_idx = leg_idx
        self.inversed = False

        self.leg_placement_offset = placement_offset
        self.coxa_len = shared_params.coxa_len
        self.femur_len = shared_params.femur_len
        self.tibia_len = shared_params.tibia_len
        # self.current_effector_pos = Coords(0,0,0)

        self.kinematics = self.hexapod.kinematics

        self.kit = self.hexapod.kit
        self.MAXIMAL_SERVO_ANGLE = shared_params.MAX_ANGLE  # 130 gives correct ~90 degrees

        # Servos are imperfect, so Trying to set the whole 0-180 range, instead of default 0 - cca 160:
        # self.kit.servo[3 * self.leg_idx + 0].set_pulse_width_range(1000, 2000)
        # self.kit.servo[3 * self.leg_idx + 1].set_pulse_width_range(1000, 2000)
        # self.kit.servo[3 * self.leg_idx + 2].set_pulse_width_range(1000, 2000)

        # If the range is really 0-140, then set it as max to compensate the difference:
        self.kit.servo[3 * self.leg_idx + 0].actuation_range = self.MAXIMAL_SERVO_ANGLE
        self.kit.servo[3 * self.leg_idx + 1].actuation_range = self.MAXIMAL_SERVO_ANGLE
        self.kit.servo[3 * self.leg_idx + 2].actuation_range = self.MAXIMAL_SERVO_ANGLE

    def set_inversed(self):
        self.inversed = True

    def unset_inversed(self):
        self.inversed = False

    #def move_to_point(self, start_point: Coords, target_point: Coords):
    def move_to_point(self, target_point: Coords):
        print("Moving to point", target_point)

        angles = self.kinematics.inverse_kinematics(self, target_point)
        print("Calculated angles: (base, shoulder, elbow): ", angles)

        self.set_angle(BASE_SERVO_ID, angles.base_angle)
        self.set_angle(SHOULDER_SERVO_ID, angles.shoulder_angle)
        self.set_angle(ELBOW_SERVO_ID, angles.elbow_angle)

        # TODO: Add approximation from the current Coord to the target_coord


    def move_leg_X(self, value):
        # Move leg along X-axis
        pass

    def move_leg_Y(self, value):
        # Move leg along Y-axis
        pass

    def move_leg_Z(self, value):
        # Move leg along Z-axis
        pass

    def __calculate_arc_between_2_points(self, start_point: Coords, target_point: Coords, number_of_points: int, max_step_height: float):
        # Calculates arc above two points A and B with sinusoidal z height.
        #
        #          . 
        #     .    |   .
        #   .      h     .
        #  A       |     B
        #
        # Does not work for different z values for the points.
        # if A.z != B.z:
        #     print("Error: A and B has to have the same Z value!")
        #     return []
        
        import numpy as np
        # A = np.array([0, 6, 0])
        # B = np.array([8, 1, 0])
        A = np.array(start_point.list())
        B = np.array(target_point.list())

        distance = B - A
        num_of_points = number_of_points
        max_arc_height = max_step_height
        positive_sine_len = math.pi
        distance = distance / num_of_points

        R = []
        for i in range(0, num_of_points):
            R.append(Coords(A[0] + distance[0] * i, A[1] + distance[1] * i, A[2] + max_arc_height * round(math.sin((positive_sine_len / num_of_points) * i), 3)))
        R.append(Coords(B[0], B[1], B[2]))
        
        return R

    # just straight line between two points A(0,6) and B(8, 1) with z = 0
    def __calculate_line_between_2_points(self, start_point: Coords, target_point: Coords, number_of_points):
        import numpy as np
        
        A = np.array(start_point.list())
        B = np.array(target_point.list())
        
        D = B - A
        num_of_points = number_of_points
        D = D / num_of_points

        R = []
        for i in range(0, num_of_points):
            R.append(Coords(A[0] + D[0] * i, A[1] + D[1] * i, A[2] + D[2] * i))
        R.append(Coords(B[0], B[1], B[2]))

        return R

    def swing_from_point_to_point(self, start_point: Coords, target_point: Coords, num_of_points=10, max_step_height=5, increment=0.1, speed=0.05):
        # Move leg back and forth along X axis

        R = self.__calculate_arc_between_2_points(start_point, target_point, num_of_points, max_step_height)
        for point in R:
            print(point, ":", self.kinematics.inverse_kinematics(self, point))
            angles = self.kinematics.inverse_kinematics(self, point)

            self.set_angle(BASE_SERVO_ID, angles.base_angle)
            self.set_angle(SHOULDER_SERVO_ID, angles.shoulder_angle)
            self.set_angle(ELBOW_SERVO_ID, angles.elbow_angle)

            sleep(speed)

    def stance_from_point_to_point(self, start_point: Coords, target_point: Coords, num_of_points=10, increment=0.1, speed=0.05):
        # Move leg back and forth along X axis

        R = self.__calculate_line_between_2_points(start_point, target_point, num_of_points)
        for point in R:
            print(point, ":", self.kinematics.inverse_kinematics(self, point))
            angles = self.kinematics.inverse_kinematics(self, point)

            self.set_angle(BASE_SERVO_ID, angles.base_angle)
            self.set_angle(SHOULDER_SERVO_ID, angles.shoulder_angle)
            self.set_angle(ELBOW_SERVO_ID, angles.elbow_angle)

            sleep(speed)



    def draw_x_line(self, start_x, target_x, increment=0.1, speed=0.05):
        # Move leg back and forth along X axis
        x_increment = increment
        x_coord = start_x
        condition = x_coord < target_x if start_x < target_x else target_x < x_coord
        while condition: 
            print(Coords(x_coord, 0, -2), ":", self.kinematics.inverse_kinematics(self, Coords(x_coord, 0, -2)))
            angles = self.kinematics.inverse_kinematics(self, Coords(x_coord, 0, -2))

            self.set_angle(BASE_SERVO_ID, angles.base_angle)
            self.set_angle(SHOULDER_SERVO_ID, angles.shoulder_angle)
            self.set_angle(ELBOW_SERVO_ID, angles.elbow_angle)
            
            x_coord = x_coord + x_increment if start_x < target_x else x_coord - x_increment
            condition = x_coord < target_x if start_x < target_x else target_x < x_coord

            sleep(speed)

    def draw_y_line(self, start_x, target_x, increment=0.1, speed=0.05):
        # Move leg back and forth along X axis
        x_increment = increment
        x_coord = start_x
        condition = x_coord < target_x if start_x < target_x else target_x < x_coord
        while condition: 
            print(Coords(15, x_coord, -2), ":", self.kinematics.inverse_kinematics(self, Coords(15, x_coord, -2)))
            angles = self.kinematics.inverse_kinematics(self, Coords(15, x_coord, -2))

            self.set_angle(BASE_SERVO_ID, angles.base_angle)
            self.set_angle(SHOULDER_SERVO_ID, angles.shoulder_angle)
            self.set_angle(ELBOW_SERVO_ID, angles.elbow_angle)
            
            x_coord = x_coord + x_increment if start_x < target_x else x_coord - x_increment
            condition = x_coord < target_x if start_x < target_x else target_x < x_coord

            sleep(speed)


    def draw_z_line(self, start_x, target_x, increment=0.1, speed=0.05):
        # Move leg back and forth along X axis
        x_increment = increment
        x_coord = start_x
        condition = x_coord < target_x if start_x < target_x else target_x < x_coord
        while condition: 
            print(Coords(15, 0, x_coord), ":", self.kinematics.inverse_kinematics(self, Coords(15, 0, x_coord)))
            angles = self.kinematics.inverse_kinematics(self, Coords(15, 0, x_coord))

            self.set_angle(BASE_SERVO_ID, angles.base_angle)
            self.set_angle(SHOULDER_SERVO_ID, angles.shoulder_angle)
            self.set_angle(ELBOW_SERVO_ID, angles.elbow_angle)
            
            x_coord = x_coord + x_increment if start_x < target_x else x_coord - x_increment
            condition = x_coord < target_x if start_x < target_x else target_x < x_coord

            sleep(speed)


    def set_angle(self, servo_id: int, angle):
        assert(servo_id < 3)
        assert(servo_id >= 0)
        assert(angle <= self.MAXIMAL_SERVO_ANGLE)

        if self.inversed:  # if leg is Right, servos have mirror orientation
            self.kit.servo[3 * self.leg_idx + servo_id].angle = self.MAXIMAL_SERVO_ANGLE - angle
        else:    
            self.kit.servo[3 * self.leg_idx + servo_id].angle = angle


# Prevent Circular import error
from kinematics import Kinematics, ServoAngles