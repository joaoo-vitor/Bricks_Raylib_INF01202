#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LARGURA_TELA 1000
#define ALTURA_TELA 800
#define BASE_X LARGURA_TELA / 2
#define BASE_Y ALTURA_TELA - 100
#define ANGULO_INICIAL -90
#define VARIACAO_MAXIMA_ANGULO 80
#define ALCANCE_MIRA 500
#define PASSO_MIRA 1

#define RAIO_BOLA 6
#define VEL_BOLA 40

struct Bola
{
    Vector2 posicao;
    Vector2 velocidade;
};

typedef enum GameScreen
{
    LOGO = 0,
    TITLE,
    GAMEPLAY,
    ENDING
} GameScreen;

void desenhaMira(float angulo);
float atualizaDirecaoDoCanhaoTeclas(float anguloMira);
float atualizaDirecaoDoCanhaoMouse(void);
void checaDisparoESetaVelocidade(struct Bola *bola, float anguloDaMira);
void moveBola(struct Bola *bola);
int houveColisaoComBordas(struct Bola *bola);
void resetaBola(struct Bola *bola);

int main(void)
{
    struct Bola bola;
    bola.posicao.x = BASE_X;
    bola.posicao.x = BASE_Y;
    bola.velocidade.x = 0;
    bola.velocidade.y = 0;

    float anguloDaMira = ANGULO_INICIAL;

    // Inicializacoes de Jogo
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Bricks"); // Inicializa janela, com certo tamanho e titulo
    SetTargetFPS(60);                                // Ajusta a execucao do jogo para 60 frames por segundo
    GameScreen currentScreen = LOGO;                 // Começa na etapa de logo do jogo
    int framesCounter = 0;                           // Usado para contar o tempo passado (em frames)

    // Carregar a textura do canhão do jogo
    Texture2D texture = LoadTexture("resources/canhao.png");
    const float posX_canhao = LARGURA_TELA/2.0f;
    const float posY_canhao = BASE_Y;

    //--------------------------------------------------------------------------------------
    // Laco principal do jogo
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //----------------------------------------------------------------------------------
        // Trata entrada do usuario e atualiza estado do jogo
        //----------------------------------------------------------------------------------
        switch (currentScreen)
        {
        case LOGO:

            // TODO: Update LOGO screen variables here!

            framesCounter++; // Count frames

            // Wait for 2 seconds (120 frames) before jumping to TITLE screen
            if (framesCounter > 120)
            {
                currentScreen = GAMEPLAY; // VAI DIRETO PRA GAMEPLAY (por enquanto)
            }

            break;
        case TITLE:

            // TODO: Update TITLE screen variables here!

            // Press enter to change to GAMEPLAY screen
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                currentScreen = GAMEPLAY;
            }

            break;
        case GAMEPLAY:

            // Chama funcao para atualizar direcao do canhao atraves das teclas direcionais (do exercicio da aula passada)
            anguloDaMira = atualizaDirecaoDoCanhaoTeclas(anguloDaMira);

            // Chama funcao para checar disparo ao apertar KEY_SPACE e atualizar velocidades da bola
            checaDisparoESetaVelocidade(&bola, anguloDaMira);

            // Chama funcao que verifica se a bola colidiu com as bordas da tela
            //        Se sim, chama funcao para resetar posicao e velocidade da bola
            if (houveColisaoComBordas(&bola))
            {
                resetaBola(&bola);
            }

            // Chama funcao para atualizar posicao da bola dada velocidade atual
            moveBola(&bola);

            // Logs
            printf("Velocidade da bola (x,y): (%f, %f)\n Posicao bola = (%f, %f)\n Angulo (%f rad)\n\n", bola.velocidade.x, bola.velocidade.y, bola.posicao.x, bola.posicao.y, anguloDaMira);
            // Press enter to change to ENDING screen
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                currentScreen = ENDING;
            }

            break;
        }

        //----------------------------------------------------------------------------------
        // Atualiza a representacao visual a partir do estado do jogo
        //----------------------------------------------------------------------------------
        BeginDrawing(); // Inicia o ambiente de desenho na tela
        ClearBackground(RAYWHITE);

        switch (currentScreen)
        {
        case LOGO:

            // TODO: Draw LOGO screen here!
            // Measure text width and height
            char text[] = "BRICKS";
            Font font = GetFontDefault();
            font.baseSize = 100;

            float textWidth = MeasureText(text, font.baseSize);
            float textHeight = font.baseSize;

            DrawText("BRICKS", (LARGURA_TELA - textWidth) / 2, (ALTURA_TELA - textHeight) / 2, font.baseSize, LIGHTGRAY);

            break;
        case TITLE:

            // TODO: Draw TITLE screen here!
            DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, GREEN);
            DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
            DrawText("[Pressione ENTER ou clique para prosseguir]", 120, 220, 20, DARKGREEN);

            break;
        case GAMEPLAY:

            // Desenha o chão
            DrawRectangle(0, BASE_Y, LARGURA_TELA, LARGURA_TELA - BASE_Y, GRAY);

            // Desenha a mira
            desenhaMira(anguloDaMira);

            // Desenha bola
            DrawCircle(bola.posicao.x, bola.posicao.y, RAIO_BOLA, BLUE);

            // Desenhar o canhao no centro do chão com redimensionamento e rotação
            DrawTexturePro(texture,
                           (Rectangle){0.0f, 0.0f, (float)texture.width, (float)texture.height},                                               // Source rectangle
                           (Rectangle){posX_canhao, posY_canhao, texture.width * 0.1f, texture.height * 0.1f}, // Destination rectangle
                           (Vector2){texture.width * 0.05f, texture.height * 0.05f},
                           anguloDaMira, WHITE);
            break;
        case ENDING:

            // TODO: Draw ENDING screen here!
            DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, BLUE);
            DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
            DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);

            break;
        default:
            break;
        }
        EndDrawing(); // Finaliza o ambiente de desenho na tela
        //----------------------------------------------------------------------------------
    }
    CloseWindow(); // Fecha a janela e o contexto OpenGL
    return 0;
}

