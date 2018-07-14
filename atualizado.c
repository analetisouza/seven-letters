#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define Gravidade 6.0f //
#define STATUS_STATE_LIVES 0
#define STATUS_STATE_GAME 1
#define STATUS_STATE_GAMEOVER 2


typedef struct {
  float x, y;
  float dx, dy;
  short lives;
  int pontos, Chaves, Carta1;
  //char *name;
  int onPlat, morta;

  int animFrame, facingLeft, slowingDown;
} Alice;

typedef struct {
  float x, y;
} PLAT;

typedef struct {
  float x, y;
} INIMIGO;

typedef struct {
  float x, y;
} CHAVE;

typedef struct {
  float x, y;
} MOEDA;

typedef struct {
  float x, y;
} CARTA;

typedef struct { //jogo
  float scrollX; //rolagem

  Alice alice; //players
  PLAT plat[100];
  MOEDA moedas[100];
  CARTA cartas[7];
  CHAVE chaves;
  INIMIGO inim;

 
  SDL_Texture *aliceFrames[5]; //alterar pra não dar erro de seg
  SDL_Texture *inimFrames[4];
  SDL_Texture *plataforma;
  SDL_Texture *moeda;
  SDL_Texture *chave;
  SDL_Texture *carta;
  SDL_Texture *label;
  int labelW, labelH;

  TTF_Font *font, *font1;

  int time, mortes;
  int statusState;

  SDL_Renderer *renderer;
} GameState;


bool inicializador();

void saida();

SDL_Texture* loadTextura (const char *path); 

bool telainicial(SDL_Renderer*);

void telapause(SDL_Renderer*);

bool nivel1 (SDL_Renderer*);

void loadGame(GameState*);

void RenderNivel(SDL_Renderer*, GameState*);

void RenderObjetos(SDL_Renderer*, GameState*);

bool eventos(SDL_Window*, GameState*);

void processo(GameState*);

void colisao(GameState*);

int collide2d(int, int, int, int, int, int, int, int);

int distancia(int, int, int, int);

void shutdown_status_lives(GameState*); //tirar?
void shutdown_status_lives(GameState* game) {
  SDL_DestroyTexture(game->label);
  game->label = NULL;
}

SDL_Window* janela = NULL;
SDL_Renderer* renderer = NULL;


//Variáveis de controle
int cont = 0, reseta = 0, ncolisao = 0;

//Dimensões da janela
const int LARG = 1280; 
const int ALT = 720;

//Imagens



int main (int argc, char *argv[]) {
  GameState game;
  bool jogando = false; 
  int nivel;


  if ( !inicializador() ) {
    printf("Falha ao inicializar!\n");
    saida();
  }
  else {
    game.renderer = renderer;
    if ( !telainicial(renderer) ) {
      saida();
      IMG_Quit();
      SDL_Quit();
      exit(1);
    }

    jogando = true;
    //nivel = 1;
    nivel1(renderer);
   }

  saida();

  return 0;
}


bool inicializador () {
  bool sucesso = true;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) { //-1 dá problema
    printf("O programa não pode ser inicializado! SDL_ERROR: %s\n", SDL_GetError());
    sucesso = false;
  }
  else {
    //srandom((int)time(NULL)); para caso querer colocar algo surgindo aleatóriamente
    janela = SDL_CreateWindow("7 letters from my love", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARG, ALT, SDL_WINDOW_SHOWN);
    if (janela == NULL) {
      printf("A janela não pode ser criada! SDL_ERROR: %s\n", SDL_GetError());
      sucesso = false;
    }
    else {
      renderer = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (renderer == NULL) {
        printf("O renderer não pode ser criado! SDL Error: %s\n", SDL_GetError() );
        sucesso = false;
      }
      else {
      	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        if ( TTF_Init() ) {
        	printf( "SDL_ttf não pode inicializar! SDL_ttf Error: %s\n", TTF_GetError() );
        	sucesso = false;
        }
        int imgFlags = IMG_INIT_PNG;
        if ( !(IMG_Init(imgFlags) & imgFlags) ) {
          printf( "SDL_image não pode inicializar! SDL_image Error: %s\n", IMG_GetError() );
          sucesso = false;
        }
      }
    }
  }

  return sucesso;
}


void saida () {

  SDL_DestroyWindow(janela);
  SDL_DestroyRenderer(renderer);

  janela = NULL;
  renderer = NULL;

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}


SDL_Texture* loadTextura(const char *path) {
  SDL_Surface* loadedSurface = IMG_Load(path);
  SDL_Texture* textura = NULL;

  if(loadedSurface == NULL) {
    printf( "Não foi possível carregar a imagem %s! SDL Error: %s\n", path, IMG_GetError() );
  }
  else {
    textura = SDL_CreateTextureFromSurface (renderer, loadedSurface);
    if (textura == NULL) {
      printf( "Não foi possível carregar a textura %s! SDL Error: %s\n", path, IMG_GetError() );
    }
  }

  SDL_FreeSurface(loadedSurface);

  return textura;
}


