int moveX,moveY; //inteiros para o mouse(substituir se já tiver)

// gameloop
while(gaming = 1) // loop
{
    //codigo

    while(SDL_PolEvent(&evento))
    {
        if(menu == //menu do jogo ou menu de pausa)    
            if(evento.type == SDL_MOUSEMOTION)
            {
                moveX = evento.motion.x;
                moveY = evento.motion.y;
            }

            if(evento.type == SDL_MOUSEBUTTONDOWN)
            {
                if(move.X >= coordenada && move.Y <= coordenada && moveY >= coordenada && moveY <= coordenada && evento.button.button == SDL_BUTTON_LEFT)
                {
                    Mix_HaltChannel(game.musicChannel);
                    Mix_FreeChuck(game.MENU);
                }
            }
    }
}