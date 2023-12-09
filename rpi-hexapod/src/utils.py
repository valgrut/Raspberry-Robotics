import math

def map_range(v, a, b, c, d):
       return (v-a) / (b-a) * (d-c) + c

# a = map_range(4, 0, 10, 0, 1)