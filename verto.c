#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
  float x, y;
  float dy;
  short life;
  char *name;
} Alice;

typedef struct {
  int x, y;
} Star; //objetos

typedef struct {
  int x, y, w, h;
} Ledge;

typedef struct { //personagem
  Alice alice; //players
  Star stars[5]; //quantidade de objetos
  Ledge ledges[5];

  SDL_Texture *star;
  SDL_Texture *aliceFrames[2];
  SDL_Texture *plataforma;

  SDL_Renderer *renderer;
} GameState;

const int LARG = 1280; 
const int ALT = 720;

bool inicializador();
bool eventos(SDL_Window*, GameState*);
void doRender(SDL_Renderer*, GameState*);
void RenderNivel(SDL_Renderer*, GameState*);
void RenderObjetos(SDL_Renderer*, GameState*);
void saida();
bool telainicial(SDL_Renderer*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*);
void trocar(SDL_Renderer*, int);
void loadGame(GameState*);
bool nivel1 (SDL_Renderer*);

SDL_Texture* loadTextura (const char *path); 


SDL_Window* janela = NULL;
SDL_Renderer* renderer = NULL;

SDL_Texture* background = NULL;
SDL_Texture* jogar = NULL;
SDL_Texture* niveis = NULL;
SDL_Texture* opcoes = NULL;
SDL_Texture* creditos = NULL;
SDL_Texture* sair = NULL;


int main(int argc, char *argv[]) {
  GameState game;
  bool jogando = false; 
  int nivel;

  if ( !inicializador() ) {
    printf("Falha ao inicializar!\n");
  }
  else {
    srandom((int)time(NULL));
    game.renderer = renderer;
    if ( !telainicial(renderer, background, jogar, niveis, opcoes, creditos, sair)) {
      saida();
      exit(1);
    }

    jogando = true;
    nivel = 1;
    nivel1(renderer);

    /*while(jogando != false) {
      jogando = eventos(janela, &game);
      //doRender(renderer, &game);
      SDL_Delay(20);
    }*/
}

  SDL_DestroyTexture(game.star);
  SDL_DestroyTexture(game.plataforma);
  SDL_DestroyTexture(game.aliceFrames[0]);
  SDL_DestroyTexture(game.aliceFrames[1]);
  saida();

  return 0;
}


bool eventos(SDL_Window *janela, GameState *game) {
  SDL_Event evento;
  bool jogando = true;

  while(SDL_PollEvent(&evento) != 0) {
    if (evento.type == SDL_QUIT) {
      jogando = false;
      break;
    }
    if (evento.type == SDL_KEYDOWN) {
      switch (evento.key.keysym.sym) {
        case (SDLK_LALT && SDLK_F4):
          jogando = false;
          break;
        case SDLK_ESCAPE: //ajeitar
          jogando = false;
          break;
        case SDLK_BACKSPACE:
          game->alice.y -= 5;
          break;
        default:
          break;
      }
    }
  }
  
const Uint8 *state = SDL_GetKeyboardState(NULL);
  if(state[SDL_SCANCODE_LEFT]) {
    game->alice.x -= 5;
  }
  if(state[SDL_SCANCODE_RIGHT]) {
    game->alice.x += 5;
  }
  if(state[SDL_SCANCODE_UP]) {
    game->alice.y -= 5;
  }
  if(state[SDL_SCANCODE_DOWN]) {
    game->alice.y += 5;
  }
  
  return jogando;
}

void doRender(SDL_Renderer *renderer, GameState *game) {
  int i;

  for(i = 0; i < 5; i++) {
    SDL_Rect starRect = { game->stars[i].x, game->stars[i].y, 64, 64 };
    SDL_RenderCopy(renderer, game->star, NULL, &starRect);
  }

  for(i = 0; i < 5; i++) {
    SDL_Rect ledgeRect = { game->ledges[i].x, game->ledges[i].y, game->ledges[i].w, game->ledges[i].h };
    SDL_RenderCopy(renderer, game->plataforma, NULL, &ledgeRect);
  }

  SDL_Rect rect = { game->alice.x, game->alice.y, 64, 64 };
  SDL_RenderCopyEx(renderer, game->aliceFrames[0], NULL, &rect, 0, NULL, 0);

  SDL_RenderPresent(renderer);

}


bool inicializador () {
  bool sucesso = true;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) { //-1 dá problema
    printf("O programa não pode ser inicializado! SDL_ERROR: %s\n", SDL_GetError());
    sucesso = false;
  }
  else {
    srandom((int)time(NULL));
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

  IMG_Quit();
  SDL_Quit();

}


