#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define Gravidade 6.0f
#define STATUS_STATE_LIVES 0
#define STATUS_STATE_GAME 1
#define STATUS_STATE_GAMEOVER 2

typedef struct {
  float x, y;
  float dx, dy;
  short lives;
  int pontos;
  char *name;
  int onPlat;

  int animFrame, facingLeft, slowingDown;
} Alice;


typedef struct {
  float x, y, w, h;
} PLAT;

typedef struct {
  float x, y;
} INIMIGO;

typedef struct { //personagem
  Alice alice; //players
  PLAT plat[100];
  INIMIGO inim;

  SDL_Texture *aliceFrames[4]; //alterar pra não dar erro de seg
  SDL_Texture *plataforma;
  SDL_Texture *inimFrames[4];
  SDL_Texture *label;
  int labelW, labelH;

  TTF_Font *font;

  int time;
  int statusState;

  SDL_Renderer *renderer;
} GameState;

const int LARG = 1280; 
const int ALT = 720;

bool inicializador();
bool eventos(SDL_Window*, GameState*);
void RenderNivel(SDL_Renderer*, GameState*);
void RenderObjetos(SDL_Renderer*, GameState*);
void saida();
bool telainicial(SDL_Renderer*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*);
void trocar(SDL_Renderer*, int);
void loadGame(GameState*);
bool nivel1 (SDL_Renderer*);
void colisaoplat(GameState*);
void colisaoinim(GameState*);
void processo(GameState*);
void init_status_lives(GameState*);
void draw_status_lives(GameState*);
void shutdown_status_lives(GameState*);


void init_status_lives(GameState* game) {
  SDL_Color white = {255, 255, 255, 255};

  /*SDL_Surface *tmp = TTF_RenderText_Blended(game->font, "Words", white);
  game->labelW = tmp->w;
  game->labelH = tmp->h;
  game->label = SDL_CreateTextureFromSurface(game->renderer, tmp);
  SDL_FreeSurface(tmp);*/

}

void draw_status_lives(GameState* game) {
  //SDL_Renderer *renderer = game->renderer;

  SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
  SDL_RenderClear(game->renderer);
  SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
  SDL_Rect textRect = {320, 240-game->labelH, game->labelW, game->labelH};
  SDL_RenderCopy(game->renderer, game->label, NULL, &textRect);

}

void shutdown_status_lives(GameState* game) {
  SDL_DestroyTexture(game->label);
  game->label = NULL;
}

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
    //srandom((int)time(NULL));
    game.renderer = renderer;
    if ( !telainicial(renderer, background, jogar, niveis, opcoes, creditos, sair)) {
      saida();
      IMG_Quit();
      SDL_Quit();
      exit(1);
    }

    jogando = true;
    nivel = 1;
    nivel1(renderer);

}

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
        case SDLK_SPACE:
          if (game->alice.onPlat == 1) { //só se tiver no chão
            game->alice.dy = -27;
            game->alice.onPlat = 0;
          }
          break;
        //default:
          //break;
      }
    }
  }
  
const Uint8 *state = SDL_GetKeyboardState(NULL); //arrumar
  if(state[SDL_SCANCODE_LEFT]) {
    if (game->alice.x - 6 < 0) {
      game->alice.x = 0;
    }
    else {
      game->alice.x -= 5;
    }

    game->alice.dx -= 0.5; //aceleração

    if(game->alice.dx < -5) {
      game->alice.dx = -5;
    }
    game->alice.facingLeft = 1;
    game->alice.slowingDown = 0;
  }

  else if(state[SDL_SCANCODE_RIGHT]) {
    if ((game->alice.x + 84 + 6) > LARG) {
      game->alice.x = LARG - 84;
    }
    else {
      game->alice.x += 5;
    }

    game->alice.dx += 0.5; //aceleração
    if(game->alice.dx > 5) {
      game->alice.dx = 5;
    }
    game->alice.facingLeft = 0;
    game->alice.slowingDown = 0;
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
        TTF_Init();
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

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();

}


