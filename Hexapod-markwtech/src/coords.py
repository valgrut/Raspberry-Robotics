from dataclasses import dataclass

# @dataclass
# class Coords:
#     x: int = 0.0
#     y: int = 0.0
#     z: int = 0.0

class Coords:
    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = x
        self.y = y
        self.z = z

    def __eq__(self, other): 
        if not isinstance(other, Coords):
            # don't attempt to compare against unrelated types
            return NotImplemented

        return self.x == other.x and self.y == other.y and self.z == other.z

    def __repr__(self):
        return "Coords()"
    
    def __str__(self):
        return f"({self.x}, {self.y}, {self.z})"

    def print(self):
        print(self.x, self.y, self.z)
        print()

    def list(self):
        return [self.x, self.y, self.z]
