#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LARGURA_TELA 1000
#define ALTURA_TELA 800
#define BASE_X LARGURA_TELA/2
#define BASE_Y ALTURA_TELA-100
#define ANGULO_INICIAL -90
#define VARIACAO_MAXIMA_ANGULO 80
#define ALCANCE_MIRA 500
#define PASSO_MIRA 1

#define RAIO_BOLA 10
#define VEL_BOLA 40

void desenhaMira(float angulo);
float atualizaDirecaoDoCanhaoTeclas(float anguloMira);
float atualizaDirecaoDoCanhaoMouse();
void moveBola(float *xBola, float *yBola, float vxBola, float vyBola);
void checaDisparoESetaVelocidade(float *vxBola, float *vyBola, float xBola, float yBola, float anguloDaMira);
void resetaBola(float *xBola, float *yBola, float *vxBola, float *vyBola);
int houveColisaoComBordas(float xBola, float yBola);

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING } GameScreen;

int main(void)
{

    float anguloDaMira = ANGULO_INICIAL;
    float xBola = BASE_X;
    float yBola = BASE_Y;
    float vxBola = 0;
    float vyBola = 0;

    //Inicializacoes
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Jogo");//Inicializa janela, com certo tamanho e titulo
    SetTargetFPS(100);// Ajusta a execucao do jogo para 60 frames por segundo
    GameScreen currentScreen = LOGO;
    int framesCounter = 0;          // Useful to count frames

    //--------------------------------------------------------------------------------------
    //Laco principal do jogo
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {


        // Trata entrada do usuario e atualiza estado do jogo
        //----------------------------------------------------------------------------------
        switch(currentScreen)
        {
        case LOGO:
        {
            // TODO: Update LOGO screen variables here!

            framesCounter++;    // Count frames

            // Wait for 2 seconds (120 frames) before jumping to TITLE screen
            if (framesCounter > 120)
            {
                currentScreen = TITLE;
            }
        }
        break;
        case TITLE:
        {
            // TODO: Update TITLE screen variables here!

            // Press enter to change to GAMEPLAY screen
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                currentScreen = GAMEPLAY;
            }
        }
        break;
        case GAMEPLAY:
        {
            // TODO: Update GAMEPLAY screen variables here!
            //Chama funcao para atualizar direcao do canhao atraves das teclas direcionais (do exercicio da aula passada)
            anguloDaMira = atualizaDirecaoDoCanhaoTeclas(anguloDaMira);

            //Chama funcao para checar disparo ao apertar KEY_SPACE e atualizar velocidades da bola
            checaDisparoESetaVelocidade(&vxBola, &vyBola, xBola, yBola, anguloDaMira);

            //Chama funcao que verifica se a bola colidiu com as bordas da tela
            //       Se sim, chama funcao para resetar posicao e velocidade da bola
            if(houveColisaoComBordas(xBola,yBola))
            {
                resetaBola(&xBola, &yBola, &vxBola, &vyBola);
            }

            //Chama funcao para atualizar posicao da bola dada velocidade atual
            moveBola(&xBola, &yBola, vxBola, vyBola);

            //Logs
            printf("Velocidade da bola (x,y): (%f, %f)\n Posicao bola = (%f, %f)\n Angulo (%f rad)\n\n", vxBola, vyBola, xBola, yBola, anguloDaMira);
            // Press enter to change to ENDING screen
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                currentScreen = ENDING;
            }
        }
        break;

        }



        //----------------------------------------------------------------------------------
        // Atualiza a representacao visual a partir do estado do jogo
        //----------------------------------------------------------------------------------
        BeginDrawing(); //Inicia o ambiente de desenho na tela
        ClearBackground(RAYWHITE);

        switch(currentScreen)
        {
        case LOGO:
        {
            // TODO: Draw LOGO screen here!
            DrawText("BRICKS", 20, 20, 40, LIGHTGRAY);

        }
        break;
        case TITLE:
        {
            // TODO: Draw TITLE screen here!
            DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, GREEN);
            DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
            DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);

        }
        break;
        case GAMEPLAY:
        {
            ClearBackground(RAYWHITE);//Limpa a tela e define cor de fundo

            // Desenha linha da base
            DrawLine(0,BASE_Y,LARGURA_TELA,BASE_Y,GRAY);
            // Desenha circulo na base
            DrawCircle(BASE_X,BASE_Y,10,GREEN);
            desenhaMira(anguloDaMira);
            // Desenha bola
            DrawCircle(xBola,yBola,RAIO_BOLA,BLUE);

        }
        break;
        case ENDING:
        {
            // TODO: Draw ENDING screen here!
            DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, BLUE);
            DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
            DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);
        }
        break;
        default:
            break;
        }
        EndDrawing(); //Finaliza o ambiente de desenho na tela
        //----------------------------------------------------------------------------------
    }
    CloseWindow();// Fecha a janela e o contexto OpenGL
    return 0;
}


//Fun��es auxili�res **********************************************/
void desenhaMira(float angulo)
{
    int d_x,d_y;
    float angulo_rad = DEG2RAD*angulo;
    d_x=cos(angulo_rad)*ALCANCE_MIRA;
    d_y=sin(angulo_rad)*ALCANCE_MIRA;
    DrawLine(BASE_X,BASE_Y,BASE_X+d_x, BASE_Y+d_y, RED);
    //printf("Angulo da mira: %f\n", angulo);

}

float atualizaDirecaoDoCanhaoTeclas(float anguloMira)
{
    if (IsKeyDown(KEY_RIGHT))
    {
        anguloMira+=PASSO_MIRA;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        anguloMira-=PASSO_MIRA;
    }
    if(anguloMira<-180) anguloMira=-180;
    if(anguloMira>0)anguloMira=0;


    return anguloMira;

}


float atualizaDirecaoDoCanhaoMouse(void)
{

    float anguloRad=-PI+(atan2(GetMouseY()-BASE_Y,GetMouseX()-BASE_X)/PI)*PI;
    return anguloRad;

}

void checaDisparoESetaVelocidade(float *vxBola, float *vyBola, float xBola, float yBola, float anguloDaMira)
{

    //Se for pressionado o espa�o e a bolinha estiver na base
    if(IsKeyPressed(KEY_SPACE) && (xBola == BASE_X && yBola == BASE_Y))
    {
        float angulo_rad = DEG2RAD*anguloDaMira;
        *vxBola=VEL_BOLA*cos(angulo_rad);
        *vyBola=VEL_BOLA*sin(angulo_rad);
        printf("Space foi pressionada!");
    }

}


void moveBola(float *xBola, float *yBola, float vxBola, float vyBola)
{
    *xBola=*xBola+vxBola;
    *yBola=*yBola+vyBola;

}

int houveColisaoComBordas(float xBola, float yBola)
{

    //se bater em uma das laterais
    if(xBola+RAIO_BOLA>=LARGURA_TELA||xBola-RAIO_BOLA<=0)
    {
        return 1;
    }
    //ou se bater no teto da tela
    else if(yBola-RAIO_BOLA<=0)
    {
        return 1;
    }
    //caso contr�rio (n�o bateu)
    else
    {

        return 0;
    }

}

void resetaBola(float *xBola, float *yBola, float *vxBola, float *vyBola)
{

    *xBola=BASE_X;
    *yBola=BASE_Y;
    *vxBola=0.0;
    *vyBola=0.0;

}