bool telainicial (SDL_Renderer *renderer) {
  bool sucesso = true, apertou = false, gameloop = true;
  SDL_Event event;

  SDL_Texture* background = NULL;
  SDL_Texture* jogar = NULL;
  SDL_Texture* niveis = NULL;
  SDL_Texture* creditos = NULL;
  SDL_Texture* ranking = NULL;
  SDL_Texture *retornar = NULL;
  SDL_Texture* sair = NULL;
  SDL_Texture *ajuda = NULL;

  SDL_Texture *imgNiveis = NULL;
  imgNiveis = loadTextura("media/menu_niveis.png");

  SDL_Texture *imgCreditos = NULL;
  imgCreditos = loadTextura("media/creditos_teste.png"); 

  SDL_Texture *imgRanking = NULL;
  imgRanking = loadTextura("media/ranking.png"); 

  SDL_Texture *imgAjuda = NULL;
  imgAjuda = loadTextura("media/instrucoes.png"); 

  SDL_Texture *jogar2 = NULL;
  jogar2 = loadTextura("media/jogar3.png");
  SDL_Rect jogar2Rect = {378, 397, 124, 50};

  SDL_Texture *niveis2 = NULL;
  SDL_Rect niveis2Rect = {385, 490, 146, 53};
  niveis2 = loadTextura("media/niveis3.png");

  SDL_Texture *cred2 = NULL;
  SDL_Rect cred2Rect = {360, 587, 196, 57};
  cred2 = loadTextura("media/creditos3.png");

  SDL_Texture *ranking2 = NULL;
  SDL_Rect ran2Rect = {740, 392, 196, 57};
  ranking2 = loadTextura("media/ranking3.png");

  SDL_Texture *sair2 = NULL;
  SDL_Rect sair2Rect = {780, 490, 98, 41};
  sair2 = loadTextura("media/sair3.png");

  SDL_Texture *retornar2 = NULL;
  SDL_Rect ret2Rect = {30, 600, 100, 100};
  retornar2 = loadTextura("media/retornar2.png");


  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  background = loadTextura("media/menu.png");
  SDL_RenderCopy(renderer, background, NULL, NULL);

  jogar = loadTextura("media/jogar1.png");
  SDL_Rect jogarRect = {378, 397, 124, 50};
  SDL_RenderCopy(renderer, jogar, NULL, &jogarRect);

  niveis = loadTextura("media/niveis1.png");
  SDL_Rect niveisRect = {385, 490, 146, 53};
  SDL_RenderCopy(renderer, niveis, NULL, &niveisRect);

  creditos = loadTextura("media/creditos1.png");
  SDL_Rect credRect = {360, 587, 196, 57};
  SDL_RenderCopy(renderer, creditos, NULL, &credRect);

  ranking = loadTextura("media/ranking1.png");
  SDL_Rect ranRect = {740, 392, 196, 57};
  SDL_RenderCopy(renderer, ranking, NULL, &ranRect);

  sair = loadTextura("media/sair1.png");
  SDL_Rect sairRect = {780, 490, 98, 41};
  SDL_RenderCopy(renderer, sair, NULL, &sairRect);

  ajuda = loadTextura("media/interrogacao.png");
  SDL_Rect ajudaRect = {1150, 600, 100, 98};
  SDL_RenderCopy(renderer, ajuda, NULL, &ajudaRect);

  retornar = loadTextura("media/retornar1.png");
  SDL_Rect retRect = {30, 600, 100, 100};

  SDL_RenderPresent(renderer);

  SDL_Texture* jogar3 = NULL;
  jogar3 = loadTextura("media/jogar2.png");


  while (gameloop == true) {
    while ( SDL_PollEvent (&event) ) {
      if (event.type == SDL_QUIT) {
        gameloop = false;
        sucesso = false;
        saida();
        exit(1);
        break;
      }
      else {
      switch (event.type) {
        case SDL_MOUSEMOTION: //colocar som
        	//if (event.motion.x > 287 && event.motion.x < 600 && event.motion.y > 382 && event.motion.y < 460) {
          case SDL_MOUSEBUTTONDOWN:
          	if (event.button.button == SDL_BUTTON_LEFT) {
            	apertou = true;
          	}
            if (apertou == true && event.motion.x > 287 && event.motion.x < 600 && event.motion.y > 382 && event.motion.y < 460 && event.button.button == SDL_BUTTON_LEFT) {
                SDL_RenderCopy(renderer, jogar2, NULL, &jogar2Rect);
                SDL_RenderPresent(renderer);
                SDL_Delay(200);
                if(cont == 1) {
                  nivel1(renderer);
                }
                gameloop = false;
                nivel1(renderer);
                apertou = false;
            }
            else if (apertou == true && event.motion.x > 300 && event.motion.x < 610 && event.motion.y > 484 && event.motion.y < 546 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, niveis2, NULL, &niveis2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(200);
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgNiveis, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
              apertou = false;
            }

            else if (apertou == true && event.motion.x > 304 && event.motion.x < 613 && event.motion.y > 571 && event.motion.y < 660 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, cred2, NULL, &cred2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(200);
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgCreditos, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
              apertou = false;
            }

            else if (apertou == true && event.motion.x > 686 && event.motion.x < 1000 && event.motion.y > 380 && event.motion.y < 459 && event.button.button == SDL_BUTTON_LEFT) {
			  SDL_RenderCopy(renderer, ranking2, NULL, &ran2Rect);
			  SDL_RenderPresent(renderer);
              SDL_Delay(200); 
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgRanking, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
              apertou = false; 
            }

            else if (apertou == true && event.motion.x > 676 && event.motion.x < 992 && event.motion.y > 475 && event.motion.y < 537 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, sair2, NULL, &sair2Rect);
    		  SDL_RenderPresent(renderer);
              SDL_Delay(200);               
              gameloop = false;
              sucesso = false;
              apertou = false; 
              exit(1);
            }

            else if (apertou == true && event.motion.x > 1158 && event.motion.x < 1245 && event.motion.y > 611 && event.motion.y < 695 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_Delay(200);
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgAjuda, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
              apertou = false; 
            }

            else if(apertou == true && event.motion.x > 40 && event.motion.x < 120 && event.motion.y > 608 && event.motion.y < 694 && event.button.button == SDL_BUTTON_LEFT) { //botao retornar
              SDL_RenderCopy(renderer, retornar2, NULL, &ret2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(200); 
              telainicial(renderer);
              apertou = false; 
            }
            break;
        }
      }
    }
  }

    SDL_Delay(300);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(jogar);
    SDL_DestroyTexture(niveis);
    SDL_DestroyTexture(ranking);
    SDL_DestroyTexture(creditos);
    SDL_DestroyTexture(sair);
    SDL_DestroyTexture(ajuda);
    SDL_DestroyTexture(imgNiveis);
    SDL_DestroyTexture(imgCreditos);
    SDL_DestroyTexture(imgRanking);
    SDL_DestroyTexture(imgAjuda);
    SDL_DestroyTexture(jogar2);
	SDL_DestroyTexture(cred2);
	SDL_DestroyTexture(ranking2);
	SDL_DestroyTexture(niveis2);
	SDL_DestroyTexture(sair2);
	SDL_DestroyTexture(retornar2);
    SDL_DestroyTexture(retornar);
    SDL_RenderClear(renderer);

  return sucesso; //tentar arrumar o erro do mouse apertar pressionando

}


