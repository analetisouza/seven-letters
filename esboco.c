#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

const int larg = 1280; //16:9
const int alt = 720;

enum teclado {
	tecla_padrao,
	tecla_cima,
	tecla_baixo,
	tecla_esquerda,
	tecla_direita,
	tecla_espaco,
	tecla_total
};

bool inicializador();

bool carregamento();

bool fases();

void saida();

SDL_Surface* loadSurface( const char *path );
SDL_Surface* loadOtimizada( const char *path ) ;
SDL_Window* janela = NULL;
SDL_Surface* windowSurface = NULL;
SDL_Surface* imageSurface = NULL;
SDL_Surface* currentImage = NULL;
SDL_Surface* gTeclado[tecla_total];
SDL_Surface* gPNGSurface = NULL;

bool inicializador () {
	bool sucesso = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) { //-1 dá problema
		printf("O programa não pode ser inicializado! SDL_ERROR: %s\n", SDL_GetError());
		sucesso = false;
	}
	else {
		janela = SDL_CreateWindow("7 letters from my love", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, larg, alt, SDL_WINDOW_SHOWN);
		if (janela == NULL) {
			printf("A janela não pode ser criada! SDL_ERROR: %s\n", SDL_GetError());
			sucesso = false;
		}
		else {
			int imgFlags = IMG_INIT_PNG;
			if( !(IMG_Init(imgFlags) & imgFlags) ) {
				printf( "SDL_image não pode inicializar! SDL_image Error: %s\n", IMG_GetError() );
				sucesso = false;
			}
			else {
				windowSurface = SDL_GetWindowSurface(janela);
			}
		}
	}

	return sucesso;
}


bool carregamento () { 
	bool sucesso = true;

	gTeclado[tecla_padrao] = loadSurface( "default.png" );
	if( gTeclado[tecla_padrao] == NULL ) {
		printf("Falha ao carregar a imagem padrao!\n");
		sucesso = false;
	}

	gTeclado[tecla_cima] = loadSurface("fotos/Tile_12.png");
	if( gTeclado[tecla_cima] == NULL ) {
		printf("Falha ao carregar a imagem cima!\n");
		sucesso = false;
	}

	gTeclado[tecla_baixo] = loadSurface("fotos/Tile_11.png");
	if( gTeclado[tecla_baixo] == NULL ) {
		printf("Falha ao carregar a imagem baixo!\n");
		sucesso = false;
	}

	gTeclado[tecla_direita] = loadSurface("andando.png");
	if( gTeclado[tecla_direita] == NULL ) {
		printf("Falha ao carregar a imagem direita!\n");
		sucesso = false;
	}

	gTeclado[tecla_esquerda] = loadSurface("sor.bmp");
	if( gTeclado[tecla_esquerda] == NULL ) {
		printf("Falha ao carregar a imagem esquerda!\n");
		sucesso = false;
	}

	gTeclado[tecla_espaco] = loadSurface("Old hero.bmp");
	if( gTeclado[tecla_espaco] == NULL ) {
		printf("Falha ao carregar a imagem espaco!\n");
		sucesso = false;
	}

	return sucesso;
}


void saida () {
	int i;
	for(i = 0; i < tecla_total; i++) {
		SDL_FreeSurface(gTeclado[i]);
		gTeclado[i] = NULL;
	}

	//SDL_FreeSurface(currentImage);
	//currentImage = NULL;

	//SDL_FreeSurface(imageSurface);
	//imageSurface = NULL;

	SDL_FreeSurface(windowSurface);
	SDL_DestroyWindow(janela);
	janela = NULL;
	windowSurface = NULL;

	IMG_Quit();
	SDL_Quit();	

}


SDL_Surface* loadSurface(const char *path) {
	SDL_Surface* loadedSurface = IMG_Load(path);

	if(loadedSurface == NULL) {
		printf( "Não foi possível carregar a imagem %s! SDL Error: %s\n", path, IMG_GetError() );
	}

	return loadedSurface;
}

SDL_Surface* loadOtimizada(const char *path) {
	SDL_Surface* surfaceOtimizada = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path);

	if(loadedSurface == NULL) {
		printf( "Não foi possível carregar a imagem %s! SDL Error: %s\n", path, IMG_GetError() );
	}
	else {
		surfaceOtimizada = SDL_ConvertSurface( loadedSurface, windowSurface->format, 0 );
		if( surfaceOtimizada == NULL ) {
			printf( "Não foi possível otimizar a imagem de fundo%s! SDL Error: %s\n", path, SDL_GetError() );
		}

		SDL_FreeSurface(loadedSurface);
	}

	return surfaceOtimizada;
}

bool fases () {
	bool sucesso = true;
	int fase = 1;
	if (fase == 1) {
		imageSurface = loadOtimizada( "Background.bmp" );
		SDL_Rect stretchRect;
		stretchRect.x = 0;
		stretchRect.y = 0;
		stretchRect.w = larg;
		stretchRect.h = alt;
		SDL_BlitScaled( imageSurface, NULL, windowSurface, &stretchRect );
	}

	//fase++;
	return sucesso;
}


int main (int argc, char *argv[]) {
	int i;

	if ( !inicializador() ) {
		printf("Falha ao inicializar!\n");
	}
	else {
		if( !carregamento() ) {
			printf("Falha ao carregar midias!\n");
		}
		else {
			bool gameloop = true;
			SDL_Event evento;
			currentImage = gTeclado[tecla_padrao];
			
			while (gameloop == true) {
				if ( !fases() ) {
					printf("Falha ao carregar fase 1!\n");
				}
				while ( SDL_PollEvent (&evento) != 0) {
						if (evento.type == SDL_QUIT || evento.type == (SDLK_LALT && SDLK_F4)) {
							gameloop = false;
							break;
						}
						else {
							if (evento.type == SDL_KEYDOWN) {
								switch (evento.key.keysym.sym) {
										case SDLK_UP:
											currentImage = gTeclado[tecla_cima];
											break;
										case SDLK_DOWN:
											currentImage = gTeclado[tecla_baixo];
											break;
										case SDLK_LEFT:
											currentImage = gTeclado[tecla_esquerda];
											break;
										case SDLK_RIGHT:
											currentImage = gTeclado[tecla_direita];
											break;
										case SDLK_BACKSPACE:
											currentImage = gTeclado[tecla_espaco];
											break;
										default:
											currentImage = gTeclado[tecla_padrao];
											break;
								}
							}
						}
				}
					SDL_BlitSurface( currentImage, NULL, windowSurface, NULL );
					SDL_UpdateWindowSurface(janela);
				}
			}
		}

	saida();

	return 0;
}