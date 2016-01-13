#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define SPEED_RATE 1
#define TIMER 40
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define FRAME_SIZE	10
#define SCREEN_X_SIZE SCREEN_WIDTH/FRAME_SIZE
#define SCREEN_Y_SIZE SCREEN_HEIGHT/FRAME_SIZE

char world[SCREEN_X_SIZE][SCREEN_Y_SIZE][4];
int x = 0, old_x = 0, y = 0, old_y = 0, snake_size = 50, actual_size = 1;

enum world_content
{
    background,
    wall,
    snake
};

void draw_frame(SDL_Surface *s, int x, int y, enum world_content content)
{
    SDL_Rect pos = {FRAME_SIZE*x, FRAME_SIZE*y, FRAME_SIZE, FRAME_SIZE};
    Uint8 red, blue, green;
    switch(content)
    {
	case background : red = 0, green = 255, blue = 0;
		break;
	case wall : red = 255, green = 0, blue = 0;
	        break;
	case snake : red = 0, green = 0, blue = 255;
		break;
    }
    if(SDL_FillRect(s, &pos, SDL_MapRGB(s->format, red, blue, green)) == -1)
    {
        printf("Can't fill rect : %s\n", SDL_GetError());
    }
}

void draw_world_frame(SDL_Surface *s, int x, int y, char content)
{
    if(content == 0)
	draw_frame(s, x, y, background);
    else if(content == 1)
        draw_frame(s, x, y, wall);
    else if(content == 2)
        draw_frame(s, x, y, snake);
}

void draw_world(SDL_Surface *s, char world[SCREEN_X_SIZE][SCREEN_Y_SIZE][4])
{
    int x, y;
    for(x = 0; x < SCREEN_X_SIZE; x++)
    {
	for(y = 0; y < SCREEN_Y_SIZE; y++)
	    draw_world_frame(s, x, y, 0);
    }
}

int move_snake(SDL_Surface *s, int xspeed, int yspeed)
{
    static timer = 0;
    if(timer < TIMER)
    {
	timer++;
	return 0;
    }
    timer = 0;
/*
[0] => droite/gauche => 0 : xspeed > 0, 1 : xspeed < 0
[1] => bas/haut => 0 : yspeed > 0, 1 : yspeed < 0
[2] => ver/pas de ver => 0 : pas de ver, 1 : ver
*/
    if(actual_size == snake_size)
    {
        int new_old_x = old_x, new_old_y = old_y;
	if(world[old_x][old_y][0] > 0)
	{
	    new_old_x = old_x + 1;
	    if(new_old_x > SCREEN_X_SIZE)
		new_old_x = 0;
	}
	else if(world[old_x][old_y][0] < 0)
	{
	    new_old_x = old_x - 1;
	    if(new_old_x < 0)
		new_old_x = SCREEN_X_SIZE;
	}
	if(world[old_x][old_y][1] > 0)
	{
	    new_old_y = old_y + 1;
	    if(new_old_y > SCREEN_Y_SIZE)
		new_old_y = 0;
	}
	else if(world[old_x][old_y][1] < 0)
	{
	    new_old_y = old_y - 1;
	    if(new_old_y < 0)
		new_old_y = SCREEN_Y_SIZE;
	}
	printf("move ass : [%d, %d] => [%d, %d]\n", old_x, old_y, new_old_x, new_old_y);
        draw_world_frame(s, old_x, old_y, 0);
        world[old_x][old_y][0] = 0;
        world[old_x][old_y][1] = 0;
        world[old_x][old_y][2] = 0;
        old_x = new_old_x;
        old_y = new_old_y;
    }
    else
	actual_size++;
    world[x][y][0] = xspeed;
    world[x][y][1] = yspeed;
    world[x][y][2] = 2;
    x += xspeed;
    if(x >= SCREEN_X_SIZE)
	x = 0;
    if(x < 0)
	x = SCREEN_X_SIZE;
    y += yspeed;
    if(y >= SCREEN_Y_SIZE)
	y = 0;
    if(y < 0)
	y = SCREEN_Y_SIZE;

    if(world[x][y][2] != 0)
	return -1;
    draw_world_frame(s, x, y, 2);
    return 0;
}

int main( int argc, char *argv[ ] )
{
    memset(world, 0, sizeof(world));
    SDL_Window *window;
    SDL_Surface *screen;
    if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
    {
        printf( "Can't init SDL:  %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }

    atexit( SDL_Quit ); 
    window = SDL_CreateWindow("Ma fenêtre de jeu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0 );
    screen = SDL_GetWindowSurface(window);

    if( screen == NULL )
    {
        printf( "Can't set video mode: %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }   

    int xspeed=0, yspeed=0;
    // Main loop
    SDL_Event event;
    draw_world(screen, world);
    while(1)
    {
	if(xspeed != 0 || yspeed != 0)
	{
            if(move_snake(screen, xspeed, yspeed) == -1)
	    {
		printf("Game Over !!!\n");
		return EXIT_SUCCESS;// Quit the program
                break;
	    }
	}

       // Check for messages
        if (SDL_PollEvent(&event))
        {
            // Check for the quit message
            switch (event.type)
            {
                case SDL_QUIT:
                    SDL_Quit();
		    return EXIT_SUCCESS;// Quit the program
                    break;
		case SDL_KEYDOWN:
			printf("keydown %d\n", event.key.keysym.sym);
			switch(event.key.keysym.sym)
			{
			case SDLK_DOWN:
			    if(yspeed != -SPEED_RATE)
			        yspeed = SPEED_RATE;
			    xspeed = 0;
			    break;
			case SDLK_UP:
			    if(yspeed != SPEED_RATE)
			        yspeed = -SPEED_RATE;
			    xspeed = 0;
			    break;
			case SDLK_RIGHT:
			    if(xspeed != -SPEED_RATE)
			        xspeed = SPEED_RATE;
			    yspeed = 0;
			    break;
			case SDLK_LEFT:
			    if(xspeed != SPEED_RATE)
			        xspeed = -SPEED_RATE;
			    yspeed = 0;
			    break;
			case SDLK_ESCAPE:
			    SDL_Quit();
			    return EXIT_SUCCESS;// Quit the program
			    break;
			}
		        printf("xspeed %d yspeed %d \n", xspeed, yspeed);
                    break;

            }
        }
       
        //Update the display
        SDL_UpdateWindowSurface(window);
        
    }

    // Tell the SDL to clean up and shut down
    SDL_Quit();

    return EXIT_SUCCESS;
}
