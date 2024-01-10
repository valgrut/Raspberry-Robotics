import pygame
import sys
import math

# Initialize Pygame
pygame.init()

# Constants
WIDTH, HEIGHT = 800, 600
FOV = 1000
FPS = 60
clock = pygame.time.Clock()

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

# Initialize the screen
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("3D Lines")

# Function to rotate a point in 3D space
def rotate_point(x, y, z, angle_x, angle_y, angle_z):
    # Rotate around x-axis
    new_y = y * math.cos(angle_x) - z * math.sin(angle_x)
    new_z = y * math.sin(angle_x) + z * math.cos(angle_x)

    # Rotate around y-axis
    new_x = x * math.cos(angle_y) + new_z * math.sin(angle_y)
    new_z = -x * math.sin(angle_y) + new_z * math.cos(angle_y)

    # Rotate around z-axis
    new_x = new_x * math.cos(angle_z) - new_y * math.sin(angle_z)
    new_y = new_x * math.sin(angle_z) + new_y * math.cos(angle_z)

    return new_x, new_y, new_z

# Function to project 3D point to 2D screen
def project_point(x, y, z):
    scale = FOV / (FOV + z)
    screen_x = WIDTH // 2 + int(x * scale)
    screen_y = HEIGHT // 2 - int(y * scale)
    return screen_x, screen_y

# Function to draw a line in 3D space
def draw_3d_line(start, end, color):
    start_x, start_y, start_z = start
    end_x, end_y, end_z = end

    # Project the points to 2D screen
    start_screen = project_point(start_x, start_y, start_z)
    end_screen = project_point(end_x, end_y, end_z)

    # Draw the line
    pygame.draw.line(screen, color, start_screen, end_screen, 2)

# Main loop
def main():
    line1_length = 100
    line2_length = 80
    line3_length = 60
    angle1 = 0
    angle2 = 0
    angle3 = 0

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

        screen.fill(BLACK)

        # Define the three connected lines
        line1_start = (0, 0, 0)
        line1_end = (line1_length, 0, 0)

        line2_start = line1_end
        line2_end = (
            line1_end[0] + line2_length * math.cos(angle2),
            line2_length * math.sin(angle2),
            0,
        )

        line3_start = line2_end
        line3_end = (
            line2_end[0] + line3_length * math.cos(angle3),
            line2_end[1] + line3_length * math.sin(angle3),
            0,
        )

        # Draw the three connected lines
        draw_3d_line(line1_start, line1_end, WHITE)
        draw_3d_line(line2_start, line2_end, WHITE)
        draw_3d_line(line3_start, line3_end, WHITE)

        pygame.display.flip()
        clock.tick(FPS)

        # Update angles for rotation
        angle1 += 0.01
        angle2 += 0.02
        angle3 += 0.03

if __name__ == "__main__":
    main()