// Funções auxiliares **********************************************/
void desenhaMira(float angulo)
{
    int d_x, d_y;
    float angulo_rad = DEG2RAD * angulo;
    d_x = cos(angulo_rad) * ALCANCE_MIRA;
    d_y = sin(angulo_rad) * ALCANCE_MIRA;
    DrawLine(BASE_X, BASE_Y, BASE_X + d_x, BASE_Y + d_y, RED);
}

float atualizaDirecaoDoCanhaoTeclas(float anguloMira)
{
    if (IsKeyDown(KEY_RIGHT))
    {
        anguloMira += PASSO_MIRA;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        anguloMira -= PASSO_MIRA;
    }
    if (anguloMira < -180)
        anguloMira = -180;
    if (anguloMira > 0)
        anguloMira = 0;

    return anguloMira;
}

/// TODO
float atualizaDirecaoDoCanhaoMouse(void)
{

    float anguloRad = -PI + (atan2(GetMouseY() - BASE_Y, GetMouseX() - BASE_X) / PI) * PI;
    return anguloRad;
}

void checaDisparoESetaVelocidade(struct Bola *bola, float anguloDaMira)
{

    // Se for pressionado o espaço e a bolinha estiver na base
    if (IsKeyPressed(KEY_SPACE) && (bola->posicao.x == BASE_X && bola->posicao.y == BASE_Y))
    {
        // Setta a velocidade da bola;
        float angulo_rad = DEG2RAD * anguloDaMira;
        bola->velocidade.x = VEL_BOLA * cos(angulo_rad);
        bola->velocidade.y = VEL_BOLA * sin(angulo_rad);
    }
}

void moveBola(struct Bola *bola)
{
    bola->posicao.x = bola->posicao.x + bola->velocidade.x;
    bola->posicao.y = bola->posicao.y + bola->velocidade.y;
}

int houveColisaoComBordas(struct Bola *bola)
{

    // se bater em uma das laterais
    if (bola->posicao.x + RAIO_BOLA >= LARGURA_TELA || bola->posicao.x - RAIO_BOLA <= 0)
    {
        return 1;
    }
    // ou se bater no teto da tela
    else if (bola->posicao.y - RAIO_BOLA <= 0)
    {
        return 1;
    }
    // caso contrário (não bateu)
    else
    {

        return 0;
    }
}

void resetaBola(struct Bola *bola)
{

    bola->posicao.x = BASE_X;
    bola->posicao.y = BASE_Y;
    bola->velocidade.x = 0.0;
    bola->velocidade.y = 0.0;
}
