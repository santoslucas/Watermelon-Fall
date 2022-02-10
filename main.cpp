/*
    Integrantes:    Lara Gama Santos (20213001497)
                    Lucas Santos Rodrigues (20213001521)
                    Marcela Caram (20213006528)
                    Vinícius Ferreira (20213005208)
*/

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "SDL/SDL_opengl.h"
#include "string"
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SOIL/SOIL.h>

#define num 38
#define qntd 1000

//carrega texturas
GLuint loadTexture(const char* arquivo) {
    GLuint idTextura = SOIL_load_OGL_texture(
                           arquivo,
                           SOIL_LOAD_AUTO,
                           SOIL_CREATE_NEW_ID,
                           SOIL_FLAG_INVERT_Y
                       );

    if (idTextura == 0) {
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());
    }

    return idTextura;
}

struct tiro{
    float x, y, ca, velY;
    bool vivo = true;
};

struct inimigo{
    float x, y, ca, velY,velX;
    bool vivo = true;
    tiro poder;
};

struct vida{
    float x,y,ca;
    bool vivo = true;
};

//funcao colisao
 bool colisao(float Ax,float Ay, float Acomp, float Aalt, float Bx, float By, float Bcomp, float Balt){
        //verifica se a parte de cima colide com a de baixo
        if(Ay+Aalt < By) return false;
        else if(Ay > By+Balt) return false;

        //verifica se as laterais colidem
        else if(Ax+Acomp < Bx) return false;
        else if(Ax > Bx+Bcomp) return false;
        return true;
    }