void telapause(SDL_Renderer *renderer) { //falta a opção de opções de jogo
  bool gameloop = true;
  SDL_Event event;
  SDL_Texture *telaPause = NULL;
  //SDL_Texture *menuPause = NULL;

  SDL_Texture *menu2 = NULL;
  SDL_Rect menu2Rect = {356, 365, 175, 200};
  menu2 = loadTextura("media/menu3.png");

  SDL_Texture *cont2 = NULL;
  SDL_Rect cont2Rect = {552, 364, 175, 200};
  cont2 = loadTextura("media/continuar3.png");

  SDL_Texture *pause2 = NULL;
  SDL_Rect pause2Rect = {746, 364, 175, 200};
  pause2 = loadTextura("media/opcoes_pausa3.png");


  telaPause = loadTextura("media/menu_pausa.png");
  SDL_RenderCopy(renderer, telaPause, NULL, NULL);
  SDL_RenderPresent(renderer);

  while (gameloop == true) {
  while(SDL_PollEvent(&event)) {
  	if (event.type == SDL_QUIT) {
  		gameloop = false;
  		saida();
        exit(1);
    }
    else switch (event.type) {
        case SDL_MOUSEMOTION:
          case SDL_MOUSEBUTTONDOWN:
            if (event.motion.x > 372 && event.motion.x < 520 && event.motion.y > 372 && event.motion.y < 552 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, menu2, NULL, &menu2Rect);
    		  SDL_RenderPresent(renderer);
              cont = 1;
              SDL_Delay(800);
              telainicial(renderer);
              gameloop = false;
            }

            if (event.motion.x > 565 && event.motion.x < 714 && event.motion.y > 372 && event.motion.y < 552 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, cont2, NULL, &cont2Rect);
    	      SDL_RenderPresent(renderer);
              SDL_Delay(500);
              gameloop = false;
            }

            if (event.motion.x > 780 && event.motion.x < 889 && event.motion.y > 372 && event.motion.y < 552 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, pause2, NULL, &pause2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(500);
              gameloop = false;
            }
          break;
       }
    }
  }

  SDL_DestroyTexture(telaPause);
  SDL_DestroyTexture(menu2);
  SDL_DestroyTexture(cont2);
  SDL_DestroyTexture(pause2);

}


