import math

dx = 500  # m
dy = 20  # m - height the target is on
v = 120  # m/s
g = 9.81  # m/s^2


A = ((pow(v,2)*(pow(v,2)-2*g*dy))/(pow(g,2)*pow(dx,2)))

if A <= 0:
    print("sqrt error: Nema reseni s danymi parametry")
    exit(1)

result = (pow(v, 2) / (g * dx)) - math.sqrt(A - 1)

print("theta: ", math.degrees(math.atan(result)))