bool telainicial (SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *jogar, SDL_Texture *niveis, SDL_Texture *opcoes, SDL_Texture *creditos, SDL_Texture *sair) {
  bool sucesso = true;
  int num = 0;
  SDL_Texture *imgNiveis = NULL;
  SDL_Texture *imgCreditos = NULL;
  SDL_Texture *imgOpcoes = NULL;

    background = loadTextura("media/menu.png");
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
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

    opcoes = loadTextura("media/opcoes1.png");
    SDL_Rect opcRect = {770, 392, 148, 57};
    SDL_RenderCopy(renderer, opcoes, NULL, &opcRect);

    sair = loadTextura("media/sair1.png");
    SDL_Rect sairRect = {780, 490, 98, 41};
    SDL_RenderCopy(renderer, sair, NULL, &sairRect);

    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool gameloop = true;
  while (gameloop == true) {
    while ( SDL_PollEvent (&event) ) {
      if (event.type == SDL_QUIT) {
        gameloop = false;
        sucesso = false;
        break;
      }
      else {
      switch (event.type) {
        case SDL_MOUSEMOTION:
          case SDL_MOUSEBUTTONDOWN:
            if (event.motion.x > jogarRect.x && event.motion.x < jogarRect.x + jogarRect.w && event.motion.y > jogarRect.y && event.motion.y < jogarRect.y + jogarRect.h && event.button.button == SDL_BUTTON_LEFT) {
                num = 1;
                trocar(renderer, num);
                SDL_Delay(500);
                gameloop = false;
            }

            if (event.motion.x > niveisRect.x && event.motion.x < niveisRect.x + niveisRect.w && event.motion.y > niveisRect.y && event.motion.y < niveisRect.y + niveisRect.h && event.button.button == SDL_BUTTON_LEFT) {
              num = 2;
              trocar(renderer, num);
              SDL_Delay(500);
              imgNiveis = loadTextura("media/creditos_teste.png"); //
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgNiveis, NULL, NULL);
              SDL_RenderPresent(renderer);
              SDL_Delay(3000);
              gameloop = false;
              telainicial(renderer, background, jogar, niveis, opcoes, creditos, sair);
              //gameloop = false;
            }

            if (event.motion.x > credRect.x && event.motion.x < credRect.x + credRect.w && event.motion.y > credRect.y && event.motion.y < credRect.y + credRect.h && event.button.button == SDL_BUTTON_LEFT) {
              num = 3;
              trocar(renderer, num);
              SDL_Delay(500);
              imgCreditos = loadTextura("media/creditos_teste.png"); 
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgCreditos, NULL, NULL);
              SDL_RenderPresent(renderer);
              SDL_Delay(3000);
              gameloop = false;
              telainicial(renderer, background, jogar, niveis, opcoes, creditos, sair);
              //gameloop = false;
            }

            if (event.motion.x > opcRect.x && event.motion.x < opcRect.x + opcRect.w && event.motion.y > opcRect.y && event.motion.y < opcRect.y + opcRect.h && event.button.button == SDL_BUTTON_LEFT) {
              num = 4;
              trocar(renderer, num);
              SDL_Delay(500);
              imgOpcoes = loadTextura("media/creditos_teste.png"); //
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgOpcoes, NULL, NULL);
              SDL_RenderPresent(renderer);
              SDL_Delay(3000);
              gameloop = false;
              telainicial(renderer, background, jogar, niveis, opcoes, creditos, sair);
              //gameloop = false;
            }

            if (event.motion.x > sairRect.x && event.motion.x < sairRect.x + sairRect.w && event.motion.y > sairRect.y && event.motion.y < sairRect.y + sairRect.h && event.button.button == SDL_BUTTON_LEFT) {
              num = 5;
              trocar(renderer, num);
              SDL_Delay(500);               
              gameloop = false;
              sucesso = false;
              exit(1);
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
    SDL_DestroyTexture(opcoes);
    SDL_DestroyTexture(creditos);
    SDL_DestroyTexture(sair);
    SDL_DestroyTexture(imgNiveis);
    SDL_DestroyTexture(imgCreditos);
    SDL_DestroyTexture(imgOpcoes);
    SDL_RenderClear(renderer);

  return sucesso;

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

void RenderNivel(SDL_Renderer *renderer, GameState *game) {
  SDL_Texture *fundo = NULL;
  SDL_Texture *barra = NULL;
  SDL_Texture *vida = NULL;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    fundo = loadTextura("media/mapa.png");
    SDL_RenderCopy(renderer, fundo, NULL, NULL);

    barra = loadTextura("media/base_barra.png");
    SDL_Rect barraRect = {0, 0, 1280, 109};
    SDL_RenderCopy(renderer, barra, NULL, &barraRect);

    //if (alice.vida == 1)
    vida = loadTextura("media/coracao_vida.png");
    SDL_Rect vidaRect = {180, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vidaRect);

    //if (alice.vida == 2)
    //SDL_RenderCopy(renderer, vida, NULL, &vidaRect);
    SDL_Rect vida2Rect = {245, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vida2Rect);

    //if (alice.vida == 3) 
    SDL_Rect vida3Rect = {310, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vida3Rect);

}

void RenderObjetos(SDL_Renderer *renderer, GameState *game) {
  int i;

  for(i = 0; i < 5; i++) {
    SDL_Rect starRect = { game->stars[i].x, game->stars[i].y, 64, 64 };
    SDL_RenderCopy(renderer, game->star, NULL, &starRect);
  }

  for(i = 0; i < 5; i++) {
    SDL_Rect ledgeRect = { game->ledges[i].x, game->ledges[i].y, game->ledges[i].w, game->ledges[i].h };
    SDL_RenderCopy(renderer, game->plataforma, NULL, &ledgeRect);
  }

  SDL_Rect rect = { game->alice.x, game->alice.y, 64, 64 };
  SDL_RenderCopyEx(renderer, game->aliceFrames[0], NULL, &rect, 0, NULL, 0);

}


void loadGame(GameState *game) {
  int i;
  SDL_Surface *surface = NULL;

  surface = IMG_Load("media/star.png");
  if(surface == NULL) {
    printf("Não foi possivel encontrar o arquivo 'star.png'!\n");
  }
  game->star = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);


  surface = IMG_Load("media/default.png");
  if(surface == NULL) {
    printf("Não foi possivel encontrar o arquivo 'default.png'!\n");
  }
  game->aliceFrames[0] = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);


  surface = IMG_Load("media/andando.png");
  if(surface == NULL) {
    printf("Não foi possivel encontrar o arquivo 'andando.png'!\n");
  }
  game->aliceFrames[1] = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);


  surface = IMG_Load("media/plataforma.png");
  if(surface == NULL) {
    printf("Não foi possivel encontrar o arquivo 'plataforma.png'!\n");
  }
  game->plataforma = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);

  game->alice.x = 320-40;
  game->alice.y = 240-40;

  //init stars
  for (i = 0; i < 5; i++) {
    game->stars[i].x = random()%LARG;
    game->stars[i].y = random()%ALT;
  }

  //init ledges
  for(int i = 0; i < 5; i++) {
    game->ledges[i].x = i*100;
    game->ledges[i].y = 400;
    game->ledges[i].w = 256;
    game->ledges[i].h = 64;
  }

  game->ledges[4].x = 100;
  game->ledges[4].y = 200;
  
  game->ledges[3].x = 350;
  game->ledges[3].y = 350;

}


