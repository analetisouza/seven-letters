#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
  int x, y;
  short life;
  char *name;
} Alice;

typedef struct {
  int x, y;
} Star; //objetos

typedef struct { //personagem
  Alice alice;
  Star stars[5]; //quantidade de objetos
  SDL_Texture *star;
  SDL_Renderer *renderer;
} GameState;

const int LARG = 1280; 
const int ALT = 720;

bool inicializador();
bool eventos(SDL_Window*, GameState*);
void doRender(SDL_Renderer*, GameState*);
void saida();
bool telainicial(SDL_Renderer*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*);
void loadGame(GameState *);
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
  GameState gameState;
  bool jogando = false; 


  if ( !inicializador() ) {
    printf("Falha ao inicializar!\n");
  }
  else {
    srandom((int)time(NULL));
    gameState.renderer = renderer;
    telainicial(renderer, background, jogar, niveis, opcoes, creditos, sair);
    //SDL_DestroyTexture(background);
    //SDL_DestroyTexture(jogar);
    jogando = true;
    loadGame(&gameState);
    gameState.star = loadTextura("media/star.png");
    while(jogando != false) {
      jogando = eventos(janela, &gameState);
      doRender(renderer, &gameState);
      SDL_Delay(10);
    }
  }
  SDL_DestroyTexture(gameState.star);
  saida();

  return 0;
}


bool eventos(SDL_Window *janela, GameState *gameState) {
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
          gameState->alice.y -= 5;
          break;
        default:
          break;
      }
    }
  }
  
const Uint8 *state = SDL_GetKeyboardState(NULL);
  if(state[SDL_SCANCODE_LEFT]) {
    gameState->alice.x -= 5;
  }
  if(state[SDL_SCANCODE_RIGHT]) {
    gameState->alice.x += 5;
  }
  if(state[SDL_SCANCODE_UP]) {
    gameState->alice.y -= 5;
  }
  if(state[SDL_SCANCODE_DOWN]) {
    gameState->alice.y += 5;
  }
  
  return jogando;
}

void doRender(SDL_Renderer *renderer, GameState *gameState) {
  int i;
  //set the drawing color to blue
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  
  SDL_RenderClear(renderer);
  
  //set the drawing color to white
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  //estrela
  SDL_Rect starRect = {50, 50, 64, 64};
  SDL_RenderCopy(renderer, gameState->star, NULL, &starRect);

  //retangulo
  SDL_Rect rect = { gameState->alice.x, gameState->alice.y, 80, 80 };
  SDL_RenderFillRect(renderer, &rect);

  for(i = 0; i < 5; i++) {
    SDL_Rect starRect = { gameState->stars[i].x, gameState->stars[i].y, 64, 64 };
    SDL_RenderCopy(renderer, gameState->star, NULL, &starRect);
  }

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
  SDL_Event evento;

    background = loadTextura("media/menu.png");
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL); //fundo

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
    
    /*while ( SDL_PollEvent (&evento) != 0) {
      if (evento.type == SDL_MOUSEMOTION) {
          if (evento.type == SDL_MOUSEBUTTONDOWN) {
              if (evento.button.x < 500 && evento.button.button == SDL_BUTTON_LEFT) {


          if (evento.button.button == SDL_BUTTON_LEFT) {
            currentImage = gTeclado[tecla_space];
          }
              if (evento.button.button == SDL_BUTTON_RIGHT) {
                currentImage = gTeclado[tecla_up];
              }
              if (evento.button.button == SDL_BUTTON_MIDDLE) {
                currentImage = gTeclado[tecla_padrao];
              }
            }*/
    SDL_Delay(5000);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(jogar);
    SDL_DestroyTexture(niveis);
    SDL_DestroyTexture(opcoes);
    SDL_DestroyTexture(creditos);
    SDL_DestroyTexture(sair);

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

void loadGame(GameState *gameState) {
  int i;
  SDL_Surface *starSurface = NULL;

  //Load images and create rendering textures from them
  starSurface = IMG_Load("media/star.png");
  if(starSurface == NULL) {
    printf("Cannot find star.png!\n\n");
    SDL_Quit();
    exit(1);
  }
  
  gameState->star = SDL_CreateTextureFromSurface(gameState->renderer, starSurface);
  SDL_FreeSurface(starSurface);

  gameState->alice.x = 320-40;
  gameState->alice.y = 240-40;

  //init stars
  for (i = 0; i < 5; i++) {
    gameState->stars[i].x = random()%LARG;
    gameState->stars[i].y = random()%ALT;
  }
}