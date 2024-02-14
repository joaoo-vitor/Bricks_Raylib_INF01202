#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LARGURA_TELA 700
#define ALTURA_TELA 500
#define BASE_X LARGURA_TELA / 2
#define BASE_Y ALTURA_TELA - 100
#define ANGULO_INICIAL -90
#define VARIACAO_MAXIMA_ANGULO 80
#define ALCANCE_MIRA 500
#define PASSO_MIRA 1

#define RAIO_BOLA 6
#define VEL_BOLA RAIO_BOLA

#define BRICK_WIDTH 50
#define BRICK_HEIGHT 25
#define MAX_BRICKS 32
#define MATRIX_ROWS 4
#define MATRIX_COLS 8
#define PASSO_BRICKS 2

struct Bola
{
    Vector2 posicao;
    Vector2 velocidade;
};

typedef enum GameScreen
{
    LOGO = 0,
    MENU,
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
    //--------------------------------------------------------------------------------------
    // Inicializacoes de Jogo
    //--------------------------------------------------------------------------------------

    //Inicialização da bola
    struct Bola bola;
    bola.posicao.x = BASE_X;
    bola.posicao.y = BASE_Y;
    bola.velocidade.x = 0.0f;
    bola.velocidade.y = 0.0f;
    float anguloDaMira = ANGULO_INICIAL;
    int score = 0;

    //Inicialização variáveis para colisão
    int indexBloco=-1; //Conterá o index do bloco com que a bola colidiu
    //Usado para que a bola não colida com um bloco multiplas vezes quando na verdade deveria ser uma


    // Declara botões
    //Botões tela de início
    Rectangle botaoJogar = { LARGURA_TELA/2 - 100, ALTURA_TELA/2 - 50, 200, 40 };
    Rectangle botaoCarregar = {LARGURA_TELA/2 -100, ALTURA_TELA/2 + 20, 200, 40};
    Rectangle botaoSair = { LARGURA_TELA/2 - 100, ALTURA_TELA/2 + 90, 200, 40 };

    //Botões tela de fim (ficam à direita da tela)
    Rectangle botaoReset = { LARGURA_TELA/2 - 100 +300, ALTURA_TELA/2 - 50, 200, 40};
    Rectangle botaoTelaInicio= {LARGURA_TELA/2 -100 +300, ALTURA_TELA/2 + 20, 200, 40};
    Rectangle botaoSairFim = { LARGURA_TELA/2 - 100 +300, ALTURA_TELA/2 + 90, 200, 40 };

    //Inicialização bricks
    int bricksMatrix[MATRIX_ROWS][MATRIX_COLS];
    Brick bricks[MAX_BRICKS];

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Bricks"); // Inicializa janela, com certo tamanho e titulo
    SetTargetFPS(60);                                // Ajusta a execucao do jogo para 60 frames por segundo
    GameScreen currentScreen = LOGO;                 // Começa na etapa de logo do jogo
    int framesCounter = 0;                           // Usado para contar o tempo passado (em frames)


    // Carregar a textura do canhão do jogo
    Texture2D texture = LoadTexture("resources/canhao.png");
    const float posX_canhao = LARGURA_TELA/2.0f;
    const float posY_canhao = BASE_Y;

    // Carregar a textura do gif de fundo (tela inicio)
    int framesGifInicio=0;
    int currentAnimFrame=0;
    unsigned int nextFrameDataOffset=0;
    Image gifInicio = LoadImageAnim("resources/fundo_inicio.gif", &framesGifInicio);
    Texture2D texGifInicio =  LoadTextureFromImage(gifInicio);

    // Carregar a textura do gif de fundo (tela fim)
        int framesGifFim=0;
    Image gifFim = LoadImageAnim("resources/fundo_fim.gif", &framesGifFim);
    Texture2D texGifFim =  LoadTextureFromImage(gifFim);


    // Carrega matriz do arquivo de configuração
    FILE *file = fopen("config.txt", "r");
    if (!file)
    {
        printf("Erro: Não foi possível abrir arquivo de blocos 'config.txt'\n");
        CloseWindow();
        return -1;
    }

    printf("\n\nMatriz de blocos lida: \n");
    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        printf("\n |");
        for (int j = 0; j < MATRIX_COLS; j++)
        {
            if (fscanf(file, "%d", &bricksMatrix[i][j]) != 1)
            {
                printf("Erro: Formato do arquivo inválido! \n");
                fclose(file);
                CloseWindow();
                return -1;
            }
            printf(" %d ", bricksMatrix[i][j]);
        }
        printf("|");
    }
    printf("\n");

    // Inicializa bricks
    int brickIndex = 0;
    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        for (int j = 0; j < MATRIX_COLS; j++)
        {
            if (bricksMatrix[i][j] > 0)
            {
                bricks[brickIndex].rect = (Rectangle)
                {
                    j * (BRICK_WIDTH + 10) + 130, i * (BRICK_HEIGHT + 10) + 50, BRICK_WIDTH, BRICK_HEIGHT
                };
                bricks[brickIndex].lives = bricksMatrix[i][j];
                bricks[brickIndex].active = true;

                // Set color based on lives
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


            }
            else
            {
                bricks[brickIndex].active=false;
            }
            brickIndex++;
        }
    }

    //--------------------------------------------------------------------------------------
    // Laco principal do jogo
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //----------------------------------------------------------------------------------
        // Trata entrada do usuario e atualiza estado do jogo
        //----------------------------------------------------------------------------------
        switch (currentScreen)
        {
        case LOGO:
            framesCounter++; // Conta frames

            // A cada 3 frames passados, atualiza frame do gif de fundo
            if(framesCounter%3==0)
            {
                currentAnimFrame++;
                if (currentAnimFrame >= framesGifInicio) currentAnimFrame = 0;
                nextFrameDataOffset = texGifInicio.width*texGifInicio.height*4*currentAnimFrame;

                UpdateTexture(texGifInicio, ((unsigned char *)gifInicio.data) + nextFrameDataOffset);
            }

            // Passados 120 frames (2 segundos), vai para a próxima tela
            if (framesCounter > 120)
            {
                currentScreen++; //vai para próxima tela
            }
            break;
        case MENU:
            framesCounter++; // Conta frames

            // A cada 3 frames passados, atualiza frame do gif de fundo
            if(framesCounter%3==0)
            {
                currentAnimFrame++;
                if (currentAnimFrame >= framesGifInicio) currentAnimFrame = 0;
                nextFrameDataOffset = texGifInicio.width*texGifInicio.height*4*currentAnimFrame;

                UpdateTexture(texGifInicio, ((unsigned char *)gifInicio.data) + nextFrameDataOffset);
                framesCounter=0;

            }
            break;
        case GAMEPLAY:

            // Chama funcao para atualizar direcao do canhao atraves das teclas direcionais (do exercicio da aula passada)
            anguloDaMira = atualizaDirecaoDoCanhaoTeclas(anguloDaMira);

            // Chama funcao para checar disparo ao apertar KEY_SPACE e atualizar velocidades da bola
            checaDisparoESetaVelocidade(&bola, anguloDaMira);

            // Checa colisão com teto da janela
            if (bola.posicao.y - RAIO_BOLA < 0)
            {
                bola.velocidade.y *= -1;
                bola.posicao.y = RAIO_BOLA;
                indexBloco=-1; //Reseta comparador de blocos
            }

            // Checa colisão com lados da janela
            if (bola.posicao.x - RAIO_BOLA < 0 || bola.posicao.x + RAIO_BOLA > LARGURA_TELA)
            {
                bola.velocidade.x *= -1;
                indexBloco=-1; //Reseta comparador de blocos
            }

            // Verifica se a bola colidiu com o chão
            //        Se sim, chama funcao para resetar posicao e velocidade da bola
            if (houveColisaoComChao(bola))
            {
                resetaBola(&bola);
            }

            // Chama funcao para atualizar posicao da bola dada velocidade atual
            moveBola(&bola);

            // Move bricks closer
            for (int i = 0; i < MAX_BRICKS; i++)
            {
                if (bricks[i].active)
                {
                    bricks[i].rect.y += 0.1*PASSO_BRICKS;
                    //printf("Bloco %d (%d): (x,y) = (%f, %f). Width: %f, Height: %f\n", i, bricks[i].lives, bricks[i].rect.x,
                    //    bricks[i].rect.y, bricks[i].rect.width, bricks[i].rect.height);
                    // Check if the brick reaches or goes beyond the ground
                    if ((bricks[i].rect.y + bricks[i].rect.height) > BASE_Y)
                    {

                        printf("Fim de jogo, motivo: blocos chegaram na base.\n");
                        currentScreen = ENDING;
                    }
                }
            }

            // Checa colisão da bola com bricks
            for (int i = 0; i < MAX_BRICKS; i++)
            {
                if (bricks[i].active && CheckCollisionCircleRec(bola.posicao, RAIO_BOLA, bricks[i].rect))
                {
                    if(// O topo da bola bateu no limite inferior do bloco OU a base da bola bateu no limite superior do BLOCO
                        // E (ao mesmo tempo), o centro da bola estava dentro dos limites do comprimennto do bloco
                        // Assim, significa que bateu em uma das bases do bloco
                        ((bricks[i].rect.y<(bola.posicao.y+RAIO_BOLA))||((bola.posicao.y-RAIO_BOLA)<(bricks[i].rect.y+bricks[i].rect.height)))
                        && ((bricks[i].rect.x<bola.posicao.x)&&(bola.posicao.x<(bricks[i].rect.x+bricks[i].rect.width)))
                    )
                    {
                        bola.velocidade.y*= -1;
                        printf("Bateu as BASES do bricks\n");
                    }
                    else
                    {
                        //CASO CONTRÁRIO, significa que bateu em um dos lados do bloco
                        bola.velocidade.x*=-1;
                        printf("Bateu nos LADOS do brick\n");
                    }


                    // Atualiza vida do brick
                    bricks[i].lives--;
                    if (bricks[i].lives == 0)
                    {
                        bricks[i].active = false;
                        score++;
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

            // Press enter to change to ENDING screen
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                printf("Fim de jogo, motivo: Enter/Click pressionado.");
                currentScreen = ENDING;
            }

            break;
        case ENDING:
            framesCounter++; // Conta frames

            // A cada 3 frames passados, atualiza frame do gif de fundo
            if(framesCounter%3==0)
            {
                currentAnimFrame++;
                if (currentAnimFrame >= framesGifFim) currentAnimFrame = 0;
                nextFrameDataOffset = texGifInicio.width*texGifInicio.height*4*currentAnimFrame;

                UpdateTexture(texGifInicio, ((unsigned char *)gifInicio.data) + nextFrameDataOffset);
                framesCounter=0;

            }

            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                score=0;
                //reset blocks
                resetaBola(&bola);
                currentScreen = MENU;
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

            //Desenha fundo
            DrawTextureEx(texGifInicio, (Vector2)
            {
                -100,-100
                }, 0, 1.9, WHITE);

            // Desenha título centralizado
            desenhaTextoCentr("BRICKS", 100, (Vector2)
            {
                LARGURA_TELA/2, ALTURA_TELA/2
            });
            break;
        case MENU:
            //Desenha fundo
            DrawTextureEx(texGifInicio, (Vector2)
            {
                -100,-100
                }, 0, 1.9, WHITE);

            // Check for mouse input
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // Checa se o mouse está em cima do botão de JOGAR
                if (CheckCollisionPointRec(GetMousePosition(), botaoJogar))
                {
                    currentScreen++;
                    break;
                }
                // Checa se o mouse está em cima do botão de SAIR
                else if (CheckCollisionPointRec(GetMousePosition(), botaoSair))
                {
                    // Close the game
                    CloseWindow();
                    return 0;
                }
            }

            // Highlight buttons when mouse hovers over them
            if (CheckCollisionPointRec(GetMousePosition(), botaoJogar))
            {
                DrawRectangleRec(botaoJogar, BLUE);
            }
            else
            {
                DrawRectangleLinesEx(botaoJogar, 2, BLUE);
            }

            if (CheckCollisionPointRec(GetMousePosition(), botaoCarregar))
            {
                DrawRectangleRec(botaoCarregar, MAGENTA);
            }
            else
            {
                DrawRectangleLinesEx(botaoCarregar, 2, MAGENTA);
            }

            if (CheckCollisionPointRec(GetMousePosition(), botaoSair))
            {
                DrawRectangleRec(botaoSair, RED);
            }
            else
            {
                DrawRectangleLinesEx(botaoSair, 2, RED);
            }


            // Draw button texts
            desenhaTextoCentr("JOGAR", 20, (Vector2)
            {
                botaoJogar.x+botaoJogar.width/2,botaoJogar.y+botaoJogar.height/2
            });
            desenhaTextoCentr("CARREGAR JOGO", 20, (Vector2)
            {
                botaoCarregar.x+botaoCarregar.width/2,botaoCarregar.y+botaoCarregar.height/2
            });
            desenhaTextoCentr("SAIR", 20, (Vector2)
            {
                botaoSair.x+botaoSair.width/2,botaoSair.y+botaoSair.height/2
            });



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

            // Desenha bricks
            for (int i = 0; i < MAX_BRICKS; i++)
            {
                if (bricks[i].active)
                {
                    // Draw brick with its remaining lives in the center

                    // Desenha texto centralizado
                    DrawRectangleRec(bricks[i].rect, bricks[i].color);
                    desenhaTextoCentr(TextFormat("%d", bricks[i].lives), 18,
                                      (Vector2){
                                      bricks[i].rect.x+bricks[i].rect.width,
                                      bricks[i].rect.y+bricks[i].rect.height
                                      });
                }
            }

            // Desenha pontuação
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);

            break;
        case ENDING:

            //Desenha fundo
            DrawTextureEx(texGifFim, (Vector2)
            {
                -100,-100
                }, 0, 1.9, WHITE);
            DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, BLUE);
            DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);

            break;
        }
        EndDrawing(); // Finaliza o ambiente de desenho na tela
    }
    CloseWindow(); // Fecha a janela e o contexto OpenGL
    return 0;
}




