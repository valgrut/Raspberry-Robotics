from coords import Coords

class HexapodLeg:
    def __init__(self, leg_idx, leg_placement_coords: Coords, body_coxa_distance, coxa_len, femur_len, tibia_len):
        # Leg definition
        self.leg_idx = leg_idx
        self.leg_placement_coords = leg_placement_coords
        self.body_coxa_distance = body_coxa_distance
        self.coxa_len = coxa_len
        self.femur_len = femur_len
        self.tibia_len = tibia_len
        self.current_effector_pos = Coords(0,0,0)

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


class Hexapod:
    def __init__(self):
        self.body = None
        self.legs = []

    def walk(self):
        pass