bool telainicial (SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *jogar, SDL_Texture *niveis, SDL_Texture *opcoes, SDL_Texture *creditos, SDL_Texture *sair) {
  bool sucesso = true;
  int num = 0;
  SDL_Texture *imgNiveis = NULL;
  SDL_Texture *imgCreditos = NULL;
  SDL_Texture *imgRanking = NULL;

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
                SDL_Delay(300);
                gameloop = false;
            }

            if (event.motion.x > niveisRect.x && event.motion.x < niveisRect.x + niveisRect.w && event.motion.y > niveisRect.y && event.motion.y < niveisRect.y + niveisRect.h && event.button.button == SDL_BUTTON_LEFT) {
              num = 2;
              trocar(renderer, num);
              SDL_Delay(300);
              imgNiveis = loadTextura("media/menu_niveis.png"); //
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
              SDL_Delay(300);
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
              SDL_Delay(300);
              imgRanking = loadTextura("media/ranking.png"); //
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgRanking, NULL, NULL);
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
    SDL_DestroyTexture(imgRanking);
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
  SDL_Color preto = {0, 0, 0, 0};
  SDL_Texture *fundo = NULL;
  SDL_Texture *barra = NULL;
  SDL_Texture *vida = NULL;
  SDL_Texture *placa = NULL;
  SDL_Texture *pause = NULL;
  SDL_Texture *Nivel1 = NULL;

    if (game->statusState == STATUS_STATE_LIVES) {
      draw_status_lives(game);
    }
    else if (game->statusState == STATUS_STATE_GAME) {

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    fundo = loadTextura("media/mapa.png");
    SDL_Rect fundoRect = {0, 0, 1280, 720};
    SDL_RenderCopy(renderer, fundo, &fundoRect, NULL);

    barra = loadTextura("media/base_barra.png");
    SDL_Rect barraRect = {0, 0, 1280, 109};
    SDL_RenderCopy(renderer, barra, NULL, &barraRect);

    //caso queira inserir algum texto
    char str[128] = "";
    sprintf (str, "%d", (int)game->alice.pontos);
    SDL_Surface *tmp = TTF_RenderText_Blended(game->font, str, preto); 
    game->labelW = tmp->w;
    game->labelH = tmp->h;
    game->label = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect textRect = {965, 60-game->labelH, game->labelW, game->labelH};
    SDL_RenderCopy(renderer, game->label, NULL, &textRect);
    //game->alice.pontos++; 

    Nivel1 = loadTextura("media/nivel1.png");
    SDL_Rect nivel1Rect = {560, 35, 153, 41};
    SDL_RenderCopy(renderer, Nivel1, NULL, &nivel1Rect);

    //if (alice.vida == 1)
    vida = loadTextura("media/coracao_vida.png");
    SDL_Rect vidaRect = {180, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vidaRect);

    placa = loadTextura("media/signRight.png");
    SDL_Rect placRect = { 120, 570, 70, 100 };
    SDL_RenderCopy(renderer, placa, NULL, &placRect);

    pause = loadTextura("media/botao_pausa.png");
    SDL_Rect pauseRect = { 1100, 15, 74, 75 };
    SDL_RenderCopy(renderer, pause, NULL, &pauseRect);

    //if (alice.vida == 2)
    //SDL_RenderCopy(renderer, vida, NULL, &vidaRect);
    SDL_Rect vida2Rect = {245, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vida2Rect);

    //if (alice.vida == 3) 
    SDL_Rect vida3Rect = {310, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vida3Rect);


    SDL_DestroyTexture(fundo);
    SDL_DestroyTexture(barra);
    SDL_DestroyTexture(vida);
    SDL_DestroyTexture(placa);
    SDL_DestroyTexture(pause);
    SDL_DestroyTexture(Nivel1);


  }
    
}

void RenderObjetos(SDL_Renderer *renderer, GameState *game) {
  int i;
  if (game->statusState == STATUS_STATE_LIVES) {
    draw_status_lives(game);
  }
  else if (game->statusState == STATUS_STATE_GAME) {
  for (i = 0; i < 13; i++) { //quantidades de plataformas que existirão
    SDL_Rect platRect = { game->plat[i].x, game->plat[i].y, 128, 30 };
    SDL_RenderCopy(renderer, game->plataforma, NULL, &platRect);
  }

  //inimigo arrumar
  SDL_Rect inimRect = { game->inim.x, game->inim.y, 50, 28 };
  SDL_RenderCopyEx(renderer, game->inimFrames[0], NULL, &inimRect, 0, NULL, 0);

  //Alice
  SDL_Rect rect = { game->alice.x, game->alice.y, 68, 118 };
  SDL_RenderCopyEx(renderer, game->aliceFrames[game->alice.animFrame], NULL, &rect, 0, NULL, (game->alice.facingLeft == 1));
  }
}


