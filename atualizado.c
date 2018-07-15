#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
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
  int pontos, Chaves, Carta1, Carta2, Carta3, Carta4, Carta5, Carta6, Carta7;
  int onPlat;

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

typedef struct {
  float x, y;
} PORTA;

typedef struct {
  float x, y;
} PAREDE;

typedef struct { //struct para pontos do record
  char nome[11];
  char ponto[11];
  int pontos;
} PONTUACAO;

typedef struct { //jogo
  float scrollX; //rolagem

  Alice alice; //players
  PLAT plat[100];
  MOEDA moedas[100];
  CARTA cartas[7];
  CHAVE chaves;
  PORTA marrom[2];
  PORTA verdeagua[2];
  PORTA roxa[2];
  PORTA amarela[2];
  PORTA laranja[2];
  PORTA azul[2];
  PORTA rosa[2];
  PORTA verde[2];
  PAREDE parede1[5];
  PAREDE parede2[5];
  PAREDE parede3[5];
  INIMIGO inim;

  PONTUACAO recorde[10];
    int recordista;
    int contador_recordista;
    char nome_recordista[5]; 

  SDL_Texture *aliceFrames[5]; //alterar pra não dar erro de seg
  SDL_Texture *inimFrames[4];
  SDL_Texture *plataforma;
  SDL_Texture *moeda;
  SDL_Texture *portamarrom;
  SDL_Texture *portaazul;
  SDL_Texture *portalaranja;
  SDL_Texture *portaverde;
  SDL_Texture *portaverdeagua;
  SDL_Texture *portaamarela;
  SDL_Texture *portarosa;
  SDL_Texture *portaroxa;
  SDL_Texture *muro1;
  SDL_Texture *muro2;
  SDL_Texture *muro3;
  SDL_Texture *chave;
  SDL_Texture *carta;
  SDL_Texture *fire;
  SDL_Texture *label;
  SDL_Texture *label2;
  int labelW, labelH, label2W, label2H;

  TTF_Font *font, *font1;

  int musicChannel;
  Mix_Chunk *bg, *MENU, *passa;

  int time, mortes;
  int statusState;

  SDL_Renderer *renderer;
} GameState;


bool inicializador();

void saida();

SDL_Texture* loadTextura (const char *path); 

bool telainicial(SDL_Renderer*, GameState*);

void telapause(SDL_Renderer*, GameState*);

void opcoesvolume(SDL_Renderer*, GameState*); 

bool nivel1 (SDL_Renderer*);

void loadGame(GameState*);

void RenderFundo(SDL_Renderer*, GameState*);

void RenderNivel(SDL_Renderer*, GameState*);

void RenderObjetos(SDL_Renderer*, GameState*);

bool eventos(SDL_Window*, GameState*);

void processo(GameState*);

void colisao(GameState*);

int collide2d(int, int, int, int, int, int, int, int);

int distancia(int, int, int, int);

void le_arquivo(PONTUACAO*);

void escreve_arquivo(PONTUACAO*, char*, int);

void ordena_recorder(PONTUACAO*);


SDL_Window* janela = NULL;
SDL_Renderer* renderer = NULL;

//Variáveis de controle
int cont = 0, reseta = 0, ncolisao = 0;

    int now = 0;
    int ex = 0;
    int periodeFps = 16; // 33ms  fps = 30 img/s
    int dt = 0;
    int i;
    float delta_s = 0;