void trocar (SDL_Renderer *renderer, int num) {

  if (num == 1) {
    SDL_Texture *jogar2 = NULL;
    SDL_Rect jogarRect = {378, 397, 124, 50};
    jogar2 = loadTextura("media/jogar3.png");
    SDL_RenderCopy(renderer, jogar2, NULL, &jogarRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(jogar2);
  }

  if (num == 2) {
    SDL_Texture *niveis2 = NULL;
    SDL_Rect niveisRect = {385, 490, 146, 53};
    niveis2 = loadTextura("media/niveis3.png");
    SDL_RenderCopy(renderer, niveis2, NULL, &niveisRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(niveis2);
  }

   if (num == 3) {
    SDL_Texture *cred2 = NULL;
    SDL_Rect credRect = {360, 587, 196, 57};
    cred2 = loadTextura("media/creditos3.png");
    SDL_RenderCopy(renderer, cred2, NULL, &credRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(cred2);
  }

  if (num == 4) {
    SDL_Texture *opcoes2 = NULL;
    SDL_Rect opcRect = {770, 392, 148, 57};
    opcoes2 = loadTextura("media/opcoes3.png");
    SDL_RenderCopy(renderer, opcoes2, NULL, &opcRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(opcoes2);
  }

  if (num == 5) {
    SDL_Texture *sair2 = NULL;
    SDL_Rect sairRect = {780, 490, 98, 41};
    sair2 = loadTextura("media/sair3.png");
    SDL_RenderCopy(renderer, sair2, NULL, &sairRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(sair2);
  }

}


bool nivel1(SDL_Renderer *renderer) {
  bool sucesso = true;
  bool jogando = true;
  GameState game;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);  //tempo para carregar o jogo
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  SDL_Delay(50);

  loadGame(&game);
  game.star = loadTextura("media/star.png");
  game.plataforma = loadTextura("media/plataforma.png");
  game.aliceFrames[0] = loadTextura("media/andando.png");

    while(jogando != false) {
      RenderNivel(renderer, &game);
      SDL_RenderClear(renderer);

      RenderNivel(renderer, &game);
      RenderObjetos(renderer, &game);
      jogando = eventos(janela, &game);
      SDL_RenderPresent(renderer);
      SDL_Delay(10);
    }

    SDL_Delay(1000);
    SDL_DestroyTexture(game.star);
    SDL_DestroyTexture(game.plataforma);
    SDL_DestroyTexture(game.aliceFrames[0]);
    SDL_DestroyTexture(game.aliceFrames[1]);

    return sucesso;
    
}
