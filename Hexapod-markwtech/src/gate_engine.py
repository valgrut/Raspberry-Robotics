from abc import ABC, abstractmethod 

class GateEngine:
    def __init__(self):
        # TODO: init gyroskop sensor
        self.gyroscope = None
        self.walk_mode = None
        pass

    def set_mode(self, mode):
        if mode == "RIPPLE_MODE":
            self.walk_mode = RippleMode()
        elif mode == "TRIPOD_MODE":
            self.walk_mode = TripodMode()
        elif mode == "WAVE_MODE":
            self.walk_mode = WaveMode()

    def walk(self):
        self.walk_mode.walk()


class MovementMode(ABC):
    @abstractmethod
    def walk(self):
        pass

    @abstractmethod
    def turn_left(self):
        pass

    @abstractmethod
    def turn_right(self):
        pass

    @abstractmethod
    def walk_backward(self):
        pass


class RippleMode(MovementMode):
    def walk(self):
        pass

class WaveMode(MovementMode):
    def walk(self):
        pass

class TripodMode(MovementMode):
    def walk(self):
        pass