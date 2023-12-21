from coords import Coords
from hexapod import Hexapod, HexapodLeg
from kinematics import Kinematics, ServoAngles

class Tests:
    def __init__(self):
        pass

    def run_tests(self, leg: HexapodLeg):
        # Testing:
        # print("Testing Forward Kinematics")
        # print("RESULT | INPUT | EXPECTED | ACTUAL_OUT")
        # self.run_fk_test(leg, ServoAngles(0,0,0), expected_output=Coords(23.5, 0, 0))
        # self.run_fk_test(leg, ServoAngles(90, 0, 0), expected_output=Coords(0, 23.5, 0))
        # self.run_fk_test(leg, ServoAngles(-90, 0, 0), expected_output=Coords(0, -23.5, 0))
        # self.run_fk_test(leg, ServoAngles(0, 90, 0), expected_output=Coords(5, 0, 18.5))
        # self.run_fk_test(leg, ServoAngles(0, -90, 0), expected_output=Coords(5, 0, -18.5))
        # self.run_fk_test(leg, ServoAngles(90, 90, 0), expected_output=Coords(0, 5, 18.5))
        # self.run_fk_test(leg, ServoAngles(90, 90, 90), expected_output=Coords(0, -7, 6.5))
        # self.run_fk_test(leg, ServoAngles(0, 45, 45), expected_output=Coords(9.596, 0, 16.596))
        # self.run_fk_test(leg, ServoAngles(0, 90, -90), expected_output=Coords(17, 0, 6.5))

        # self.run_fk_test(leg, ServoAngles(90, 50, 90), expected_output=Coords(23.5, 0, 0))
        # print()

        # FORWARD KINEMATICS TESTS
        print("Testing Forward Kinematics")
        print("RESULT | INPUT | EXPECTED | ACTUAL_OUT")
        self.run_fk_test(leg, ServoAngles(70, 60, 130), expected_output=Coords(23.1, 0, 0)) #Natazena noha 
        self.run_fk_test(leg, ServoAngles(0, 130, 60), expected_output=Coords(6.1, 16.8, -5.2))
        self.run_fk_test(leg, ServoAngles(130, 130, 60), expected_output=Coords(9.0, -15.5, -5.2))
        self.run_fk_test(leg, ServoAngles(70, 110, 100), expected_output=Coords(18.6, 0, -8.3))

        print()
        self.run_fk_test(leg, ServoAngles(0, -90, 0), expected_output=Coords(5, 0, -18.5))
        self.run_fk_test(leg, ServoAngles(90, 90, 0), expected_output=Coords(0, 5, 18.5))
        self.run_fk_test(leg, ServoAngles(90, 90, 90), expected_output=Coords(0, -7, 6.5))
        self.run_fk_test(leg, ServoAngles(0, 45, 45), expected_output=Coords(9.596, 0, 16.596))
        self.run_fk_test(leg, ServoAngles(0, 90, -90), expected_output=Coords(17, 0, 6.5))

        self.run_fk_test(leg, ServoAngles(90, 50, 90), expected_output=Coords(23.5, 0, 0))
        print() 



        # INVERSE KINEMATICS TESTS
        print("Testing Inverse Kinematics:")
        self.run_ik_test(leg, Coords(11.6, 0, 12.1), expected_output=ServoAngles(70, 60, 130))

        self.run_ik_test(leg, Coords(23.1, 0, 0), expected_output=ServoAngles(70, 60, 130))

        self.run_ik_test(leg, Coords(0, 23.5, 0), expected_output=ServoAngles(0, 50, 90))
        self.run_ik_test(leg, Coords(0, -23.5, 0), expected_output=ServoAngles(180, 50, 90))
        self.run_ik_test(leg, Coords(15, 15, 0), expected_output=ServoAngles(45, 9.854999999999997, 150.584))
        self.run_ik_test(leg, Coords(15, -15, 0), expected_output=ServoAngles(135, 9.854999999999997, 150.584))
        print()

        self.run_ik_test(leg, Coords(5, 0, 18.5), expected_output=ServoAngles(90, None, 90))
        # print(kinematics.forward_kinematics(leg, kinematics.inverse_kinematics(leg, Coords(5, 0, 18.5))))
        self.run_ik_test(leg, Coords(5, 0, -18.5), expected_output=ServoAngles(90, None, 90))

        print()
        
        self.run_ik_test(leg, Coords(11.5, 0, 12), expected_output=ServoAngles(90, 50, 180))
        self.run_ik_test(leg, Coords(11.5, 0, -12), expected_output=ServoAngles(90, 50, 0))
        print()

        self.run_ik_test(leg, Coords(19, 0, 5), expected_output=ServoAngles(90, 50, 90))
        

    def run_ik_test(self, leg: HexapodLeg, test_data: Coords, expected_output: ServoAngles):
        kinematics = Kinematics()
        self.compare_results(test_data, expected_output, kinematics.inverse_kinematics(leg, test_data))


    def run_fk_test(self, leg: HexapodLeg, test_data: ServoAngles, expected_output: Coords):
        kinematics = Kinematics()
        self.compare_results(test_data, expected_output, kinematics.forward_kinematics(leg, test_data))


    def compare_results(self, input, expected_coords, actual_coords):
        if expected_coords == actual_coords:
            print("[PASS]", input, ":", expected_coords, "==", actual_coords)
        else:
            print("[FAIL]", input, ":", expected_coords, "=!=", actual_coords)