void telafim (SDL_Renderer *renderer, GameState *game) {
  int num = 0;
  bool gameloop = true;
  SDL_Event event;
  SDL_Texture *telaFim = NULL;
  SDL_Texture *jogardnv = NULL;
  SDL_Texture *Menu = NULL;

  SDL_Texture *jogarnvm2 = NULL;
  SDL_Rect jogarnv2Rect = {80, 300, 175, 200};
  jogarnvm2 = loadTextura("media/novamente3.png");

  SDL_Texture *menu3 = NULL;
  SDL_Rect menu3Rect = {80, 500, 175, 200};
  menu3 = loadTextura("media/menu3.png");

  telaFim = loadTextura("media/fim_jogo.png");
  SDL_RenderCopy(renderer, telaFim, NULL, NULL);

  jogardnv = loadTextura("media/novamente1.png");
  SDL_Rect jogardnvRect = {80, 300, 175, 200};
  SDL_RenderCopy(renderer, jogardnv, NULL, &jogardnvRect);

  Menu = loadTextura("media/menu1.png");
  SDL_Rect menuuRect = {80, 500, 175, 200};
  SDL_RenderCopy(renderer, Menu, NULL, &menuuRect);

  SDL_RenderPresent(renderer);

  while (gameloop == true) {
  while(SDL_PollEvent(&event)) {
    //printf ("%d\n", event.motion.y);
    switch (event.type) {
        case SDL_MOUSEMOTION:
          case SDL_MOUSEBUTTONDOWN:
            if (event.motion.x > 90 && event.motion.x < 245 && event.motion.y > 307 && event.motion.y < 485 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, jogarnvm2, NULL, &jogarnv2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(300);
              game->alice.Chaves = 0;
              nivel1(renderer);
              gameloop = false;
            } 
            if (event.motion.x > 92 && event.motion.x < 243 && event.motion.y > 503 && event.motion.y < 686 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, menu3, NULL, &menu3Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(300);
              cont = 1;
              game->alice.Chaves = 0;
              gameloop = false;
              telainicial(renderer);
            }
          break;
       }
      if (event.type == SDL_QUIT) {
        saida();
        exit(1);
      }
    }
  }

  SDL_DestroyTexture(jogarnvm2);
  SDL_DestroyTexture(menu3);
  SDL_DestroyTexture(telaFim);
  SDL_DestroyTexture(jogardnv);
  SDL_DestroyTexture(Menu);

}

bool nivel1(SDL_Renderer *renderer) {
  bool sucesso = true;
  bool jogando = true;
  GameState game;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);  //tempo para carregar o jogo ou colocar uma imagem explicando o jogo
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  SDL_Delay(500);

  loadGame(&game);
  game.font = TTF_OpenFont("media/TravelingTypewriter.ttf", 30);
  game.font1 = TTF_OpenFont("media/TravelingTypewriter.ttf", 20);
  game.plataforma = loadTextura("media/plataforma.png");
  game.moeda = loadTextura("media/moeda.png");
  game.chave = loadTextura("media/chave.png");
  game.carta = loadTextura("media/carta.png");
  game.aliceFrames[0] = loadTextura("media/alice1.png");
  game.aliceFrames[1] = loadTextura("media/alice2.png");
  game.inimFrames[0] = loadTextura("media/slimeWalk1.png");
  game.inimFrames[1] = loadTextura("media/slimeWalk2.png");

    while(jogando != false) {
      SDL_RenderClear(renderer);
      RenderNivel(renderer, &game);
      RenderObjetos(renderer, &game);
      jogando = eventos(janela, &game);
      processo(&game);
      colisao(&game);
      SDL_RenderPresent(renderer);
      SDL_Delay(1/60);

    }

    SDL_Delay(500);

  SDL_DestroyTexture(game.plataforma);
  SDL_DestroyTexture(game.moeda);
  SDL_DestroyTexture(game.chave);
  SDL_DestroyTexture(game.carta);
  SDL_DestroyTexture(game.aliceFrames[0]);
  SDL_DestroyTexture(game.aliceFrames[1]); 
  SDL_DestroyTexture(game.inimFrames[0]);
  SDL_DestroyTexture(game.inimFrames[1]); 
  TTF_CloseFont(game.font);
  TTF_CloseFont(game.font1);


    return sucesso;
    
}

