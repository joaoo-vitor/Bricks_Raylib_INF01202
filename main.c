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
#define VEL_BOLA 10

#define BRICK_WIDTH 80
#define BRICK_HEIGHT 30
#define MAX_BRICKS 48
#define MATRIX_ROWS 4
#define MATRIX_COLS 12
#define PASSO_BRICKS 1

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

typedef struct
{
    Rectangle rect;
    int lives;
    bool active;
    Color color;
} Brick;

void desenhaMira(float angulo);
float atualizaDirecaoDoCanhaoTeclas(float anguloMira);
float atualizaDirecaoDoCanhaoMouse(void);
void checaDisparoESetaVelocidade(struct Bola *bola, float anguloDaMira);
void moveBola(struct Bola *bola);
int houveColisaoComChao(struct Bola bola);
void resetaBola(struct Bola *bola);

int main(void)
{
    struct Bola bola;
    bola.posicao.x = BASE_X;
    bola.posicao.y = BASE_Y;
    bola.velocidade.x = 0.0f;
    bola.velocidade.y = 0.0f;

    float anguloDaMira = ANGULO_INICIAL;
    int score = 0;


    int bricksMatrix[MATRIX_ROWS][MATRIX_COLS];
    Brick bricks[MAX_BRICKS];

    // Inicializacoes de Jogo
    //**********************************************************************************************************
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Bricks"); // Inicializa janela, com certo tamanho e titulo
    SetTargetFPS(60);                                // Ajusta a execucao do jogo para 60 frames por segundo
    GameScreen currentScreen = LOGO;                 // Começa na etapa de logo do jogo
    int framesCounter = 0;                           // Usado para contar o tempo passado (em frames)

    // Load matrix from file
    FILE *file = fopen("config.txt", "r");
    if (!file)
    {
        printf("Error: Could not open file 'config.txt'\n");
        CloseWindow();
        return;
    }

    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        for (int j = 0; j < MATRIX_COLS; j++)
        {
            if (fscanf(file, "%d", &bricksMatrix[i][j]) != 1)
            {
                printf("Error: Invalid file format\n");
                fclose(file);
                CloseWindow();
                return;
            }
        }
    }

    // Initialize bricks
    int brickIndex = 0;
    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        for (int j = 0; j < MATRIX_COLS; j++)
        {
            if (bricksMatrix[i][j] > 0)
            {
                bricks[brickIndex].rect = (Rectangle)
                {
                    j * (BRICK_WIDTH + 10) + 50, i * (BRICK_HEIGHT + 10) + 50, BRICK_WIDTH, BRICK_HEIGHT
                };
                bricks[brickIndex].lives = bricksMatrix[i][j];
                bricks[brickIndex].active = true;

                // Set color based on remaining lives
                if (bricks[brickIndex].lives == 3)
                {
                    bricks[brickIndex].color = (Color)
                    {
                        237, 108, 43, 255
                    }; // #ed6c2b
                }
                else if (bricks[brickIndex].lives == 2)
                {
                    bricks[brickIndex].color = (Color)
                    {
                        252, 186, 3, 255
                    };  // #fcba03
                }
                else if (bricks[brickIndex].lives == 1)
                {
                    bricks[brickIndex].color = (Color)
                    {
                        252, 244, 3, 255
                    };  // #fcf403
                }

                brickIndex++;
            }
        }
    }
    // Carregar a textura do canhão do jogo
    Texture2D texture = LoadTexture("resources/canhao.png");
    const float posX_canhao = LARGURA_TELA/2.0f;
    const float posY_canhao = BASE_Y;
    //**********************************************************************************************************



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

            // Check collision with ceiling
            if (bola.posicao.y - RAIO_BOLA < 0)
            {
                bola.velocidade.y *= -1;
                bola.posicao.y = RAIO_BOLA;
            }

            // Check collision with sides
            if (bola.posicao.x - RAIO_BOLA < 0 || bola.posicao.x + RAIO_BOLA > LARGURA_TELA)
            {
                bola.velocidade.x *= -1;
            }

            // Chama funcao que verifica se a bola colidiu com o chão
            //        Se sim, chama funcao para resetar posicao e velocidade da bola
            if (houveColisaoComChao(bola))
            {
                resetaBola(&bola);
            }

            // Check collision with bricks
            for (int i = 0; i < MAX_BRICKS; i++)
            {
                if (bricks[i].active && CheckCollisionCircleRec(bola.posicao, RAIO_BOLA, bricks[i].rect))
                {

                    float recCenterX = bricks[i].rect.x + bricks[i].rect.width/2.0f;
                    float recCenterY = bricks[i].rect.y + bricks[i].rect.height/2.0f;
                    float dx = fabsf(bola.posicao.x - recCenterX);
                    float dy = fabsf(bola.posicao.y - recCenterY);

                    printf("Centro da bola:(%f,%f)\nCentro do tijolo:(%f,%f)\nDistancia:[%f, %f]\n\DistanciaX da base do tijolo:%f \n", bola.posicao.x, bola.posicao.y, recCenterX,
                           recCenterY, dx, dy, dx-(bricks[i].rect.width/2.0f));


                    bricks[i].lives--;
                    if (bricks[i].lives == 0)
                    {
                        bricks[i].active = false;
                        score++;
                    }
                    //check if the hit was on the sides
                    if(dx-bricks[i].rect.width/2.0f <= RAIO_BOLA)
                    {
                        bola.velocidade.x*= -1;
                        printf("BATEU NO LADO DO TIJOLO");
                    }
                    else
                    {
                        bola.velocidade.y*=-1;
                        printf("BATEU NA BASE DO TIJOLO");
                    }

                    // Change brick color based on remaining lives
                    if (bricks[i].lives == 2)
                    {
                        bricks[i].color = (Color)
                        {
                            252, 186, 3, 255
                        };  // #fcba03
                    }
                    else if (bricks[i].lives == 1)
                    {
                        bricks[i].color = (Color)
                        {
                            252, 244, 3, 255
                        };  // #fcf403
                    }
                }
            }

            // Chama funcao para atualizar posicao da bola dada velocidade atual
            moveBola(&bola);


            // Move bricks closer
            for (int i = 0; i < MAX_BRICKS; i++)
            {
                if (bricks[i].active)
                {
                    bricks[i].rect.y += 0.1*PASSO_BRICKS;

                    // Check if the brick reaches or goes beyond the ground
                    if (bricks[i].rect.y + bricks[i].rect.height > BASE_Y)
                    {
                        currentScreen = ENDING;
                    }
                }
            }

            // Logs
            // printf("Velocidade da bola (x,y): (%f, %f)\n Posicao bola = (%f, %f)\n Angulo (%f rad)\n\n", bola.velocidade.x, bola.velocidade.y, bola.posicao.x, bola.posicao.y, anguloDaMira);
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
                           (Rectangle)
            {
                0.0f, 0.0f, (float)texture.width, (float)texture.height
            },                                               // Source rectangle
            (Rectangle)
            {
                posX_canhao, posY_canhao, texture.width * 0.1f, texture.height * 0.1f
            }, // Destination rectangle
            (Vector2)
            {
                texture.width * 0.05f, texture.height * 0.05f
            },
            anguloDaMira, WHITE);


            // Draw bricks
            for (int i = 0; i < MAX_BRICKS; i++)
            {
                if (bricks[i].active)
                {
                    // Draw brick with its remaining lives in the center
                    DrawRectangleRec(bricks[i].rect, bricks[i].color);
                    DrawText(TextFormat("%d", bricks[i].lives), bricks[i].rect.x + bricks[i].rect.width / 2 - 10, bricks[i].rect.y + bricks[i].rect.height / 2 - 10, 20, BLACK);
                }
            }

            // Draw score
            //DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);

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


int houveColisaoComChao(struct Bola bola)
{

    // se bater no fundo
    if (bola.posicao.y + RAIO_BOLA > ALTURA_TELA)
    {
        return 1;
    }
    else return 0;
}


void resetaBola(struct Bola *bola)
{

    bola->posicao.x = BASE_X;
    bola->posicao.y = BASE_Y;
    bola->velocidade.x = 0.0;
    bola->velocidade.y = 0.0;
}
