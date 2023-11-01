# Pro plynuly pohyb na zacatku a konci prizpusobit zmenu uhlu dle rozsahu

starting_angle = 58
ending_angle = 155
steps = [2, 4, 8, 10, 10, 10, 10, 10, 8, 4, 2]

def get_step(current_angle, starting_angle, ending_angle, steps_list):
    angle_range = max(starting_angle, ending_angle) - min(starting_angle, ending_angle)
    idx = (current_angle // (angle_range // len(steps_list))) - 7
    if idx >= len(steps_list):
        idx = len(steps_list) - 1
    print(f"steps index for angle {current_angle} is {idx} with step value {steps_list[idx]}")
    return steps_list[idx]

angle = starting_angle
while angle < ending_angle:
    angle += get_step(angle, starting_angle, ending_angle, steps)
    print("Result angle: ", angle)