void loadGame(GameState *game) { //posição dos elementos do mapa que podem ser mudados
  int i;
  //posição da alice;
  game->label = NULL;

  game->alice.x = 30;
  game->alice.y = 500;
  game->alice.dx = 0;
  game->alice.dy = 0;
  game->alice.onPlat = 0;
  game->alice.animFrame = 0;
  game->alice.facingLeft = 0; //0 pq o boneco tá virado pra direita
  game->alice.slowingDown = 0;
  game->alice.lives = 3;
  game->alice.pontos = 0;
  game->statusState = STATUS_STATE_LIVES;

  game->time = 0;
  game->scrollX = 0;


  //posição das plataformas 
  for (i = 0; i < 20; i++) { //do chão
    game->plat[i].x = 0+(128*i);
    game->plat[i].y = 675;
  }

  game->plat[20].x = 0;
  game->plat[20].y = 500;

  game->plat[21].x = 128;
  game->plat[21].y = 500;

  game->plat[22].x = 256;
  game->plat[22].y = 500;

  game->plat[23].x = 470;
  game->plat[23].y = 600;

  game->plat[24].x = 520;
  game->plat[24].y = 450;

  game->plat[25].x = 648;
  game->plat[25].y = 450;

  game->plat[26].x = 620;
  game->plat[26].y = 520;

  game->plat[27].x = 748;
  game->plat[27].y = 520;

  game->plat[28].x = 876;
  game->plat[28].y = 520;

  // Plataformas saída

  game->plat[29].x = 2304;
  game->plat[29].y = 200;

  game->plat[30].x = 2432;
  game->plat[30].y = 200;

  //-----------------------

  game->plat[31].x = 256;
  game->plat[31].y = 300;

  game->plat[32].x = 384;
  game->plat[32].y = 300;

  game->plat[33].x = 512;
  game->plat[33].y = 300;

  game->plat[34].x = 0;
  game->plat[34].y = 400;

  game->plat[35].x = 128;
  game->plat[35].y = 400;

  game->plat[36].x = 648;
  game->plat[36].y = 250;

  game->plat[37].x = 776;
  game->plat[37].y = 250;

  game->plat[38].x = 1032;
  game->plat[38].y = 450;

  game->plat[39].x = 1160;
  game->plat[39].y = 450;

  game->plat[40].x = 1288;
  game->plat[40].y = 450;

  game->plat[41].x = 1416;
  game->plat[41].y = 450;

  game->plat[42].x = 950;
  game->plat[42].y = 220;

  game->plat[43].x = 1078;
  game->plat[43].y = 220;

  game->plat[44].x = 1226;
  game->plat[44].y = 290;

  game->plat[45].x = 1566;
  game->plat[45].y = 350;

  game->plat[46].x = 1694;
  game->plat[46].y = 350;

  game->plat[47].x = 1822;
  game->plat[47].y = 350;

  game->plat[48].x = 1744;
  game->plat[48].y = 220;

  game->plat[49].x = 1872;
  game->plat[49].y = 220;

  game->plat[50].x = 2000;
  game->plat[50].y = 220;

  //MOEDINHAS
  game->moedas[0].x = 540;
  game->moedas[0].y = 560;

  game->moedas[1].x = 140;
  game->moedas[1].y = 300;

  game->moedas[2].x = 180;
  game->moedas[2].y = 300;

  game->moedas[3].x = 430;
  game->moedas[3].y = 230;

  game->moedas[4].x = 640;
  game->moedas[4].y = 370;

  game->moedas[5].x = 600;
  game->moedas[5].y = 370;

  game->moedas[6].x = 680;
  game->moedas[6].y = 370;

  game->moedas[7].x = 920;
  game->moedas[7].y = 450;

  game->moedas[8].x = 880;
  game->moedas[8].y = 450;

  game->moedas[9].x = 840;
  game->moedas[9].y = 450;

  game->moedas[10].x = 1120;
  game->moedas[10].y = 370;

  game->moedas[11].x = 1160;
  game->moedas[11].y = 370;

  game->moedas[12].x = 1200;
  game->moedas[12].y = 370;

  game->moedas[13].x = 1240;
  game->moedas[13].y = 370;

  game->moedas[14].x = 1280;
  game->moedas[14].y = 370;

  game->moedas[15].x = 1320;
  game->moedas[15].y = 370;

  game->moedas[16].x = 320;
  game->moedas[16].y = 450;

  game->moedas[17].x = 320;
  game->moedas[17].y = 450;

  game->moedas[18].x = 1600;
  game->moedas[18].y = 270;

  game->moedas[19].x = 1560;
  game->moedas[19].y = 270;

  game->moedas[20].x = 1640;
  game->moedas[20].y = 270;

  game->moedas[21].x = 1280;
  game->moedas[21].y = 230;

  game->moedas[22].x = 390;
  game->moedas[22].y = 230;

  game->moedas[23].x = 470;
  game->moedas[23].y = 230;

  game->moedas[24].x = 730;
  game->moedas[24].y = 170;

  game->moedas[25].x = 770;
  game->moedas[25].y = 170;

  game->chaves.x = 14;
  game->chaves.y = 355; //350

  game->inim.x = 2515;
  game->inim.y = 644;

  game->cartas[0].x = 860;
  game->cartas[0].y = 485; //480

}

