from matplotlib import pyplot as plt

class Plotter:
    def __init__(self):
        self.points = []
        plt.rcParams["figure.figsize"] = [7.50, 3.50]
        plt.rcParams["figure.autolayout"] = True
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(projection="3d")
        self.ax.set_xlabel('x')
        self.ax.set_ylabel('y')
        self.ax.set_zlabel('z')

    def add_point(self, point):
        self.points.append(point)

    def empty_points(self):
        self.points = []

    def plot_point(self, point):
        x, y, z = [point[0]], [point[1]], [point[2]]
        self.ax.scatter(x, y, z, c='red', s=100)
        self.ax.plot(x, y, z, color='black')
        plt.show()

    def plot_list(self, points_list):
        x = [p[0] for p in points_list]
        y = [p[1] for p in points_list]
        z = [p[2] for p in points_list]
        self.ax.scatter(x, y, z, c='red', s=100)
        self.ax.plot(x, y, z, color='black')
        plt.show()

    def plot_points(self):
        x = [p[0] for p in self.points]
        y = [p[1] for p in self.points]
        z = [p[2] for p in self.points]
        self.ax.scatter(x, y, z, c='red', s=100)
        self.ax.plot(x, y, z, color='black')
        plt.show()

# plotter = Plotter()
# plotter.plot_point([1,3,2])