void loadGame(GameState *game) {
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

  init_status_lives(game);

  game->time = 0;

  //posição das plataformas 
  for (i = 0; i < 10; i++) { //do chão
    game->plat[i].x = 0+(128*i);
    game->plat[i].y = 675;
  }

  game->plat[10].x = 300;
  game->plat[10].y = 600;

  game->plat[11].x = 450;
  game->plat[11].y = 500;

  game->plat[12].x = 300;
  game->plat[12].y = 400;

  game->inim.x = 700;
  game->inim.y = 644;
  //game->inim.w = 50;
  //game->inim.h = 28;


  /*posição aleatoria de stars, caso queira inserir algo aleatório
  for (i = 0; i < 5; i++) {
    game->stars[i].x = random()%LARG;
    game->stars[i].y = random()%ALT;
  }*/

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


bool telapause(SDL_Renderer *renderer){
  bool sucesso = true;

}


bool nivel1(SDL_Renderer *renderer) {
  bool sucesso = true;
  bool jogando = true;
  GameState game;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);  //tempo para carregar o jogo
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  SDL_Delay(100);

  loadGame(&game);
  game.font = TTF_OpenFont("media/TravelingTypewriter.ttf", 30);
  game.plataforma = loadTextura("media/plataforma.png");
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
      colisaoplat(&game);
      SDL_RenderPresent(renderer);
      SDL_Delay(1000/60);

    }

    SDL_Delay(500);

  SDL_DestroyTexture(game.plataforma);
  SDL_DestroyTexture(game.aliceFrames[0]);
  SDL_DestroyTexture(game.aliceFrames[1]); 
  SDL_DestroyTexture(game.inimFrames[0]);
  SDL_DestroyTexture(game.inimFrames[1]); 
  if(game.label != NULL) {
    SDL_DestroyTexture(game.label);
  }
  TTF_CloseFont(game.font);


    return sucesso;
    
}

void processo(GameState *game) {
  //add time
  game->time++;

  if(game->time > 120) {
    shutdown_status_lives(game);
    game->statusState = STATUS_STATE_GAME;
  }

  if (game->statusState == STATUS_STATE_GAME) {
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
  int velX = 3;
  INIMIGO *inim = &game->inim;

  inim->x -= velX;
  if (inim->x + 3 < 0) {  //vai só pra esquerda
    inim->x = LARG + 3;
  }
}

 //vai só pra esquerda
  /*if (i == 0) {
  if (inim->x - 3 < 0) {
      inim->x = 0;
      i = 1;
  }
  else {
      inim->x -= velX;
  }
}
  if (i == 1) {
    if (inim->x + 50 + 3 > LARG) {
      inim->x = LARG - 50;
      i = 0;
    }
    else {
      inim->x -= velX;
    }
  }*/


}


void colisaoplat(GameState *game) {
  int i;

  for (i = 0; i < 15; i++) {
  float aw = 68, ah = 112; //largura e altura -3 pra ficar no chao da alice;
  float ax = game->alice.x, ay = game->alice.y; //posição da alice

  float pw = 128, ph = 29; //largura e altura da plat;
  float px = game->plat[i].x, py = game->plat[i].y; //posição da plat;

  if (px < ax+aw/2 && ax+aw/2 < px+pw) {
      //are we bumping our head?
    if (ay < py+ph && ay > py && game->alice.dy < 0) {
        //correct y
        game->alice.y = py + ph;
        ay = py + ph; 
        //bumped our head, stop any jump velocity
        game->alice.dy = 0;
        game->alice.onPlat = 1;
    }
  }

  if (px < ax+aw && ax < px+pw) {
    if (ay+ah > py && ay < py && game->alice.dy > 0) {
      //correct y
      game->alice.y = py - ah;
      ay = py - ah;
      //landed on this ledge, stop any jump velocity
      game->alice.dy = 0;
      game->alice.onPlat = 1;
    }
  }

      if(ay+ah > py && ay<py+ph) {
      //rubbing against right edge
      if(ax < px+pw && ax+aw > px+pw && game->alice.dx < 0) {
        //correct x
        game->alice.x = px+pw;
        ax = pw+pw;

        game->alice.dx = 0;
      }
      //rubbing against left edge
      else if(ax+aw > px && ax < px && game->alice.dx > 0)
      {
        //correct x
        game->alice.x = px-aw;
        ax = px-aw;
        
        game->alice.dx = 0;
      }
    }
  //arrumar esssa parte pq tá teleportando
  /*if (py < ay+ah && ay < py+ph) {
    //rubbing against right edge
    if (ax < px+pw && ax+aw > px+pw && game->alice.dx < 0) {
      //correct x
      game->alice.x = px + pw;
      ax = px + pw;

      game->alice.dx = 0;
    }
    else {
      //rubbing against left edge
      if (px < ax+aw && ax < px && game->alice.dx > 0) {
        //correct x
        //game->alice.x -= 1;
        game->alice.x = px-aw;
        ax = px-aw;
        
        game->alice.dx = 0;
      }
    }
  }*/
  }
}

/*void colisaoplat(GameState *game) {
  float aw = 84, ah = 144; //largura e altura da alice;
  float ax = game->alice.x, ay = game->alice.y; //posição da alice

  float pw = 50, ph = 28; 
  float px = game->inim.x, py = game->inim.y; 

  if ()
}*/