void RenderNivel(SDL_Renderer *renderer, GameState *game) {
  SDL_Color preto = {0, 0, 0, 0};
  SDL_Texture *porta = NULL;
  SDL_Texture *fundo = NULL;
  SDL_Texture *barra = NULL;
  SDL_Texture *vida = NULL;
  SDL_Texture *placa = NULL;
  SDL_Texture *pause = NULL;
  SDL_Texture *Nivel1 = NULL;
  SDL_Texture *chavinha = NULL;
  SDL_Texture *cartinha = NULL;
  SDL_Texture *fim = NULL;


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    fundo = loadTextura("media/mapa.png");
    SDL_Rect fundoRect = {game->scrollX + 0, 0, 2560+155, 720};
    SDL_RenderCopy(renderer, fundo, NULL, &fundoRect);

    porta = loadTextura("media/porta.png");
    SDL_Rect portRect = { game->scrollX + 2432, 67, 113, 133};
    SDL_RenderCopy(renderer, porta, NULL, &portRect);

    barra = loadTextura("media/base_barra.png");
    SDL_Rect barraRect = {0, 0, 1280, 109};
    SDL_RenderCopy(renderer, barra, NULL, &barraRect);

    //caso queira inserir algum texto
    char str[50] = "";
    sprintf (str, "%d", (int)game->alice.pontos);
    SDL_Surface *tmp = TTF_RenderText_Blended(game->font, str, preto); 
    game->labelW = tmp->w;
    game->labelH = tmp->h;
    game->label = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect textRect = {960, 60-game->labelH, game->labelW, game->labelH}; //960
    SDL_RenderCopy(renderer, game->label, NULL, &textRect); 

    Nivel1 = loadTextura("media/nivel1.png");
    SDL_Rect nivel1Rect = {560, 35, 153, 41};
    SDL_RenderCopy(renderer, Nivel1, NULL, &nivel1Rect);

    //if (alice.vida == 1)
    vida = loadTextura("media/coracao_vida.png");
    SDL_Rect vidaRect = {180, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vidaRect);

    cartinha = loadTextura("media/carta.png");
    SDL_Rect cartinhaRect = {860, 10, 40, 30};
    SDL_RenderCopy(renderer, cartinha, NULL, &cartinhaRect);

    if (game->alice.Carta1 >= 0) {
    	char str[10] = "";
    	sprintf (str, "x%d", (int)game->alice.Carta1);
    	SDL_Surface *tmp = TTF_RenderText_Blended(game->font1, str, preto); //colcar menor
    	game->labelW = tmp->w;
    	game->labelH = tmp->h;
    	game->label = SDL_CreateTextureFromSurface(renderer, tmp);
    	SDL_FreeSurface(tmp);
    	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    	SDL_Rect textRect = {900, 45-game->labelH, game->labelW, game->labelH}; 
    	SDL_RenderCopy(renderer, game->label, NULL, &textRect); 
    }

    if (game->alice.lives == 3) {
      SDL_Rect vida2Rect = {245, 10, 49, 39};
      SDL_RenderCopy(renderer, vida, NULL, &vida2Rect);
      SDL_Rect vida3Rect = {310, 10, 49, 39};
      SDL_RenderCopy(renderer, vida, NULL, &vida3Rect);
    }

    if (game->alice.lives == 2) {
      SDL_Rect vida2Rect = {245, 10, 49, 39};
      SDL_RenderCopy(renderer, vida, NULL, &vida2Rect);
    }

    if (game->alice.lives == 0) {
    	game->alice.Chaves = 0;
    	game->alice.Carta1 = 0;
    	telafim(renderer,game);
    	cont = 1;
    	reseta = 1;
    }

    chavinha = loadTextura("media/keyYellow.png");
    SDL_Rect chavRect = {370, 7, 50, 50};


    //printf("%f\n", game->alice.y);
    if (game->alice.Chaves == 1 && game->alice.Carta1 == 1) {
      SDL_RenderCopy(renderer, chavinha, NULL, &chavRect);
      if (game->alice.x > 2451 && game->alice.x < 2454 && game->alice.y > 87 && game->alice.y < 90) {
        game->alice.Chaves = 0;
        game->alice.Carta1 = 0;
        telafim(renderer, game);
    	cont = 1;
    	reseta = 1;
      }
    }

    placa = loadTextura("media/signRight.png");
    SDL_Rect placRect = { game->scrollX + 120, 570, 70, 100 };
    SDL_RenderCopy(renderer, placa, NULL, &placRect);

    pause = loadTextura("media/botao_pausa.png");
    SDL_Rect pauseRect = { 1100, 15, 74, 75 };
    SDL_RenderCopy(renderer, pause, NULL, &pauseRect);

  	SDL_DestroyTexture(porta);
    SDL_DestroyTexture(fundo);
    SDL_DestroyTexture(barra);
    SDL_DestroyTexture(vida);
    SDL_DestroyTexture(placa);
    SDL_DestroyTexture(pause);
    SDL_DestroyTexture(Nivel1);
    SDL_DestroyTexture(cartinha);
    SDL_DestroyTexture(chavinha);
    SDL_DestroyTexture(fim);
    if(game->label != NULL) {
    	SDL_DestroyTexture(game->label);
  	}

}

