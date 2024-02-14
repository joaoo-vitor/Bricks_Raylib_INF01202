#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

int main(void)
{
    // Initialize window and other required resources
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Menu Screen");

    // Define buttons
    Rectangle playButton = { SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 50, 200, 40 };
    Rectangle leftButton = { SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 20, 200, 40 };

    // Main loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update

        // Check for mouse input
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            // Check if mouse is over the "Play" button
            if (CheckCollisionPointRec(GetMousePosition(), playButton))
            {
                // Go to game screen
                CloseWindow(); // Close current window
                InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game"); // Open new window
                break;
            }
            // Check if mouse is over the "Left" button
            else if (CheckCollisionPointRec(GetMousePosition(), leftButton))
            {
                // Close the game
                CloseWindow();
                return 0;
            }
        }

        // Highlight buttons when mouse hovers over them
        if (CheckCollisionPointRec(GetMousePosition(), playButton))
        {
            DrawRectangleRec(playButton, BLUE);
        }
        else
        {
            DrawRectangleLinesEx(playButton, 2, BLUE);
        }

        if (CheckCollisionPointRec(GetMousePosition(), leftButton))
        {
            DrawRectangleRec(leftButton, RED);
        }
        else
        {
            DrawRectangleLinesEx(leftButton, 2, RED);
        }

        // Draw button texts
        DrawText("Play", playButton.x + 50, playButton.y + 10, 20, WHITE);
        DrawText("Left", leftButton.x + 50, leftButton.y + 10, 20, WHITE);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        EndDrawing();
    }

    return 0;
}
