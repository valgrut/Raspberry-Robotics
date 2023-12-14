from coords import Coords
import math

# Description of the problem:
#   Brute force method for finding the correct offsets for the elbow and shoulder angles.
#   Those offsets are required because of the real limits of the cheap servos, not corresponding to the
#   simplified scheme of the leg, for which the IK and FK was calculated previously.
#
#   The catch is that my servos have only 0-130 degrees available motion. I can assign 180, but
#   those 180 degrees will be squished into those ~130 degrees, so the angles will be off
#   and so all the angles between.
# 
#   For example I assign 90 degrees and expect perfect angle,
#   but servo will turn just to 50 degree. This caused all the frustration because both forward and
#   inverse kinematics were giving wrong results.

def forward_kinematics(base_offset, shoulder_offset, elbow_offset) -> Coords:
    """
    Forward Kinematics
    @input: Joint angles
    @output: Coordinates of the end effector based on the input angles
    """
    L1 = 5
    L2 = 6.4
    L3 = 12

    # INPUT ANGLES FOR WHICH WE WILL BE FINDING the offset to end on the Coord Range given below.
    input_base_angle = 70
    input_shoulder_angle = 60
    input_elbow_offset = 130

    a0 = math.radians(base_offset - input_base_angle)  # Source Base angle
    a1 = math.radians(shoulder_offset - input_shoulder_angle)  # Source Shoulder angle
    a2 = math.radians(elbow_offset - input_elbow_offset) # Source Elbow angle

    p0 = Coords(0, 0, 3.2)  # Coords(0, 0, 0)
    p1 = Coords(p0.x + L1, 0, p0.z)
    p2 = Coords(p1.x + L2 * math.cos(a1), 0, p1.z + L2 * math.sin(a1))
    p3 = Coords(p2.x + L3 * math.cos(a1 + a2), 0, round(p2.z + L3 * math.sin(a1 + a2), 2))
    E3 = Coords(round(p3.x * math.cos(a0), 3),
                round(p3.x * math.sin(a0), 3),
                round(p3.z, 3))

    return E3


################## USAGE ########################
# Try all combinations of the offsets for elbow and shoulder angles.
closest_results_cnt = 0
angle_incr = 5  # or 2 for increased granularity
for shoulder_offset in range(0, 200, angle_incr):
    for elbow_offset in range(0, 200, angle_incr):
        effector_coords = forward_kinematics(70, shoulder_offset, elbow_offset)
        
        # We are finding such offsets to add to the input angles that will cause to end-effector to end up
        # in the given coordinate range closest to the estimated one (by eye).
        if 23.0 <= effector_coords.x <= 24 and effector_coords.y == 0 and 0.1 > effector_coords.z > - 0.1:
            print("Possible correct angles offset combination:")
            print(effector_coords)
            print("shoulder offset:", shoulder_offset)
            print("elbow offset", elbow_offset)
            print()
            closest_results_cnt += 1

print("Closest results:", closest_results_cnt)