void RenderObjetos(SDL_Renderer *renderer, GameState *game) {
  int i;
    SDL_Texture *placa = NULL;

    placa = loadTextura("media/signRight.png");
    SDL_Rect placRect = { game->scrollX + 2050, 120, 70, 100 };
    SDL_RenderCopy(renderer, placa, NULL, &placRect);

  for (i = 0; i < 51; i++) { //quantidades de plataformas que existirão
    SDL_Rect platRect = { game->scrollX + game->plat[i].x, game->plat[i].y, 128, 30 };
    SDL_RenderCopy(renderer, game->plataforma, NULL, &platRect);
  }

  for (i = 0; i < 26; i++) { //quantidades de moedas que existirão
    SDL_Rect moedaRect = { game->scrollX + game->moedas[i].x, game->moedas[i].y, 30, 30 };
    SDL_RenderCopy(renderer, game->moeda, NULL, &moedaRect);
  }

  SDL_Rect chaveRect = { game->scrollX + game->chaves.x, game->chaves.y, 75, 40}; //chave era 100 e 50
  SDL_RenderCopy(renderer, game->chave, NULL, &chaveRect);

  for (i = 0; i < 1; i++) {
  	SDL_Rect cartaRect = { game->scrollX + game->cartas[i].x, game->cartas[i].y, 50, 30}; //carta era 69 e 42
  	SDL_RenderCopy(renderer, game->carta, NULL, &cartaRect);
  }

  //inimigo arrumar
  SDL_Rect inimRect = { game->scrollX + game->inim.x, game->inim.y, 50, 28 }; //colocar game->scrollX
  SDL_RenderCopyEx(renderer, game->inimFrames[0], NULL, &inimRect, 0, NULL, 0);

  //Alice
  SDL_Rect rect = { game->scrollX + game->alice.x, game->alice.y, 68, 118 };
  SDL_RenderCopyEx(renderer, game->aliceFrames[game->alice.animFrame], NULL, &rect, 0, NULL, (game->alice.facingLeft == 1));

  SDL_DestroyTexture(placa);
}

bool eventos(SDL_Window *janela, GameState *game) {
  SDL_Event event;
  bool jogando = true;
  while(SDL_PollEvent(&event)) {
    //printf ("%d\n", event.motion.x);
    switch (event.type) {
        case SDL_MOUSEMOTION:
          case SDL_MOUSEBUTTONDOWN:
            if (event.motion.x > 1105 && event.motion.x < 1170 && event.motion.y > 24 && event.motion.y < 84 && event.button.button == SDL_BUTTON_LEFT) {
                telapause(renderer);
            }
    }
    if (event.type == SDL_QUIT) {
      jogando = false;
      saida();
      exit(1);
      break;
    }
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case (SDLK_LALT && SDLK_F4):
          jogando = false;
          break;
        case SDLK_ESCAPE:
          telapause(renderer);
          break;
        case SDLK_SPACE:
          if (game->alice.onPlat == 1) { //só se tiver no chão
            game->alice.dy = -25;
            game->alice.onPlat = 0;
          }
          break;
      }
    }
  }
  
const Uint8 *state = SDL_GetKeyboardState(NULL); //arrumar
  if(state[SDL_SCANCODE_LEFT]) {
    if (game->alice.x - 6 < 0) {
      game->alice.x = 0;
    }
    else {
      game->alice.x -= 6;
    }

    game->alice.dx -= 0.5; //aceleração

    if(game->alice.dx < -5) {
      game->alice.dx = -5;
    }
    game->alice.facingLeft = 1;
    game->alice.slowingDown = 0;
  }

  else if(state[SDL_SCANCODE_RIGHT]) {
    if ((game->alice.x + 68 + 6) > 2560) {
      game->alice.x = 2560 - 68;
    }
    else {
      game->alice.x += 7;
    }

    game->alice.dx += 0.5; //aceleração
    if(game->alice.dx > 5) {
      game->alice.dx = 5;
    }
    game->alice.facingLeft = 0;
    game->alice.slowingDown = 0;
  }
  else if (state[SDL_SCANCODE_SPACE]) {
    if (game->alice.onPlat == 1) { //só se tiver no chão
      game->alice.dy = 0;
      game->alice.onPlat = 0;
    }
  }
  else {
    game->alice.animFrame = 0;
    game->alice.dx *= 0.8f;
    game->alice.slowingDown = 1;
    if (fabsf(game->alice.dx) < 0.1f) {
      game->alice.dx = 0;
    }
  }

  return jogando;

}