//inicia jogo
int iniciarJogo(){
    int inicia = 0;
    int menus = 0; //operacao no menu de iniciar
    int tiros = 0; //quantidade de tiros
    int abatidos = 0; //inimigos abatidos
    int dano = 4; //mede a vida
    int R = -1; //variavel random entre 0 e num
    bool parededireita = false; //para a melancia andar no eixo X

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Surface *tela;
    SDL_Surface *icone;

    //configuracoes audio
    int frequencia = 22050;
    Uint16 formato = AUDIO_S16SYS;
    int canais = 2;
    int buffer = 4096;

    Mix_OpenAudio(frequencia, formato, canais, buffer);

    //musica
    Mix_Music *som;

    //carrega musica
    som = Mix_LoadMUS("som.mp3");

    // controle de memoria
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // nome janela
    //Icone do jogo esta NULL
    SDL_WM_SetCaption("Watermelon Fall", NULL);

    // tamanho janela
    icone = IMG_Load("icon.png");
    tela = SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);

    Mix_PlayMusic(som, 1);

    SDL_WM_SetIcon(icone, NULL);

    // cor
    glClearColor(1,1,1,1);

    // area exibida
    glViewport(0,0,800,600);

    // sombra
    glShadeModel(GL_SMOOTH);

    // 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); //desenho geometrico

    // desabilita 3D
    glDisable(GL_DEPTH_TEST);

    // para uso da imagem
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool executando = true; // rodar jogo
    bool continuar = true; // utilizado para pausar o jogo
    bool atirar = false; // personagem Harry atirando
    SDL_Event eventos;
    SDL_Event pausar;
    SDL_Event menu;

    //personagem medidas
    float harryX = 960;
    float harryY = 880;
    float harryComp = 180;
    float harryAlt = 200;
    bool esq=false, dir=false; //movimentacao

    inimigo melancia[num];
    tiro poder[qntd]; //tiro do personagem Harry
    vida vida[4]; //vida Harry

    //gera inimigo (melancia)
   for(int i=0, espaco = 90, espacov=0; i<num; i++, espaco += 150)
   {
       //espacamento dos inimigos
        if(espaco > 1850){
            espaco = 0;
            espacov += 150;
        }
        melancia[i].x = espaco;
        melancia[i].y = espacov;
        melancia[i].ca = 70;
        melancia[i].velY = 0.005;
        melancia[i].velX = 0.02;
    }

    //gera a vida
    for(int i=0,comecox=1600; i<4; i++,comecox+=65){
        vida[i].x = comecox;
        vida[i].y = 20;
        vida[i].ca = 50;
    }

   //gera tiro
   for(int i=0; i<qntd; i++){
        poder[i].x = harryX+(harryComp/2);
        poder[i].y = harryY+(harryAlt/2);
        poder[i].ca = 10;
        poder[i].velY = 0.4;
    }

    //gera tiro inimigo
    for(int j=0; j<(num); j++){
        for(int i=0; i<1; i++){
            melancia[j].poder.x = melancia[j].x+(melancia[j].ca/2);
            melancia[j].poder.y = melancia[j].y+(melancia[j].ca/2);
            melancia[j].poder.ca = 10;
            melancia[j].poder.velY = 0.4;
            melancia[j].poder.vivo = false;
        }
    }

    //texturas
    unsigned int harry_texture = 0;
	harry_texture = loadTexture("harry.png");

	unsigned int melancia_texture = 0;
	melancia_texture = loadTexture("melancia.png");

	unsigned int fundo_texture = 0;
	fundo_texture = loadTexture("fundo.jpg");

	unsigned int vida_texture = 0;
	vida_texture = loadTexture("vida.png");

	unsigned int morte_texture = 0;
	morte_texture = loadTexture("morte.png");

	unsigned int explosao_texture = 0;
	explosao_texture = loadTexture("borboleta.png");

	unsigned int inicio_texture = 0;
	inicio_texture = loadTexture("inicio.png");

	unsigned int opcao_texture = 0;
	opcao_texture = loadTexture("opcao.png");

	unsigned int instrucao_texture = 0;
	instrucao_texture = loadTexture("instrucao.png");

    unsigned int creditos_texture = 0;
	creditos_texture = loadTexture("creditos.png");

    unsigned int ganhou_texture = 0;
	ganhou_texture = loadTexture("ganhou.png");

    unsigned int gameover_texture = 0;
	gameover_texture = loadTexture("gameover.png");

    //loop do jogo
    while(executando){

        //eventos
        while(SDL_PollEvent(&eventos)){

            //comecar o jogo
            if(eventos.type == SDL_KEYDOWN && eventos.key.keysym.sym == SDLK_c){
                menus=1;
            }

            //recomecar o jogo
            if(eventos.type == SDL_KEYDOWN && eventos.key.keysym.sym == SDLK_c && (num == abatidos || dano==0)){
                inicia = 1;
                executando = false;
            }

            //abrir opcoes
            if(eventos.type == SDL_KEYDOWN && eventos.key.keysym.sym == SDLK_o){
                menus=2;
            }

            //abrir instrucoes
            if(eventos.type == SDL_KEYDOWN && eventos.key.keysym.sym == SDLK_i){
                menus=3;
            }

            //abrir creditos
            if(eventos.type == SDL_KEYDOWN && eventos.key.keysym.sym == SDLK_x){
                menus=4;
            }

                //seta a dificuldade para facil
            if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_f && menus == 2){
                for(int i=0; i<num; i++){
                    melancia[i].velY = 0.005;
                    menus = 0;
                }
            }

                //seta a dificuldade para media
            if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_m && menus == 2){
                for(int i=0; i<num; i++){
                    melancia[i].velY = 0.01;
                    menus = 0;
                }
            }

                //seta a dificuldade para dificil
            if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_d && menus == 2){
                for(int i=0; i<num; i++){
                    melancia[i].velY = 0.02;
                    menus = 0;
                }
            }

                //fecha o jogo ao clicar no X da janela
            if(eventos.type == SDL_QUIT){
                executando = false;
            }
                //ESC (fecha o jogo)
            if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_ESCAPE && menus!=2 && menus!=3 && menus!=4){
                executando = false;
            }

                //voltar menu
            if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_ESCAPE && (menus==2 || menus==3 || menus==4)){
                menus =0;
            }

                //tecla R reinicia o jogo
           if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_r){
                inicia = 1;
                executando = false;
            }

                 //tecla P pausa/continua o jogo
            if(eventos.type == SDL_KEYDOWN && eventos.key.keysym.sym == SDLK_p){
                 while(continuar){
                    while(SDL_PollEvent(&pausar)){
                        if(pausar.type == SDL_KEYDOWN && eventos.key.keysym.sym == SDLK_p){
                            continuar=false;
                        }
                    }
                 }
                 continuar = true;
            }


            //Movimento personagem
            if(eventos.type == SDL_KEYDOWN){
                if(eventos.key.keysym.sym == SDLK_LEFT){
                    esq = true;
                }
                else if(eventos.key.keysym.sym == SDLK_RIGHT){
                    dir = true;
                }
            }
            else if(eventos.type == SDL_KEYUP){
                 if(eventos.key.keysym.sym == SDLK_LEFT){
                    esq = false;
                }
                else if(eventos.key.keysym.sym == SDLK_RIGHT){
                    dir = false;
                }
            }

            //atirar
            if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_SPACE){
                atirar = true;
                poder[tiros].x = harryX+(harryComp/2); //trava o X do tiro

                int cont=0; // contador de tiros disparados
                for(int i=0; i<num; i++){
                    if(melancia[i].poder.vivo == false){
                        cont++;
                    }
                }

                if(cont == (num-1)){
                    for(int i=0; i<num; i++){
                        melancia[i].poder.y = melancia[i].y+(melancia[i].ca/2);
                        melancia[i].poder.x = melancia[i].x+(melancia[i].ca/2);
                    }
                }

                /*
                    R recebe um random de 0 a num
                    Assim, a medida que o jogador dispara, uma melancia dispara
                */
                R = (rand()%num);
                if(R>=0 && R<=num){
                    melancia[R].poder.y = melancia[R].y+(melancia[R].ca/2);
                    melancia[R].poder.x = melancia[R].x+(melancia[R].ca/2);
                    melancia[R].poder.vivo = true;
                    //Renderiza o tiro
                    glClear(GL_COLOR_BUFFER_BIT);
                }
                tiros++; //tiros disparados
            }

        }

    //Carrega o menu iniciar
    if(menus == 0){
        // renderizacao
        glClear(GL_COLOR_BUFFER_BIT); //LIMPA O BUFFER

        //inicializa a matriz
        glPushMatrix();
        glOrtho(0, 1920, 1080, 0, -1, 1);

        //fundo
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, inicio_texture);
        glBegin(GL_QUADS);

            glTexCoord2d(1,1);   glVertex2f(0, 0);
            glTexCoord2d(0,1);   glVertex2f(1920, 0);
            glTexCoord2d(0,0);   glVertex2f(1920, 1080);
            glTexCoord2d(1,0);   glVertex2f(0, 1080);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // animacao
        SDL_GL_SwapBuffers();
    }

    // Comeca o jogo
    if(menus == 1){
        // logica

        //movimento

        //Personagem
        if(esq == true){
            harryX -= 0.4;
        }
        else if(dir == true){
            harryX += 0.4;
        }

        //nao sair da janela
        if(harryX < 0){
            harryX = 0;
        }
        else if(harryX+harryComp > 1920){
            harryX = 1920-harryComp;
        }

        //movimentacao tiro inimigo
        for(int i=0; i<num; i++){
                //dispara o tiro
            if(melancia[i].poder.vivo && melancia[i].vivo){
                melancia[i].poder.y += melancia[i].poder.velY;

                //colisao do harry com o tiro
                if((melancia[i].vivo)&&(melancia[i].poder.vivo)){

                        //colisao tiro inimigo e Harry
                    if(colisao(melancia[i].poder.x,melancia[i].poder.y,melancia[i].poder.ca,melancia[i].poder.ca,harryX,harryY,harryComp,harryAlt)==true){
                         melancia[i].poder.vivo = false;
                         vida[dano-1].vivo = false;
                         dano--;
                    }
                }

                //apaga o tiro quando ele sai da tela
                if((melancia[i].poder.vivo) && (melancia[i].poder.y>1080)){
                    melancia[i].poder.vivo = false;
                }
            }
        }

        //movimentacao tiro
        for(int i=0; i<tiros; i++){
            poder[i].y -= poder[i].velY;
                for(int j=0; j<num;j++){

                    //colisao do inimigo com o tiro
                    if((melancia[j].vivo == true)&&(poder[i].vivo == true)){
                        if(colisao(melancia[j].x,melancia[j].y,melancia[j].ca,melancia[j].ca,poder[i].x,poder[i].y,poder[i].ca,poder[i].ca)==true){
                             melancia[j].vivo = false;
                             melancia[j].poder.vivo = false;
                             melancia[j].velY = 0;
                             melancia[j].velX = 0;
                             poder[i].vivo = false;
                             abatidos++;
                    }
                }
            }
        }


        //Movimentacao melancia
        for(int i=0; i<num; i++){
            melancia[i].y += melancia[i].velY;

            //harry nao passa da teça
            if(parededireita==false){
                melancia[i].x -= melancia[i].velX;

                if(melancia[i].x<0){
                    parededireita =true;
                }
            }

            else if(parededireita==true){
                 melancia[i].x+=melancia[i].velX;
                 if(melancia[i].x+melancia[i].ca>1920){
                    parededireita =false;
                }
            }

            if(melancia[i].y < 0){
                melancia[i].velY = -melancia[i].velY;
            }

            else if(melancia[i].y + melancia[i].ca > 1080 && melancia[i].vivo){
                vida[dano-1].vivo=false;
                dano--;
                melancia[i].vivo=false;
                abatidos++;

            }

            //colisao inimigo com o Harry ou com o chão
            if(colisao(melancia[i].x,melancia[i].y,melancia[i].ca,melancia[i].ca,harryX,harryY,harryComp,harryAlt)== true && melancia[i].vivo){
                vida[dano-1].vivo=false;
                dano--;
                abatidos++;
                melancia[i].vivo = false;

                }
            }



        // *** RENDERIZACAO ***

        glClear(GL_COLOR_BUFFER_BIT); //LIMPA O BUFFER

        //inicializa a matriz
        glPushMatrix();
        glOrtho(0, 1920, 1080, 0, -1, 1);

        //fundo
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, fundo_texture);
        glBegin(GL_QUADS);

            glTexCoord2d(1,1);   glVertex2f(0, 0);
            glTexCoord2d(0,1);   glVertex2f(1920, 0);
            glTexCoord2d(0,0);   glVertex2f(1920, 1080);
            glTexCoord2d(1,0);   glVertex2f(0, 1080);

        glEnd();
        glDisable(GL_TEXTURE_2D);

        //personagem - Harry
        glColor4ub(255, 255, 255, 255);

        //harry vivo
        if(dano!=0){

        glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, harry_texture);

		glBegin(GL_QUADS);

            glTexCoord2d(0,0);  glVertex2f(harryX, harryY);
            glTexCoord2d(1,0);  glVertex2f(harryX+harryComp,harryY);
            glTexCoord2d(1,1);  glVertex2f(harryX+harryComp, harryY+harryAlt);
            glTexCoord2d(0,1);  glVertex2f(harryX, harryY+harryAlt);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        }

        else if(dano==0){
            //morte do harry
            glColor4ub(255, 255, 255, 255);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, explosao_texture);

            glBegin(GL_QUADS);

            glTexCoord2d(0,0);  glVertex2f(harryX, harryY);
            glTexCoord2d(1,0);  glVertex2f(harryX+harryComp,harryY);
            glTexCoord2d(1,1);  glVertex2f(harryX+harryComp, harryY+harryAlt);
            glTexCoord2d(0,1);  glVertex2f(harryX, harryY+harryAlt);

            glEnd();
            glDisable(GL_TEXTURE_2D);

        }

        //inimigo - melancia
        glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, melancia_texture);

        glBegin(GL_QUADS);
        glColor4ub(255, 255, 255, 255);
        for(int i=0; i<num; i++)
        {
            if(melancia[i].vivo == true){
                glTexCoord2d(0,0);  glVertex2f(melancia[i].x, melancia[i].y);
                glTexCoord2d(1,0);  glVertex2f(melancia[i].x+melancia[i].ca,melancia[i].y);
                glTexCoord2d(1,1);  glVertex2f(melancia[i].x+melancia[i].ca, melancia[i].y+melancia[i].ca);
                glTexCoord2d(0,1);  glVertex2f(melancia[i].x, melancia[i].y+melancia[i].ca);
            }
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);

        //tiros
        if(atirar == true){
            glBegin(GL_QUADS);
            glColor4ub(255, 20, 147, 255);
            for(int i=0; i<tiros; i++)
            {
                if(poder[i].vivo==true){
                    glVertex2f(poder[i].x, poder[i].y);
                    glVertex2f(poder[i].x+poder[i].ca,poder[i].y);
                    glVertex2f(poder[i].x+poder[i].ca,poder[i].y+poder[i].ca);
                    glVertex2f(poder[i].x, poder[i].y+poder[i].ca);
                }
            }
            glEnd();
        }

        //desenhar tiro inimigo
        glBegin(GL_QUADS);
        glColor4ub(0, 0, 0, 255);
        for(int i=0; i<tiros; i++){
            for(int j=0; j<1; j++){
                if(melancia[i].poder.vivo && melancia[i].vivo){
                    glVertex2f(melancia[i].poder.x, melancia[i].poder.y);
                    glVertex2f(melancia[i].poder.x+melancia[i].poder.ca, melancia[i].poder.y);
                    glVertex2f(melancia[i].poder.x+melancia[i].poder.ca, melancia[i].poder.y+melancia[i].poder.ca);
                    glVertex2f(melancia[i].poder.x, melancia[i].poder.y+melancia[i].poder.ca);
                }
            }
        }
        glEnd();

        //desenhar vida
        for(int i=0; (i<4); i++){
            if(vida[i].vivo){
                glColor4ub(255, 255, 255, 255);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, vida_texture);
                glBegin(GL_QUADS);

                    glTexCoord2d(1,1);   glVertex2f(vida[i].x, vida[i].y);
                    glTexCoord2d(0,1);   glVertex2f(vida[i].x+vida[i].ca,vida[i].y);
                    glTexCoord2d(0,0);   glVertex2f(vida[i].x+vida[i].ca, vida[i].y+vida[i].ca);
                    glTexCoord2d(1,0);   glVertex2f(vida[i].x, vida[i].y+vida[i].ca);
            }
            if(vida[i].vivo==false){
                glColor4ub(255, 255, 255, 255);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, morte_texture);
                glBegin(GL_QUADS);

                    glTexCoord2d(0,0);   glVertex2f(vida[i].x, vida[i].y);
                    glTexCoord2d(1,0);   glVertex2f(vida[i].x+vida[i].ca,vida[i].y);
                    glTexCoord2d(1,1);   glVertex2f(vida[i].x+vida[i].ca, vida[i].y+vida[i].ca);
                    glTexCoord2d(0,1);   glVertex2f(vida[i].x, vida[i].y+vida[i].ca);
             }
             glEnd();
             glDisable(GL_TEXTURE_2D);
        }

            glPopMatrix();

            // animacao
            SDL_GL_SwapBuffers();
        }

    //carrega opcoes
    if(menus == 2){
        // renderizacao
        glClear(GL_COLOR_BUFFER_BIT); //LIMPA O BUFFER

        //inicializa a matriz
        glPushMatrix();
        glOrtho(0, 1920, 1080, 0, -1, 1);

        //fundo
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, opcao_texture);
        glBegin(GL_QUADS);

            glTexCoord2d(1,1);   glVertex2f(0, 0);
            glTexCoord2d(0,1);   glVertex2f(1920, 0);
            glTexCoord2d(0,0);   glVertex2f(1920, 1080);
            glTexCoord2d(1,0);   glVertex2f(0, 1080);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // animacao
        SDL_GL_SwapBuffers();
    }

    //carrega instrucoes
    if(menus == 3){
        // renderizacao
        glClear(GL_COLOR_BUFFER_BIT); //LIMPA O BUFFER

        //inicializa a matriz
        glPushMatrix();
        glOrtho(0, 1920, 1080, 0, -1, 1);

        //fundo
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, instrucao_texture);
        glBegin(GL_QUADS);

            glTexCoord2d(1,1);   glVertex2f(0, 0);
            glTexCoord2d(0,1);   glVertex2f(1920, 0);
            glTexCoord2d(0,0);   glVertex2f(1920, 1080);
            glTexCoord2d(1,0);   glVertex2f(0, 1080);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // animacao
        SDL_GL_SwapBuffers();
    }

    //carrega creditos
    if(menus == 4){
        // renderizacao
        glClear(GL_COLOR_BUFFER_BIT); //LIMPA O BUFFER

        //inicializa a matriz
        glPushMatrix();
        glOrtho(0, 1920, 1080, 0, -1, 1);

        //fundo
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, creditos_texture);
        glBegin(GL_QUADS);

            glTexCoord2d(1,1);   glVertex2f(0, 0);
            glTexCoord2d(0,1);   glVertex2f(1920, 0);
            glTexCoord2d(0,0);   glVertex2f(1920, 1080);
            glTexCoord2d(1,0);   glVertex2f(0, 1080);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // animacao
        SDL_GL_SwapBuffers();
    }

    //tela game over
    if(dano == 0){
        // renderizacao
        glClear(GL_COLOR_BUFFER_BIT); //LIMPA O BUFFER

        //inicializa a matriz
        glPushMatrix();
        glOrtho(0, 1920, 1080, 0, -1, 1);

        //fundo
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, gameover_texture);
        glBegin(GL_QUADS);

            glTexCoord2d(1,1);   glVertex2f(0, 0);
            glTexCoord2d(0,1);   glVertex2f(1920, 0);
            glTexCoord2d(0,0);   glVertex2f(1920, 1080);
            glTexCoord2d(1,0);   glVertex2f(0, 1080);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // animacao
        SDL_GL_SwapBuffers();
        menus=-1;
    }

    //tela "voce ganhou"
     if(abatidos == num){
        // renderizacao
        glClear(GL_COLOR_BUFFER_BIT); //LIMPA O BUFFER

        //inicializa a matriz
        glPushMatrix();
        glOrtho(0, 1920, 1080, 0, -1, 1);

        //fundo
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ganhou_texture);
        glBegin(GL_QUADS);

            glTexCoord2d(1,1);   glVertex2f(0, 0);
            glTexCoord2d(0,1);   glVertex2f(1920, 0);
            glTexCoord2d(0,0);   glVertex2f(1920, 1080);
            glTexCoord2d(1,0);   glVertex2f(0, 1080);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // animacao
        SDL_GL_SwapBuffers();
        menus=-1;
    }

    }

    SDL_FreeSurface(icone); // icone do jogo
    Mix_FreeMusic(som); // libera a variavel da musica
    Mix_CloseAudio();// fecha o audio

    SDL_FreeSurface(tela);
    return inicia;
}

// funcao principal
int main(int argc, char** argv) {

    int inicia=0;

    //iniciar o jogo
    inicia =  iniciarJogo();

    //reinciar o jogo
    while(inicia==1){
        inicia = iniciarJogo();
    }

    printf("\nExecutando...\n");

    SDL_Delay(2000); //delay para fechar o jogo

    SDL_Quit();

   return 0;
}