//Dimensões da janela
const int LARG = 1280; 
const int ALT = 720;


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
    game.MENU = Mix_LoadWAV("media/IWish.ogg");
    if (game.MENU != NULL) {
      Mix_VolumeChunk(game.MENU, 10);
      game.musicChannel = Mix_PlayChannel(-1, game.MENU, -1);
    }
    if ( !telainicial(renderer, &game) ) {
      saida();
      IMG_Quit();
      SDL_Quit();
      exit(1);
    }
    Mix_HaltChannel(game.musicChannel);
    Mix_FreeChunk(game.MENU);

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
    exit(1);
  }
  else {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      printf("O programa não pode ser inicializado! SDL_ERROR: %s\n", SDL_GetError());
      sucesso = false;
      exit(1);
    }
  else {
    //srandom((int)time(NULL)); para caso querer colocar algo surgindo aleatóriamente
    janela = SDL_CreateWindow("7 letters from my love", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARG, ALT, SDL_WINDOW_SHOWN);
    if (janela == NULL) {
      printf("A janela não pode ser criada! SDL_ERROR: %s\n", SDL_GetError());
      sucesso = false;
      exit(1);
    }
    else {
      renderer = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (renderer == NULL) {
        printf("O renderer não pode ser criado! SDL Error: %s\n", SDL_GetError() );
        sucesso = false;
        exit(1);
      }
      else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        if ( TTF_Init() ) {
          printf( "SDL_ttf não pode inicializar! SDL_ttf Error: %s\n", TTF_GetError() );
          sucesso = false;
          exit(1);
        }
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
        int imgFlags = IMG_INIT_PNG;
        if ( !(IMG_Init(imgFlags) & imgFlags) ) {
          printf( "SDL_image não pode inicializar! SDL_image Error: %s\n", IMG_GetError() );
          sucesso = false;
          exit(1);
        }
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

  Mix_CloseAudio();
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


bool telainicial (SDL_Renderer *renderer, GameState* game) {
  bool sucesso = true, gameloop = true;
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

  game->passa = Mix_LoadWAV("media/passa.ogg");

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
           if (event.motion.x > 287 && event.motion.x < 600 && event.motion.y > 382 && event.motion.y < 460) {
            Mix_VolumeChunk(game->passa, 8);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
           }
           if (event.motion.x > 300 && event.motion.x < 610 && event.motion.y > 484 && event.motion.y < 546) {
            Mix_VolumeChunk(game->passa, 8);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
           }
           if (event.motion.x > 304 && event.motion.x < 613 && event.motion.y > 571 && event.motion.y < 660) {
            Mix_VolumeChunk(game->passa, 8);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
           }
           if (event.motion.x > 686 && event.motion.x < 1000 && event.motion.y > 380 && event.motion.y < 459) {
            Mix_VolumeChunk(game->passa, 8);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
           }
           if (event.motion.x > 676 && event.motion.x < 992 && event.motion.y > 475 && event.motion.y < 537) {
            Mix_VolumeChunk(game->passa, 8);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
           }
           if (event.motion.x > 1158 && event.motion.x < 1245 && event.motion.y > 611 && event.motion.y < 695) {
            Mix_VolumeChunk(game->passa, 8);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
           }
           if (event.motion.x > 40 && event.motion.x < 120 && event.motion.y > 608 && event.motion.y < 694) {
            Mix_VolumeChunk(game->passa, 8);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
           }

          case SDL_MOUSEBUTTONDOWN:
            if (event.motion.x > 287 && event.motion.x < 600 && event.motion.y > 382 && event.motion.y < 460 && event.button.button == SDL_BUTTON_LEFT) {
                SDL_RenderCopy(renderer, jogar2, NULL, &jogar2Rect);
                SDL_RenderPresent(renderer);
                Mix_HaltChannel(game->musicChannel);
                SDL_Delay(200);
                if(cont == 1) {
                  nivel1(renderer);
                }
                gameloop = false;
                nivel1(renderer);
            }
            else if (event.motion.x > 300 && event.motion.x < 610 && event.motion.y > 484 && event.motion.y < 546 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, niveis2, NULL, &niveis2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(200);
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgNiveis, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
            }

            else if (event.motion.x > 304 && event.motion.x < 613 && event.motion.y > 571 && event.motion.y < 660 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, cred2, NULL, &cred2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(200);
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgCreditos, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
            }

            else if (event.motion.x > 686 && event.motion.x < 1000 && event.motion.y > 380 && event.motion.y < 459 && event.button.button == SDL_BUTTON_LEFT) {
             SDL_RenderCopy(renderer, ranking2, NULL, &ran2Rect);
             SDL_RenderPresent(renderer);
              SDL_Delay(200); 
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgRanking, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
            }

            else if (event.motion.x > 676 && event.motion.x < 992 && event.motion.y > 475 && event.motion.y < 537 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, sair2, NULL, &sair2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(200);               
              gameloop = false;
              sucesso = false;
              exit(1);
            }

            else if (event.motion.x > 1158 && event.motion.x < 1245 && event.motion.y > 611 && event.motion.y < 695 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_Delay(200);
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgAjuda, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
            }

            else if(event.motion.x > 40 && event.motion.x < 120 && event.motion.y > 608 && event.motion.y < 694 && event.button.button == SDL_BUTTON_LEFT) { //botao retornar
              SDL_RenderCopy(renderer, retornar2, NULL, &ret2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(200); 
              telainicial(renderer, game);
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
    Mix_FreeChunk(game->passa);
    Mix_FreeChunk(game->MENU);

  return sucesso; //tentar arrumar o erro do mouse apertar pressionando

}


void telapause(SDL_Renderer *renderer, GameState* game) {
  bool gameloop = true;
  SDL_Event event;
  SDL_Texture *telaPause = NULL;

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


  game->passa = Mix_LoadWAV("media/passa.ogg");

  while (gameloop == true) {
  while(SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      gameloop = false;
      saida();
        exit(1);
    }
    else switch (event.type) {
        case SDL_MOUSEMOTION:
          if (event.motion.x > 372 && event.motion.x < 520 && event.motion.y > 372 && event.motion.y < 552) {
            Mix_VolumeChunk(game->passa, 5);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
          }

          if (event.motion.x > 565 && event.motion.x < 714 && event.motion.y > 372 && event.motion.y < 552) {
            Mix_VolumeChunk(game->passa, 5);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
          }

          if (event.motion.x > 780 && event.motion.x < 889 && event.motion.y > 372 && event.motion.y < 552) {
            Mix_VolumeChunk(game->passa, 5);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
          }

          case SDL_MOUSEBUTTONDOWN:
            if (event.motion.x > 372 && event.motion.x < 520 && event.motion.y > 372 && event.motion.y < 552 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, menu2, NULL, &menu2Rect);
              SDL_RenderPresent(renderer);
              cont = 1;
              SDL_Delay(800);
              Mix_HaltChannel(game->musicChannel);
              game->MENU = Mix_LoadWAV("media/IWish.ogg");
              Mix_VolumeChunk(game->MENU, 10);
              game->musicChannel = Mix_PlayChannel(-1, game->MENU, -1);
              telainicial(renderer, game);
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
              opcoesvolume(renderer, game);
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
  Mix_FreeChunk(game->passa);
  //Mix_FreeChunk(game->MENU);

}

void opcoesvolume(SDL_Renderer *renderer, GameState* game) { //alterar volumes (estudar)
  SDL_Texture *op2 = NULL;
  bool gameloop = true;
  SDL_Event event;

  SDL_Texture *retornar2 = NULL;
  SDL_Rect ret2Rect = {30, 600, 100, 100};
  retornar2 = loadTextura("media/retornar2.png");

  op2 = loadTextura("media/opcoes.png");
  SDL_RenderCopy(renderer, op2, NULL, NULL);

  SDL_Texture *retornar = NULL;
  retornar = loadTextura("media/retornar1.png");
  SDL_Rect retRect = {30, 600, 100, 100};
  SDL_RenderCopy(renderer, retornar, NULL, &retRect);

  SDL_RenderPresent(renderer);

  game->passa = Mix_LoadWAV("media/passa.ogg");

  while (gameloop == true) {
    while(SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        gameloop = false;
        saida();
        exit(1);
      }
      else switch (event.type) {
        case SDL_MOUSEMOTION:
          if(event.motion.x > 40 && event.motion.x < 120 && event.motion.y > 608 && event.motion.y < 694) {
            Mix_VolumeChunk(game->passa, 5);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
          }
          case SDL_MOUSEBUTTONDOWN:
          if(event.motion.x > 40 && event.motion.x < 120 && event.motion.y > 608 && event.motion.y < 694 && event.button.button == SDL_BUTTON_LEFT) { //botao retornar
              SDL_RenderCopy(renderer, retornar2, NULL, &ret2Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(200); 
              gameloop = false;
          }
        break;
      }
    }
  }

  SDL_DestroyTexture(op2);
  SDL_DestroyTexture(retornar);
  SDL_DestroyTexture(retornar2);
  //Mix_FreeChunk(game->passa);

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

  le_arquivo(game->recorde);
  if(game->alice.pontos > game->recorde[4].pontos) {
    game->recordista = 1;
  }

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
              game->alice.Carta1 = 0;
              game->alice.Carta2 = 0;
              game->alice.Carta3 = 0;
              game->alice.Carta4 = 0;
              game->alice.Carta5 = 0;
              game->alice.Carta6 = 0;
              game->alice.Carta7 = 0;
              nivel1(renderer);
              gameloop = false;
            } 
            if (event.motion.x > 92 && event.motion.x < 243 && event.motion.y > 503 && event.motion.y < 686 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_RenderCopy(renderer, menu3, NULL, &menu3Rect);
              SDL_RenderPresent(renderer);
              SDL_Delay(300);
              cont = 1;
              game->alice.Chaves = 0;
              game->alice.Carta1 = 0;
              game->alice.Carta2 = 0;
              game->alice.Carta3 = 0;
              game->alice.Carta4 = 0;
              game->alice.Carta5 = 0;
              game->alice.Carta6 = 0;
              game->alice.Carta7 = 0;
              gameloop = false;
              game->MENU = Mix_LoadWAV("media/IWish.ogg");
              Mix_VolumeChunk(game->MENU, 10);
              game->musicChannel = Mix_PlayChannel(-1, game->MENU, -1);
              telainicial(renderer, game);
              //gameloop = false;
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

  game.portamarrom = loadTextura("media/porta1.png");
  game.portaazul = loadTextura("media/porta7.png");
  game.portaroxa = loadTextura("media/porta2.png");
  game.portaverde = loadTextura("media/porta5.png");
  game.portaverdeagua = loadTextura("media/porta6.png");
  game.portaamarela = loadTextura("media/porta4.png");
  game.portalaranja = loadTextura("media/porta8.png");
  game.portarosa = loadTextura("media/porta3.png");

  game.muro1 = loadTextura("media/muro1.png");
  game.muro2 = loadTextura("media/muro2.png");
  game.muro3 = loadTextura("media/muro3.png");  

  game.aliceFrames[0] = loadTextura("media/alice1.png");
  game.aliceFrames[1] = loadTextura("media/alice2.png");
  game.aliceFrames[2] = loadTextura("media/alice3.png");
  game.aliceFrames[3] = loadTextura("media/alice4.png");
  game.inimFrames[0] = loadTextura("media/slimeWalk1.png");
  game.inimFrames[1] = loadTextura("media/slimeWalk2.png");

  game.bg = Mix_LoadWAV("media/bensound-pianomoment.ogg");
  if (game.bg != NULL) {
    Mix_VolumeChunk(game.bg, 3);
    game.musicChannel = Mix_PlayChannel(-1, game.bg, -1);
  }

    while(jogando != false) {
      now = SDL_GetTicks();
      dt = now - ex;
      if(dt > periodeFps) {
        delta_s = dt/1000.0f;
        SDL_RenderClear(renderer);
        RenderFundo(renderer, &game);
        RenderObjetos(renderer, &game);
        RenderNivel(renderer, &game);
        eventos(janela, &game);
        processo(&game);
        colisao(&game);
        SDL_RenderPresent(renderer);
        ex=now;
      }
      else {
        SDL_Delay(periodeFps - delta_s);
        //printf ("delay: %f\n", periodeFps - delta_s); // não tá entrando
      }
    }

    SDL_Delay(500);

  SDL_DestroyTexture(game.plataforma);
  SDL_DestroyTexture(game.moeda);
  SDL_DestroyTexture(game.chave);
  SDL_DestroyTexture(game.carta);
  SDL_DestroyTexture(game.portaroxa);
  SDL_DestroyTexture(game.portarosa);
  SDL_DestroyTexture(game.portaamarela);
  SDL_DestroyTexture(game.portaverdeagua);
  SDL_DestroyTexture(game.portaverde);
  SDL_DestroyTexture(game.portalaranja);
  SDL_DestroyTexture(game.portaazul);
  SDL_DestroyTexture(game.portamarrom);
  SDL_DestroyTexture(game.muro1);
  SDL_DestroyTexture(game.muro2);
  SDL_DestroyTexture(game.muro3);
  SDL_DestroyTexture(game.aliceFrames[0]);
  SDL_DestroyTexture(game.aliceFrames[1]); 
  SDL_DestroyTexture(game.aliceFrames[2]);
  SDL_DestroyTexture(game.aliceFrames[3]);
  SDL_DestroyTexture(game.inimFrames[0]);
  SDL_DestroyTexture(game.inimFrames[1]); 
  TTF_CloseFont(game.font);
  TTF_CloseFont(game.font1);
  Mix_FreeChunk(game.bg);


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
  game->alice.Carta1 = 0;
  game->alice.Carta2 = 0;
  game->alice.Carta3 = 0;
  game->alice.Carta4 = 0;
  game->alice.Carta5 = 0;
  game->alice.Carta6 = 0;
  game->alice.Carta7 = 0;

  game->recordista = 0;
  game->contador_recordista = 0;
  for(i = 0; i < 5; i++) {
     game->nome_recordista[i] = '\0';
  }

  game->time =  0;
  game->scrollX = 0; 

  //posição das plataformas 
  for (i = 0; i < 39; i++) { //do chão
    game->plat[i].x = 0+(128*i);
    game->plat[i].y = 675;
  }

  // Parte 1

  game->plat[39].x = 0;
  game->plat[39].y = 410;

  game->plat[40].x = 128;
  game->plat[40].y = 410;

  game->plat[41].x = 256;
  game->plat[41].y = 410;

  game->plat[42].x = 384;
  game->plat[42].y = 410;

  game->plat[43].x = 649;
  game->plat[43].y = 556;

  game->plat[44].x = 777;
  game->plat[44].y = 556;

  game->plat[45].x = 896;
  game->plat[45].y = 225;

  game->plat[46].x = 1024;
  game->plat[46].y = 225;

  game->plat[47].x = 1152;
  game->plat[47].y = 225;

  // Parte 2

  game->plat[48].x = 1455;
  game->plat[48].y = 410;

  game->plat[49].x = 1583;
  game->plat[49].y = 410;

  game->plat[50].x = 1711;
  game->plat[50].y = 410;

  game->plat[51].x = 1839;
  game->plat[51].y = 310;

  game->plat[52].x = 1967;
  game->plat[52].y = 310;

  game->plat[53].x = 2095;
  game->plat[53].y = 210;

  game->plat[54].x = 2223;
  game->plat[54].y = 210;

  game->plat[55].x = 2095;
  game->plat[55].y = 589;

  game->plat[56].x = 2223;
  game->plat[56].y = 505;

  game->plat[57].x = 2351;
  game->plat[57].y = 505;

  // Parte 3

  game->plat[58].x = 2771;
  game->plat[58].y = 230;

  game->plat[59].x = 2899;
  game->plat[59].y = 230;

  game->plat[60].x = 3027;
  game->plat[60].y = 230;

  game->plat[61].x = 3155;
  game->plat[61].y = 230;

  game->plat[62].x = 3283;
  game->plat[62].y = 230;

  game->plat[63].x = 3411;
  game->plat[63].y = 230;

  game->plat[64].x = 3539;
  game->plat[64].y = 230;

  game->plat[65].x = 2643;
  game->plat[65].y = 424;

  game->plat[66].x = 2771;
  game->plat[66].y = 424;

  game->plat[67].x = 2889;
  game->plat[67].y = 424;

  game->plat[68].x = 3460;
  game->plat[68].y = 490;

  game->plat[69].x = 3584;
  game->plat[69].y = 490;

  game->plat[70].x = 3712;
  game->plat[70].y = 490;

  // Parte 4

  game->plat[71].x = 4064;
  game->plat[71].y = 270;

  game->plat[72].x = 4192;
  game->plat[72].y = 270;

  game->plat[73].x = 4320;
  game->plat[73].y = 270;

  game->plat[74].x = 4704;
  game->plat[74].y = 270;

  game->plat[75].x = 4832;
  game->plat[75].y = 270;

  game->plat[76].x = 4064;
  game->plat[76].y = 440;

  game->plat[77].x = 4192;
  game->plat[77].y = 440;

  game->plat[78].x = 4320;
  game->plat[78].y = 440;

  game->plat[79].x = 4448;
  game->plat[79].y = 440;

  game->plat[80].x = 4576;
  game->plat[80].y = 440;

  game->plat[81].x = 4704;
  game->plat[81].y = 440;

  game->plat[82].x = 4832;
  game->plat[82].y = 440;

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

  //portas
  game->marrom[0].x = 45;
  game->marrom[0].y = 279;

  game->marrom[1].x = 329;
  game->marrom[1].y = 537;

  game->verdeagua[0].x = 777;
  game->verdeagua[0].y = 426;

  game->verdeagua[1].x = 2117;
  game->verdeagua[1].y = 80;

  game->roxa[0].x = 960;
  game->roxa[0].y = 92;

  game->roxa[1].x = 4832;
  game->roxa[1].y = 538;

  game->amarela[0].x = 1647;
  game->amarela[0].y = 279;

  game->amarela[1].x = 2832;
  game->amarela[1].y = 97;

  game->laranja[0].x = 1839;
  game->laranja[0].y = 537;

  game->laranja[1].x = 2676;
  game->laranja[1].y = 293;

  game->azul[0].x = 2351;
  game->azul[0].y = 375;

  game->azul[1].x = 4776;
  game->azul[1].y = 137;

  game->rosa[0].x = 3510;
  game->rosa[0].y = 357;

  game->rosa[1].x = 3712;
  game->rosa[1].y = 537;

  game->verde[0].x = 4277;
  game->verde[0].y = 137;

  game->verde[1].x = 4128;
  game->verde[1].y = 307;

  //chave
  game->chaves.x = 370;
  game->chaves.y = 340; //350

  //inimigo
  game->inim.x = 2515;
  game->inim.y = 644;

  //cartas
  game->cartas[0].x = 285;
  game->cartas[0].y = 340; 

  game->cartas[1].x = 1131;
  game->cartas[1].y = 155; 

  game->cartas[2].x = 1938;
  game->cartas[2].y = 240; 

  game->cartas[3].x = 2876;
  game->cartas[3].y = 354; 

  game->cartas[4].x = 3543;
  game->cartas[4].y = 600;   

  game->cartas[5].x = 4150;
  game->cartas[5].y = 200; 

  game->cartas[6].x = 4827;
  game->cartas[6].y = 370; 

  // Paredes

  game->parede1[0].x = 2315;
  game->parede1[0].y = 72;

  game->parede1[1].x = 2771;
  game->parede1[1].y = 92;

  game->parede1[2].x = 2607;
  game->parede1[2].y = 208;

  game->parede1[3].x = 4062;
  game->parede1[3].y = 302;

  game->parede1[4].x = 4412;
  game->parede1[4].y = 132;


  game->parede2[0].x = 476;
  game->parede2[0].y = 441;

  game->parede2[1].x = 3460;
  game->parede2[1].y = 260;


  game->parede3[0].x = 2607;
  game->parede3[0].y = 346;

}

void RenderFundo(SDL_Renderer *renderer, GameState *game) {

  SDL_Texture *fundo = NULL;

  fundo = loadTextura("media/mapa_extendido.png");
  SDL_Rect fundoRect = {game->scrollX + delta_s + 0, 0, 5120, 720}; //larg+155
  SDL_RenderCopy(renderer, fundo, NULL, &fundoRect);

  SDL_DestroyTexture(fundo);

}

void RenderNivel(SDL_Renderer *renderer, GameState *game) {
  SDL_Color preto = {0, 0, 0, 0};
  SDL_Texture *barra = NULL;
  SDL_Texture *vida = NULL;
  SDL_Texture *vida2 = NULL;
  SDL_Texture *vida3 = NULL;
  SDL_Texture *vida4 = NULL;
  SDL_Texture *placa = NULL;
  SDL_Texture *pause = NULL;
  SDL_Texture *Nivel1 = NULL;
  SDL_Texture *chavinha = NULL;
  SDL_Texture *cartinha = NULL;
  SDL_Texture *fim = NULL;

    //SDL_RenderClear(renderer);

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
    SDL_Rect textRect = {960, 60-game->labelH, game->labelW, game->labelH}; //960
    SDL_RenderCopy(renderer, game->label, NULL, &textRect); 

    Nivel1 = loadTextura("media/nivel1.png");
    SDL_Rect nivel1Rect = {560, 35, 153, 41};
    SDL_RenderCopy(renderer, Nivel1, NULL, &nivel1Rect);

    vida = loadTextura("media/vida1.png");
    vida2 = loadTextura("media/vida2.png");
    vida3 = loadTextura("media/vida3.png");
    vida4 = loadTextura("media/vida4.png");

    SDL_Rect vidaRect = {180, 10, 173, 39};

    cartinha = loadTextura("media/carta.png");
    SDL_Rect cartinhaRect = {860, 10, 40, 30};
    SDL_RenderCopy(renderer, cartinha, NULL, &cartinhaRect);


    if (game->alice.Carta1 >= 0) {
      char str[10] = "";
      int totalchaves = game->alice.Carta1 + game->alice.Carta2 + game->alice.Carta3 + game->alice.Carta4 + game->alice.Carta5 + game->alice.Carta6 + game->alice.Carta7;
      sprintf (str, "x%d", totalchaves);
      SDL_Surface *tmp = TTF_RenderText_Blended(game->font1, str, preto); //colcar menor
      game->labelW = tmp->w;
      game->labelH = tmp->h;
      game->label = SDL_CreateTextureFromSurface(renderer, tmp);
      SDL_FreeSurface(tmp);
      SDL_Rect textRect = {900, 45-game->labelH, game->labelW, game->labelH}; 
      SDL_RenderCopy(renderer, game->label, NULL, &textRect); 
    } 

    if (game->alice.lives == 3) {
      SDL_RenderCopy(renderer, vida, NULL, &vidaRect);
    }

    if (game->alice.lives == 2) {
      SDL_RenderCopy(renderer, vida2, NULL, &vidaRect);
    }

    if (game->alice.lives == 1) {
      SDL_RenderCopy(renderer, vida3, NULL, &vidaRect);
    }

    if (game->alice.lives == 0) {
      SDL_RenderCopy(renderer, vida4, NULL, &vidaRect);
      SDL_Delay(300);
      game->alice.Chaves = 0;
      game->alice.Carta1 = 0;
      Mix_HaltChannel(game->musicChannel);
      telafim(renderer,game);
      cont = 1;
      reseta = 1;
    }

    chavinha = loadTextura("media/keyYellow.png");
    SDL_Rect chavRect = {370, 7, 50, 50};

    //printf("%f\n", game->alice.y);
    if (game->alice.Chaves == 1) {
      SDL_RenderCopy(renderer, chavinha, NULL, &chavRect);
      if (game->alice.Carta1 == 1 && game->alice.x > 2451 && game->alice.x < 2454 && game->alice.y > 87 && game->alice.y < 90) { //fazer colisao
        game->alice.Chaves = 0;
        game->alice.Carta1 = 0;
        game->alice.Carta2 = 0;
        game->alice.Carta3 = 0;
        game->alice.Carta4 = 0;
        game->alice.Carta5 = 0;
        game->alice.Carta6 = 0;
        game->alice.Carta7 = 0;
        Mix_HaltChannel(game->musicChannel);
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

    SDL_DestroyTexture(barra);
    SDL_DestroyTexture(vida);
    SDL_DestroyTexture(vida2);
    SDL_DestroyTexture(vida3);
    SDL_DestroyTexture(vida4);
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

    /*SDL_Texture *placa = NULL;
    placa = loadTextura("media/signRight.png");
    SDL_Rect placRect = { game->scrollX + delta_s + 2050, 120, 70, 100 };
    SDL_RenderCopy(renderer, placa, NULL, &placRect);*/

  for (i = 0; i < 83; i++) { //quantidades de plataformas que existirão 
    SDL_Rect platRect = { game->scrollX + game->plat[i].x + delta_s, game->plat[i].y, 128, 30 };
    SDL_RenderCopy(renderer, game->plataforma, NULL, &platRect);
  }

  for (i = 0; i < 26; i++) { //quantidades de moedas que existirão
    SDL_Rect moedaRect = { game->scrollX + game->moedas[i].x + delta_s, game->moedas[i].y, 30, 30 };
    SDL_RenderCopy(renderer, game->moeda, NULL, &moedaRect);
  }

  SDL_Rect chaveRect = { game->scrollX + game->chaves.x + delta_s, game->chaves.y, 75, 40}; //chave era 100 e 50
  SDL_RenderCopy(renderer, game->chave, NULL, &chaveRect);

  for (i = 0; i < 7; i++) {
    SDL_Rect cartaRect = { game->scrollX + game->cartas[i].x + delta_s, game->cartas[i].y, 50, 30}; //carta era 69 e 42
    SDL_RenderCopy(renderer, game->carta, NULL, &cartaRect);
  }

  // Portas

  for (i = 0; i < 2; i++) {
    SDL_Rect marromRect = { game->scrollX + game->marrom[i].x + delta_s, game->marrom[i].y, 113, 133};
    SDL_RenderCopy(renderer, game->portamarrom, NULL, &marromRect);
  }

  for (i = 0; i < 2; i++) {
    SDL_Rect verdeaguaRect = { game->scrollX + game->verdeagua[i].x + delta_s, game->verdeagua[i].y, 113, 133};
    SDL_RenderCopy(renderer, game->portaverdeagua, NULL, &verdeaguaRect);
  }

  for (i = 0; i < 2; i++) {
    SDL_Rect roxaRect = { game->scrollX + game->roxa[i].x + delta_s, game->roxa[i].y, 113, 133};
    SDL_RenderCopy(renderer, game->portaroxa, NULL, &roxaRect);
  }

  for (i = 0; i < 2; i++) {
    SDL_Rect amarelaRect = { game->scrollX + game->amarela[i].x + delta_s, game->amarela[i].y, 113, 133};
    SDL_RenderCopy(renderer, game->portaamarela, NULL, &amarelaRect);
  }

  for (i = 0; i < 2; i++) {
    SDL_Rect laranjaRect = { game->scrollX + game->laranja[i].x + delta_s, game->laranja[i].y, 113, 133};
    SDL_RenderCopy(renderer, game->portalaranja, NULL, &laranjaRect);
  }

  for (i = 0; i < 2; i++) {
    SDL_Rect azulRect = { game->scrollX + game->azul[i].x + delta_s, game->azul[i].y, 113, 133};
    SDL_RenderCopy(renderer, game->portaazul, NULL, &azulRect);
  }

  for (i = 0; i < 2; i++) {
    SDL_Rect rosaRect = { game->scrollX + game->rosa[i].x + delta_s, game->rosa[i].y, 113, 133};
    SDL_RenderCopy(renderer, game->portarosa, NULL, &rosaRect);
  }

  for (i = 0; i < 2; i++) {
    SDL_Rect verdeRect = { game->scrollX + game->verde[i].x + delta_s, game->verde[i].y, 113, 133};
    SDL_RenderCopy(renderer, game->portaverde, NULL, &verdeRect);
  }

  // Paredes

  for (i = 0; i < 5; i++) {
    SDL_Rect muro1Rect = { game->scrollX + game->parede1[i].x + delta_s, game->parede1[i].y, 36, 138};
    SDL_RenderCopy(renderer, game->muro1, NULL, &muro1Rect);
  }

  for (i = 0; i < 2; i++) {
    SDL_Rect muro2Rect = { game->scrollX + game->parede2[i].x + delta_s, game->parede2[i].y, 36, 230};
    SDL_RenderCopy(renderer, game->muro2, NULL, &muro2Rect);
  }

  for (i = 0; i < 1; i++) {
    SDL_Rect muro3Rect = { game->scrollX + game->parede3[i].x + delta_s, game->parede3[i].y, 36, 322};
    SDL_RenderCopy(renderer, game->muro3, NULL, &muro3Rect);
  }

  //inimigo
  //SDL_Rect inimRect = { game->scrollX + game->inim.x + delta_s, game->inim.y, 50, 28 };

  //inimigo arrumar

  SDL_Rect inimRect = { game->scrollX + game->inim.x + delta_s, game->inim.y, 50, 28 }; //colocar game->scrollX
  SDL_RenderCopyEx(renderer, game->inimFrames[0], NULL, &inimRect, 0, NULL, 0);

  //Alice
  SDL_Rect rect = { game->alice.x + game->scrollX, game->alice.y, 68, 118 };
  SDL_RenderCopyEx(renderer, game->aliceFrames[game->alice.animFrame], NULL, &rect, 0, NULL, (game->alice.facingLeft == 1));


  //SDL_DestroyTexture(placa);

}

bool eventos(SDL_Window *janela, GameState *game) {
  SDL_Event event;
  game->passa = Mix_LoadWAV("media/passa.ogg");

  bool jogando = true;
  while(SDL_PollEvent(&event)) {
    //printf ("%d\n", event.motion.x);
    switch (event.type) {
        case SDL_MOUSEMOTION:
          if (event.motion.x > 1105 && event.motion.x < 1170 && event.motion.y > 24 && event.motion.y < 84) {
            Mix_VolumeChunk(game->passa, 5);
            game->musicChannel = Mix_PlayChannel(-1, game->passa, 0);
          }
          case SDL_MOUSEBUTTONDOWN:
            if (event.motion.x > 1105 && event.motion.x < 1170 && event.motion.y > 24 && event.motion.y < 84 && event.button.button == SDL_BUTTON_LEFT) {
                telapause(renderer, game);
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
          telapause(renderer, game);
          break;
        case SDLK_SPACE:
          if (game->alice.onPlat == 1) { //só se tiver no chão
            game->alice.dy = -25 - delta_s;
            game->alice.onPlat = 0;
          }
          break;
      }
    }
  }

  
const Uint8 *state = SDL_GetKeyboardState(NULL); //arrumar
  if(state[SDL_SCANCODE_LEFT]) {
    if (game->alice.x - 10 < 0) {
      game->alice.x = 0;
    }
    else {
      game->alice.x -= 10 - delta_s;
    }

    game->alice.dx -= 0.5; //aceleração

    if(game->alice.dx < -5) {
      game->alice.dx = -5;
    }
    game->alice.facingLeft = 1;
    game->alice.slowingDown = 0;
  }

  else if(state[SDL_SCANCODE_RIGHT]) {
    if ((game->alice.x + 68 + 10) > 4975) { //
      game->alice.x = 4975 - 68; //
    }
    else {
      game->alice.x += 10 + delta_s;
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

  Alice *alice = &game->alice;
  alice->x += alice->dx;
  alice->y += alice->dy;

  alice->dy += Gravidade;


  if (alice->dx != 0 && alice->onPlat && !alice->slowingDown) {
    if (game->time % 5 == 0) {
      if (alice->animFrame == 0) {
        alice->animFrame = 1;
      }
      else if (alice->animFrame = 1) {
        alice->animFrame = 2;
      }
      else if (alice->animFrame = 2) {
        alice->animFrame = 3;
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
    inim->x = 1280; //respawn
  }

  //scrollX
  game->scrollX = -game->alice.x + 598;
  if(game->scrollX > 0) {
    game->scrollX = 0;
  }
  if(game->alice.x > 4427 && game->alice.x < 5140) { //2012 2580
    game->scrollX = -5140 + (2*658);
  }
}


void colisao(GameState *game) {
  int i = 0, j = 0;

  if (collide2d(game->alice.x, game->alice.y, game->inim.x, game->inim.y, 68, 118, 50, 28)) {  //colisao inimigo com uma velocidade de 10
      if (ncolisao == 0 && j == 0) {
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

  if (collide2d(game->alice.x, game->alice.y, game->cartas[1].x, game->cartas[1].y, 68, 118, 50, 30)) { //carta 2;
      game->alice.Carta2 = 1;
      game->cartas[1].x = -50;
      game->cartas[1].y = -50;
  }

  if (collide2d(game->alice.x, game->alice.y, game->cartas[2].x, game->cartas[2].y, 68, 118, 50, 30)) { //carta 3;
      game->alice.Carta3 = 1;
      game->cartas[2].x = -50;
      game->cartas[2].y = -50;
  }

  if (collide2d(game->alice.x, game->alice.y, game->cartas[3].x, game->cartas[3].y, 68, 118, 50, 30)) { //carta 4;
      game->alice.Carta4 = 1;
      game->cartas[3].x = -50;
      game->cartas[3].y = -50;
  }

  if (collide2d(game->alice.x, game->alice.y, game->cartas[4].x, game->cartas[4].y, 68, 118, 50, 30)) { //carta 5;
      game->alice.Carta5 = 1;
      game->cartas[4].x = -50;
      game->cartas[4].y = -50;
  }

  if (collide2d(game->alice.x, game->alice.y, game->cartas[5].x, game->cartas[5].y, 68, 118, 50, 30)) { //carta 6;
      game->alice.Carta6 = 1;
      game->cartas[5].x = -50;
      game->cartas[5].y = -50;
  }

  if (collide2d(game->alice.x, game->alice.y, game->cartas[6].x, game->cartas[6].y, 68, 118, 50, 30)) { //carta 7;
      game->alice.Carta7 = 1;
      game->cartas[6].x = -50;
      game->cartas[6].y = -50;
  }

  //teleporte marrom
  if (collide2d(game->alice.x, game->alice.y, game->marrom[0].x - game->marrom[1].x/4, game->marrom[0].y, 68, 118, 113, 133)) { 
      game->alice.x = game->marrom[1].x - 90;
      game->alice.y = game->marrom[1].y+3;
      Alice *alice = &game->alice;
      alice->x += alice->dx;
      alice->y += alice->dy;
      alice->dy += Gravidade;  
  }

  if (collide2d(game->alice.x, game->alice.y, game->marrom[1].x+game->marrom[1].x/4, game->marrom[1].y, 68, 118, 20, 133)) { 
      game->alice.x = game->marrom[0].x + 110;
      game->alice.y = game->marrom[0].y;
      Alice *alice = &game->alice;
      alice->x += alice->dx;
      alice->y += alice->dy;
      alice->dy += Gravidade;
  }

  //teleporte verdeagua
  if (collide2d(game->alice.x, game->alice.y, game->verdeagua[0].x + game->verdeagua[0].x/8, game->verdeagua[0].y, 68, 118, 10, 133)) { 
      game->alice.x = game->verdeagua[1].x - 90;
      game->alice.y = game->verdeagua[1].y + 3;
      Alice *alice = &game->alice;
      alice->x += alice->dx;
      alice->y += alice->dy;
      alice->dy += Gravidade;  
  }
  
  if (collide2d(game->alice.x, game->alice.y, game->verdeagua[1].x + game->verdeagua[1].x/22, game->verdeagua[1].y, 68, 118, 60, 133)) { 
      game->alice.x = game->verdeagua[0].x - 110;
      game->alice.y = game->verdeagua[0].y;
      Alice *alice = &game->alice;
      alice->x += alice->dx;
      alice->y += alice->dy;
      alice->dy += Gravidade;
  }

  //teleporte amarelo
  /*if (collide2d(game->alice.x, game->alice.y, game->amarela[0].x + 60, game->amarela[0].y, 68, 118, 113, 133)) { 
      game->alice.x = game->verdeagua[1].x - 90;
      game->alice.y = game->verdeagua[1].y + 3;
      Alice *alice = &game->alice;
      alice->x += alice->dx;
      alice->y += alice->dy;
      alice->dy += Gravidade;  
  }
  
  if (collide2d(game->alice.x, game->alice.y, game->amarela[1].x - game->amarela[1].x/4, game->amarela[1].y, 68, 118, 113, 133)) { 
      game->alice.x = game->verdeagua[0].x + 110;
      game->alice.y = game->verdeagua[0].y;
      Alice *alice = &game->alice;
      alice->x += alice->dx;
      alice->y += alice->dy;
      alice->dy += Gravidade;
  }*/



  for (i = 0; i < 83; i++) { //71
  float aw = 68, ah = 112; //largura e altura -3 pra ficar no chao da alice;
  float ax = game->alice.x, ay = game->alice.y; //posição da alice

  float pw = 128, ph = 29; //largura e altura da plat;
  float px = game->plat[i].x, py = game->plat[i].y; //posição da plat;

  if (px < ax+aw/2 && ax+aw/2 < px+pw) { //não bate a cabeça na plat
    if (ay < py+ph && ay > py && game->alice.dy < 0) {
        game->alice.y = py + ph;
        ay = py + ph; 
        game->alice.dy = 0;
        game->alice.onPlat = 1;
    }
  }

  if (px < ax+aw && ax < px+pw) { //não cai da plat
    if (ay+ah > py && ay < py && game->alice.dy > 0) {
      game->alice.y = py - ah;
      ay = py - ah;
      game->alice.dy = 0;
      game->alice.onPlat = 1;
    }
  }

    if(ay+ah > py && ay < py+ph) { //direita
      if(ax < px+pw && ax+aw > px+pw && game->alice.dx < 0) {
        game->alice.x = px+pw;
        ax = pw+pw-10;

        game->alice.dx = 0;
      }

      else if(ax+aw > px && ax < px && game->alice.dx > 0) {
        game->alice.x = px-aw;
        ax = px-aw;
        
        game->alice.dx = 0;
      }
    }
  }

  //distancia dos muros
  for (i = 0; i < 5; i++) {
    if(distancia(game->alice.x, game->alice.y, game->parede1[i].x, game->parede1[i].y) <= 5) {
      if ((game->alice.x + 68 + 10) > game->parede1[i].y) { //
        game->alice.x = game->parede1[i].y - 68; //
    }
    
    float aw = 68, ah = 112; //largura e altura -3 pra ficar no chao da alice;
    float ax = game->alice.x, ay = game->alice.y; //posição da alice  
    float pw = 36, ph = 138; //largura e altura da plat;
    float px = game->parede1[i].x, py = game->parede1[i].y; //posição da plat;

      //game->alice.x -= 5;
    if(ay+ah > py && ay < py+ph) { //direita
      if(ax < px+pw && ax+aw > px+pw && game->alice.dx < 0) {
        game->alice.x = px+pw;
        ax = pw+pw-10;

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
    //if(collide2d(game->alice.x, game->alice.y, game->parede1[i].x, game->parede1[i].y, 68, 118, 36, 138))
  }

int collide2d(int x1, int y1, int x2, int y2, int wt1, int ht1, int wt2, int ht2) {
  return (!((x1 > (x2+wt2)) || (x2 > (x1+wt1)) || (y1 > (y2+ht2)) || (y2 > (y1+ht1))));
}

int distancia (int x1, int y1, int x2, int y2) {
  return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}

void le_arquivo(PONTUACAO* pontuacao) {
    int i = 0;
    FILE* f = fopen("scores.txt", "r");
    if(f == NULL) {
        f = fopen("scores.txt","w");
        if(f == NULL) {
            perror("scores.txt: \n");
            exit(1);
        }
        for(i=0;i<10;i++) {
            pontuacao[i].nome[0]='.';
            pontuacao[i].nome[1]='.';
            pontuacao[i].nome[2]='.';
            pontuacao[i].nome[3]='\0';
            pontuacao[i].pontos=0;

            fprintf(f,"%s %d",pontuacao[i].nome,pontuacao[i].pontos);
            fprintf(f,"\n");
        }
    }
    else {
        while(!feof(f)) {
            fscanf(f,"%s %d",pontuacao[i].nome,&pontuacao[i].pontos);
            fgetc(f);
            i++;
        }
    }
    fclose(f);
}
void escreve_arquivo(PONTUACAO* pontuacao, char* nome, int pontos) {
    int i = 0, aux = 10;
    FILE* f = fopen("scores.txt", "w");
    if(f == NULL) {
        perror("scores.txt: \n");
        exit(1);
    }
    strcpy(pontuacao[9].nome,nome);
    pontuacao[9].pontos=pontos;
    ordena_recorder(pontuacao);
    while(aux){
        fprintf(f,"%s %d",pontuacao[i].nome,pontuacao[i].pontos);
        fprintf(f,"\n");
        i++;
        aux--;
    }
    fclose(f);
}
void ordena_recorder(PONTUACAO* pontuacao) {
    int tam = 10,i;
    PONTUACAO swap;
    while(tam != 0) {
        for(i = 0; i < 9; i++) {
            if(pontuacao[i].pontos < pontuacao[i+1].pontos) {
                swap = pontuacao[i];
                pontuacao[i] = pontuacao[i+1];
                pontuacao[i+1]=swap;
            }
        }
        tam--;
    }
}

/*void dispose_recordes(GameState* game, Paisagem* Pintro)
-{
-    SDL_FreeSurface(Pintro->paisagem);
-    if(Pintro->menu[0]!=NULL)
-    {
-        SDL_FreeSurface(Pintro->menu[0]);
-        Pintro->menu[0]=NULL;
-    }
-    Pintro->paisagem=NULL;
-}
-void dispose_novo_recorde(Motor* Pmotor, Paisagem* Pintro)
-{
-    SDL_FreeSurface(Pintro->paisagem);
-
-    Pintro->paisagem=NULL;
-
-    //TTF_CloseFont(Pmotor->fonte);
-
-    cria_intro(Pmotor,Pintro);
-}
-void desenha_texto(char *texto, Motor* Pmotor,int x, int y,SDL_Color cor)
-{
-    SDL_Surface* tmp;
-
-    tmp = TTF_RenderText_Blended(Pmotor->fonte,texto,cor);
-
-    SDL_Rect tela_rect = {x, y, 0, 0};
-
-    SDL_BlitSurface(tmp,NULL,Pmotor->tela,&tela_rect);
-
-    SDL_FreeSurface(tmp);
-    
-    //SDL_Flip(Pmotor->tela);
-}
-void cria_recordes(Motor* Pmotor,Paisagem* Pintro)
-{
-    le_arquivo(Pmotor->recorde);
-    dispose_intro(Pintro);
-    Pmotor->rodando=2;
-
-    const SDL_VideoInfo *pinfo = NULL;
-    pinfo = SDL_GetVideoInfo();
-
-    int bpp=pinfo->vfmt->BitsPerPixel,i=0;
-
-    //tela
-    Pmotor->tela = NULL;
-    Pmotor->tela=SDL_SetVideoMode(LARGURA,ALTURA,bpp,SDL_HWSURFACE);
-    SDL_WM_SetCaption("recordes",NULL);
-
-    Pintro->rect_paisagem.x=0;
-    Pintro->rect_paisagem.y=0;
-    Pintro->rect_paisagem.w=LARGURA;
-    Pintro->rect_paisagem.h=ALTURA;
-
-    //paisagem
-    Pintro->paisagem = NULL;
-    SDL_Surface *tmp=NULL;
-    tmp = IMG_Load("./imagens/recordes.png");
-
-    Pintro->paisagem = SDL_DisplayFormat(tmp);
-
-    SDL_FreeSurface(tmp);
-    tmp = NULL;
-
-    
-    //voltar
-    tmp = IMG_Load("./imagens/buttonback.png");
-
-    Pintro->rect_menu[0].x=LARGURA-138;
-    Pintro->rect_menu[0].y=ALTURA-120;
-    Pintro->rect_menu[0].w=138;
-    Pintro->rect_menu[0].h=120;
-
-    Pmotor -> backSprite.y = 0;
-    Pmotor -> backSprite.h = 120;
-    Pmotor -> backSprite.x = 0;
-    Pmotor -> backSprite.w = 139;
-
-    
-
-    Pintro->menu[0] = SDL_DisplayFormat(tmp);
-    SDL_FreeSurface(tmp);
-    tmp = NULL;
-    //
-
-    SDL_BlitSurface(Pintro->paisagem,NULL,Pmotor->tela,&Pintro->rect_paisagem);
-
-    SDL_BlitSurface(Pintro->menu[0],&Pmotor->backSprite,Pmotor->tela,&Pintro->rect_menu[0]);
-
-    Pmotor->fonte = TTF_OpenFont("./fontes/ARCADEPI.ttf",30);
-
-    SDL_Color cor = {255,255,255};
-    SDL_Color cor2 = {255,255,200};
-
-    for(i=0;i<10;i++)
-    {
-        sprintf(Pmotor->recorde[i].pontu,"%d",Pmotor->recorde[i].pontos); 
-        desenha_texto(Pmotor->recorde[i].nome,Pmotor,680,90+(48*i),cor);
-        desenha_texto(Pmotor->recorde[i].pontu,Pmotor,770,90+(48*i),cor2);
-    }
-
-    SDL_Flip(Pmotor->tela);
-
-    Mix_HaltMusic();
-    Mix_PlayMusic(Pmotor->musica_recordes,-1);
-
-    TTF_CloseFont(Pmotor->fonte);
-}
-
-void cria_novo_recorde(Motor* Pmotor,Paisagem* Pintro)
-{
-    dispose_intro(Pintro);
-    const SDL_VideoInfo *pinfo = NULL;
-    SDL_Surface* tmp = NULL;
-    pinfo = SDL_GetVideoInfo();
-
-    Pmotor->rodando=8;
-
-    int bpp = pinfo->vfmt->BitsPerPixel;
-
-    Pmotor->fonte = TTF_OpenFont("./fontes/ARCADEPI.ttf",100);
-
-    //tela
-    Pmotor->tela = NULL;
-    Pmotor->tela=SDL_SetVideoMode(LARGURA,ALTURA,bpp,SDL_HWSURFACE);
-    SDL_WM_SetCaption("recorde",NULL);
-    
-    Pintro->rect_paisagem.x=0;
-    Pintro->rect_paisagem.y=0;
-    Pintro->rect_paisagem.w=LARGURA;
-    Pintro->rect_paisagem.h=ALTURA;
-    
-    //paisagem
-    Pintro->paisagem = NULL;
-    tmp = IMG_Load("./imagens/nome.png");
-
-    Pintro->paisagem = SDL_DisplayFormat(tmp);
-
-    SDL_FreeSurface(tmp);
-    tmp = NULL;
-   
-    Mix_HaltMusic();
-    Mix_PlayMusic(Pmotor->musica_intro,-1);
-}
-
-{
-    dispose_fase(Pmotor,Pintro);
-
-    le_arquivo(Pmotor->recorde);
-
-    if(Pmotor->pontos > Pmotor->recorde[9].pontos)
-    {
-        Pmotor->recordista=1;
-    }
-
-    const SDL_VideoInfo *pinfo = NULL;
-    SDL_Surface* tmp = NULL;
-    pinfo = SDL_GetVideoInfo();
-
-    Pmotor->rodando=6;
-
-    int bpp = pinfo->vfmt->BitsPerPixel,i;
-
-    //tela
-    Pmotor->tela = NULL;
-    Pmotor->tela=SDL_SetVideoMode(LARGURA,ALTURA,bpp,SDL_HWSURFACE);
-    SDL_WM_SetCaption("vitoria",NULL);
-    
-    Pintro->rect_paisagem.x=0;
-    Pintro->rect_paisagem.y=0;
-    Pintro->rect_paisagem.w=LARGURA;
-    Pintro->rect_paisagem.h=ALTURA;
-    
-    //paisagem
-    Pintro->paisagem = NULL;
-    tmp = IMG_Load("./imagens/win.png");
-
-    Pintro->paisagem = SDL_DisplayFormat(tmp);
-
-    SDL_FreeSurface(tmp);
-    tmp = NULL;
-
-    //paisagem na tela
-    SDL_BlitSurface(Pintro->paisagem,NULL,Pmotor->tela,&Pintro->rect_paisagem);
-    
-    //flip
-    SDL_Flip(Pmotor->tela);
-
-    Mix_HaltMusic();
-    Mix_PlayMusic(Pmotor->musica_vitoria,-1);*/