void processo(GameState *game) {
  //add time
  game->time++;

  //movimento da Alice
  Alice *alice = &game->alice;
  alice->x += alice->dx;
  alice->y += alice->dy;

  alice->dy += Gravidade;

  if (alice->dx != 0 && alice->onPlat && !alice->slowingDown) {
    if (game->time % 5 == 0) {
      if (alice->animFrame == 0) {
        alice->animFrame = 1;
      }
      else {
        alice->animFrame = 0;
      }
    }
  }

  //movimento do inimigo
  int velX = 10; //**
  INIMIGO *inim = &game->inim;

  inim->x -= velX;
  if (inim->x + 3 < 0) {  //vai só pra esquerda
    inim->x = 2515; //respawn
  }

  game->scrollX = -game->alice.x + 598;
  if(game->scrollX > 0) {
    game->scrollX = 0;
  }
  if(game->scrollX < -2580 + 598) {
    game->scrollX = -2580 + 598;
  }

}

void colisao(GameState *game) {
  int i = 0, j = 0;

  if (collide2d(game->alice.x, game->alice.y, game->inim.x, game->inim.y, 68, 118, 50, 28)) {  //colisao inimigo com uma velocidade de 10
      game->alice.morta = 1;
      if (ncolisao == 0 && j == 0) {
        game->alice.morta = 0;
        ncolisao = 1;
        game->alice.x += 5;
      }
      if (ncolisao == 1) {
        j = 1;
      }
      game->time % 24 == 0;
      if (game->time % 24 == 0 && ncolisao == 1 && j == 1) {
        ncolisao = 2;
        game->alice.x += 5;
      }
      game->time % 25 == 0;
      if (ncolisao == 2) {
        j = 2;
      }
      game->time % 10 == 0;
      if (game->time % 10 == 0 && ncolisao == 2 && j == 2) {
        ncolisao = 3;
        game->alice.x += 5;
      }
        if (ncolisao == 1) {
          game->alice.lives = 2;
        }
        if (ncolisao == 2) {
          game->alice.lives = 1;
        }
        if (ncolisao == 3) {
          game->alice.lives = 0;
        }
    }

  for (i = 0; i < 26; i++) { //colisao das moedas
    if (collide2d(game->alice.x, game->alice.y, game->moedas[i].x, game->moedas[i].y, 68, 118, 30, 30)) { 
      game->alice.pontos = game->alice.pontos + 100;
      game->moedas[i].x = -30;
      game->moedas[i].y = -30;
    }
  }


  if (collide2d(game->alice.x, game->alice.y, game->chaves.x, game->chaves.y, 68, 118, 100, 50)) { //colisao da chave
      game->alice.Chaves = 1;
      game->chaves.x = -50;
      game->chaves.y = -50;
  }

  if (collide2d(game->alice.x, game->alice.y, game->cartas[0].x, game->cartas[0].y, 68, 118, 50, 30)) { //carta 1;
  	  game->alice.Carta1 = 1;
      game->cartas[0].x = -50;
      game->cartas[0].y = -50;
  }

  for (i = 0; i < 51; i++) {
  float aw = 68, ah = 112; //largura e altura -3 pra ficar no chao da alice;
  float ax = game->alice.x, ay = game->alice.y; //posição da alice

  float pw = 128, ph = 29; //largura e altura da plat;
  float px = game->plat[i].x, py = game->plat[i].y; //posição da plat;

  if (px < ax+aw/2 && ax+aw/2 < px+pw) {
    if (ay < py+ph && ay > py && game->alice.dy < 0) {
        game->alice.y = py + ph;
        ay = py + ph; 
        game->alice.dy = 0;
        game->alice.onPlat = 1;
    }
  }

  if (px < ax+aw && ax < px+pw) {
    if (ay+ah > py && ay < py && game->alice.dy > 0) {
      game->alice.y = py - ah;
      ay = py - ah;
      game->alice.dy = 0;
      game->alice.onPlat = 1;
    }
  }

    if(ay+ah > py && ay < py+ph) {
      if(ax < px+pw && ax+aw > px+pw && game->alice.dx < 0) {
        game->alice.x = px+pw;
        ax = pw+pw;

        game->alice.dx = 0;
      }

      else if(ax+aw > px && ax < px && game->alice.dx > 0) {
        game->alice.x = px-aw;
        ax = px-aw;
        
        game->alice.dx = 0;
      }
    }
  }
}

int collide2d(int x1, int y1, int x2, int y2, int wt1, int ht1, int wt2, int ht2) {
  return (!((x1 > (x2+wt2)) || (x2 > (x1+wt1)) || (y1 > (y2+ht2)) || (y2 > (y1+ht1))));
}

int distancia(int x1, int y1, int x2, int y2) {
	return (sqrt(pow((x1-x2), 2)+(pow((y1-y2), 2))));
}
