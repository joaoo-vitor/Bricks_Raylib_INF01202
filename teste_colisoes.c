#include "raylib.h"
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

#define BALL_RADIUS 6
#define BLOCK_WIDTH 100
#define BLOCK_HEIGHT 50

typedef struct {
    Vector2 position;
    Vector2 speed;
} Ball;

int main() {
    // Inicialização da janela
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bola e Bloco");

    // Inicialização da posição da bola
    Ball ball;
    ball.position.x = SCREEN_WIDTH / 2;
    ball.position.y = SCREEN_HEIGHT - BALL_RADIUS;

    // Velocidade inicial da bola
    ball.speed.x = -0.5;
    ball.speed.y = -0.5; // Velocidade constante

    // Posição do bloco
    Vector2 blockPosition = {SCREEN_WIDTH / 2 - BLOCK_WIDTH / 2, SCREEN_HEIGHT / 2 - BLOCK_HEIGHT / 2};

    // Loop principal
    while (!WindowShouldClose()) {
        // Verifica se a tecla ESC foi pressionada para encerrar o programa
        if (IsKeyPressed(KEY_ESCAPE)) break;

        // Atualização da posição da bola
        ball.position.x += ball.speed.x;
        ball.position.y += ball.speed.y;

        // Verifica colisões com as bordas da tela
        if (ball.position.x >= SCREEN_WIDTH - BALL_RADIUS || ball.position.x <= BALL_RADIUS) {
            ball.speed.x *= -1; // Inverte a direção na colisão com as laterais da tela
        }
        if (ball.position.y <= BALL_RADIUS) {
            ball.speed.y *= -1; // Inverte a direção na colisão com a base da tela
        }

        // Verifica colisão com o bloco
        if (CheckCollisionCircleRec(ball.position, BALL_RADIUS, (Rectangle){blockPosition.x, blockPosition.y, BLOCK_WIDTH, BLOCK_HEIGHT})) {
            // Verifica se a bola bateu na base do bloco
            if (ball.position.y + BALL_RADIUS >= blockPosition.y && ball.position.y - BALL_RADIUS <= blockPosition.y + BLOCK_HEIGHT) {
                ball.speed.y *= -1; // Inverte a direção na colisão com a base do bloco
            }
            // Verifica se a bola bateu nas laterais do bloco
            else {
                ball.speed.x *= -1; // Inverte a direção na colisão com as laterais do bloco
            }
        }

        // Desenho
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Desenha o bloco
        DrawRectangle(blockPosition.x, blockPosition.y, BLOCK_WIDTH, BLOCK_HEIGHT, MAROON);

        // Desenha a bola
        DrawCircleV(ball.position, BALL_RADIUS, BLUE);

        EndDrawing();
    }

    // Encerramento
    CloseWindow();

    return 0;
}