//----------------------------------------------------------------------------------
//     ___FUNÇÕES____
//----------------------------------------------------------------------------------
void desenhaMira(float angulo)
{
    // Desenha reta com ângulo da mira
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
    // Checa limites do ângulo (trava mira)
    if (anguloMira < -175)
        anguloMira = -175;
    if (anguloMira > -5)
        anguloMira = -5;

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
    // Move bola na direção da velocidade
    bola->posicao.x = bola->posicao.x + bola->velocidade.x;
    bola->posicao.y = bola->posicao.y + bola->velocidade.y;
}


int houveColisaoComChao(struct Bola bola)
{

    // Se a bola bater no chão, retorna 1
    if ((bola.posicao.y + RAIO_BOLA > BASE_Y) && (bola.velocidade.y>0))
    {
        return 1;
    }
    else return 0;
}


void resetaBola(struct Bola *bola)
{
    // Zera vetor velocidade, setta posicao na base
    bola->posicao.x = BASE_X;
    bola->posicao.y = BASE_Y;
    bola->velocidade.x = 0.0;
    bola->velocidade.y = 0.0;
}

void desenhaTextoCentr(char *str, int size, Vector2 pos)
{
    Font font = GetFontDefault();
    font.baseSize = size;
    float textWidth = MeasureText(str, font.baseSize);
    float textHeight = font.baseSize;
    DrawText(str, pos.x - textWidth / 2, pos.y - textHeight / 2, font.baseSize, LIGHTGRAY);

}


