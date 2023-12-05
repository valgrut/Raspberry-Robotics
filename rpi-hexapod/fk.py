import math

class Coord:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

def ik_dle_clanku(target: Coord):
    x0 = target.x
    y0 = target.y
    z0 = target.z

    L1 = 5
    L2 = 6.5
    L3 = 12

    theta1 = math.atan(y0 / x0)
    theta2 = math.acos((-L3**2 + L2**2 + x0**2 + y0**2 + z0**2) / (2*L2*math.sqrt(x0**2 + y0**2 + z0**2))) + math.atan2(z0, (math.sqrt(x0**2 + y0**2)))
    theta3 = -math.acos((x0**2 + y0**2 + z0**2 - L2**2 - L3**2) / (2*L2*L3))

    return (math.degrees(theta1), math.degrees(theta2), math.degrees(theta3))


# main
L1 = 5
L2 = 6.5
L3 = 12

a1 = 0
a2 = 20
a0 = 45

p0 = Coord(0,0,0)

p1 = Coord(p0.x + L1, 0, p0.z)
p2 = Coord(p1.x + L2 * math.cos(a1), 0, p1.z + L2 * math.sin(a1))
p3 = Coord(p2.x + L3 * math.cos(a1 + a2), 0, p2.z + L3 * math.sin(a1 + a2))
E3 = Coord(p3.x * math.cos(a0), p3.x * math.sin(a0), p3.z)

print(p0.x, p0.y, p0.z)
print(p1.x, p1.y, p1.z)
print(p2.x, p2.y, p2.z)
print(p3.x, p3.y, p3.z)
print(E3.x, E3.y, E3.z)

print("uhly", ik_dle_clanku(Coord(8, 0, 10)))
