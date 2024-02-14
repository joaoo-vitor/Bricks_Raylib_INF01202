#include "raylib.h"

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Animated GIF Background Example");

    // Load GIF as texture
    Image gifImage = LoadImage("resources/fundo.gif");
    Texture2D background = LoadTextureFromImage(gifImage);
    UnloadImage(gifImage);

    // Get number of frames in the GIF
    int numFrames = background.width / background.height;
    int currentFrame = 0;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update

        // Move to the next frame
        currentFrame++;
        if (currentFrame >= numFrames)
        {
            currentFrame = 0; // Loop back to the beginning
        }

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw background with current frame
        Rectangle sourceRec = { currentFrame * background.height, 0, background.height, background.height };
        Rectangle destRec = { 0, 0, screenWidth, screenHeight };
        DrawTexturePro(background, sourceRec, destRec, (Vector2){ 0, 0 }, 0, WHITE);

        EndDrawing();
    }

    // Cleanup
    UnloadTexture(background);
    CloseWindow();

    return 0;
}

