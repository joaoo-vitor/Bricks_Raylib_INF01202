#include <raylib.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CANNON_RADIUS 20
#define BALL_RADIUS 10
#define BRICK_WIDTH 80
#define BRICK_HEIGHT 30
#define MAX_BRICKS 48
#define MATRIX_ROWS 4
#define MATRIX_COLS 12

#define DEG2RAD (3.14159265358979323846 / 180.0)  // Conversion factor from degrees to radians

typedef struct {
    Vector2 position;
    Vector2 speed;
    bool active;
    float angle;  // Angle of the cannon
} Ball;

typedef struct {
    Vector2 position;
} Cannon;

typedef struct {
    Rectangle rect;
    int lives;
    bool active;
    Color color;
} Brick;

// Function declarations
void InitGame();
void UpdateGame();
void DrawGame();
void UnloadGame();

// Global variables
Ball ball;
Cannon cannon;
Brick bricks[MAX_BRICKS];
int score = 0;

int bricksMatrix[MATRIX_ROWS][MATRIX_COLS];

int main(void) {
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bricks Game");
    SetTargetFPS(60);

    InitGame();

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        UpdateGame();

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawGame();
        EndDrawing();
    }

    // Unload resources
    UnloadGame();

    // Close window and OpenGL context
    CloseWindow();

    return 0;
}

void InitGame() {
    // Load matrix from file
    FILE *file = fopen("config.txt", "r");
    if (!file) {
        printf("Error: Could not open file 'config.txt'\n");
        CloseWindow();
        return;
    }

    for (int i = 0; i < MATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            if (fscanf(file, "%d", &bricksMatrix[i][j]) != 1) {
                printf("Error: Invalid file format\n");
                fclose(file);
                CloseWindow();
                return;
            }
        }
    }

    fclose(file);

    // Initialize ball
    ball.position = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50};
    ball.speed = (Vector2){0, 0};
    ball.active = false;
    ball.angle = 90.0;  // Initial angle of the cannon

    // Initialize cannon
    cannon.position = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT - CANNON_RADIUS};

    // Initialize bricks
    int brickIndex = 0;
    for (int i = 0; i < MATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            if (bricksMatrix[i][j] > 0) {
                bricks[brickIndex].rect = (Rectangle){j * (BRICK_WIDTH + 10) + 50, i * (BRICK_HEIGHT + 10) + 50, BRICK_WIDTH, BRICK_HEIGHT};
                bricks[brickIndex].lives = bricksMatrix[i][j];
                bricks[brickIndex].active = true;

                // Set color based on remaining lives
                if (bricks[brickIndex].lives == 3) {
                    bricks[brickIndex].color = (Color){237, 108, 43, 255}; // #ed6c2b
                } else if (bricks[brickIndex].lives == 2) {
                    bricks[brickIndex].color = (Color){252, 186, 3, 255};  // #fcba03
                } else if (bricks[brickIndex].lives == 1) {
                    bricks[brickIndex].color = (Color){252, 244, 3, 255};  // #fcf403
                }

                brickIndex++;
            }
        }
    }
}

void UpdateGame() {
    // Update ball
    if (ball.active) {
        ball.position.x += ball.speed.x;
        ball.position.y += ball.speed.y;

        // Check collision with bricks
        for (int i = 0; i < MAX_BRICKS; i++) {
            if (bricks[i].active && CheckCollisionCircleRec(ball.position, BALL_RADIUS, bricks[i].rect)) {
                bricks[i].lives--;
                if (bricks[i].lives == 0) {
                    bricks[i].active = false;
                    score++;
                }

                // Reflect only the y component of the ball's velocity upon collision
                ball.speed.y *= -1;

                // Change brick color based on remaining lives
                if (bricks[i].lives == 2) {
                    bricks[i].color = (Color){252, 186, 3, 255};  // #fcba03
                } else if (bricks[i].lives == 1) {
                    bricks[i].color = (Color){252, 244, 3, 255};  // #fcf403
                }
            }
        }

        // Check collision with bottom
        if (ball.position.y + BALL_RADIUS > SCREEN_HEIGHT) {
            ball.active = false;
            ball.position = (Vector2){cannon.position.x, cannon.position.y - CANNON_RADIUS - BALL_RADIUS};
        }

        // Check collision with ceiling
        if (ball.position.y - BALL_RADIUS < 0) {
            ball.speed.y *= -1;
            ball.position.y = BALL_RADIUS;
        }

        // Check collision with sides
        if (ball.position.x - BALL_RADIUS < 0 || ball.position.x + BALL_RADIUS > SCREEN_WIDTH) {
            ball.speed.x *= -1;
        }
    }

    // Move cannon angle
    if (IsKeyDown(KEY_LEFT)) {
        ball.angle -= 2.0;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        ball.angle += 2.0;
    }

    // Shoot ball
    if (IsKeyPressed(KEY_SPACE) && !ball.active) {
        ball.speed.x = 5 * cos(ball.angle * DEG2RAD);
        ball.speed.y = 5 * sin(ball.angle * DEG2RAD);
        ball.active = true;
    }
}




void DrawGame() {
    // Draw cannon
    DrawCircleV(cannon.position, CANNON_RADIUS, BLACK);

    // Draw aim line
    DrawLineEx(cannon.position,
               (Vector2){cannon.position.x + 50 * cos(ball.angle * DEG2RAD), cannon.position.y + 50 * sin(ball.angle * DEG2RAD)},
               2, RED);

    // Draw ball
    if (ball.active) {
        DrawCircleV(ball.position, BALL_RADIUS, BLACK);
    }

    // Draw bricks
    for (int i = 0; i < MAX_BRICKS; i++) {
        if (bricks[i].active) {
            // Draw brick with its remaining lives in the center
            DrawRectangleRec(bricks[i].rect, bricks[i].color);
            DrawText(TextFormat("%d", bricks[i].lives), bricks[i].rect.x + bricks[i].rect.width / 2 - 10, bricks[i].rect.y + bricks[i].rect.height / 2 - 10, 20, BLACK);
        }
    }

    // Draw score
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
}

void UnloadGame() {
    // Unload any game assets here
}
