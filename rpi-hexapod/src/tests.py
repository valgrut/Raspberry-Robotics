from coords import Coords

class Tests:
    def __init__(self):
        pass

    def run_fk_tests(self):
        from m_hexapod import Hexapod, HexapodLeg
        from m_kinematics import Kinematics
        from utils import map_range


        kinematics = Kinematics()
        
        hexapod = Hexapod()
        leg = HexapodLeg(hexapod, 0, 5, 6.5, 12)

        # Testing:
        print("RESULT | INPUT | EXPECTED | ACTUAL_OUT")
        self.test((0,0,0), Coords(23.5, 0, 0), kinematics.forward_kinematics(leg, 0, 0, 0))
        self.test((90,0,0), Coords(0, 23.5, 0), kinematics.forward_kinematics(leg, 90, 0, 0))
        self.test((-90,0,0), Coords(0, -23.5, 0), kinematics.forward_kinematics(leg, -90, 0, 0))
        self.test((0,90,0), Coords(5, 0, 18.5), kinematics.forward_kinematics(leg, 0, 90, 0))
        self.test((0,-90,0), Coords(5, 0, -18.5), kinematics.forward_kinematics(leg, 0, -90, 0))
        self.test((90,90,0), Coords(0, 5, 18.5), kinematics.forward_kinematics(leg, 90, 90, 0))
        self.test((90,90,90), Coords(0, -7, 6.5), kinematics.forward_kinematics(leg, 90, 90, 90))
        self.test((0,45,45), Coords(9.596, 0, 16.596), kinematics.forward_kinematics(leg, 0, 45, 45))
        self.test((0,90,-90), Coords(17, 0, 6.5), kinematics.forward_kinematics(leg, 0, 90, -90))
        print()   

        # Results from IK
        print("IK:")
        #self.test((45, 31, -75), Coords(0, 0, 0), kinematics.forward_kinematics(leg, 45, 31, -75))
        self.test(Coords(5, 0, 18.5), (0, 90, 0), kinematics.inverse_kinematics(leg, Coords(5, 0, 18.5)))
        self.test(Coords(23.5, 0, 0), (0, 0, 0), kinematics.inverse_kinematics(leg, Coords(23.5, 0, 0)))
        print()
        self.test(Coords(20, 0, 0), (0, 0, 0), kinematics.inverse_kinematics(leg, Coords(20, 0, 0)))
        self.test(Coords(18, 0, 0), (0, 0, 0), kinematics.inverse_kinematics(leg, Coords(18, 0, 0)))



    def test(self, input, expected_coords: Coords, actual_coords: Coords):
        if expected_coords == actual_coords:
            print("[PASS]", input, ":", expected_coords, "==", actual_coords)
        else:
            print("[FAIL]", input, ":", expected_coords, "=!=", actual_coords)
