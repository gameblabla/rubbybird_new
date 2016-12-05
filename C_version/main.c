/*
	Copyright (C) 2016 Gameblabla
	Licensed under the GAMEBLABLA's VIDEO GAME PUBLIC LICENSE version 1.0.
	See COPYING file for more details.
	If a COPYING file was not provided, you can ask the copyright holder for a copy at :
	<gameblabla@openmailbox.org>
		
	This Program is distributed without WITHOUT ANY WARRANTY; 
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
	See the GAMEBLABLA's VIDEO GAME PUBLIC LICENSE for more details.
*/

/*
 * Rubby Bird Recreation
*/

#include "API.h"
#include "INPUT.h"
#include "main.h"
#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <SDL.h>
#ifndef ANDROID
#include <sys/wait.h>
#include <sys/types.h>
#endif

/*
 * GLOBALS 
 * All variables are declared here
*/
const unsigned char size_characters_w[] = { 18, 24, 18, 48, 45, 36, 32 };
const unsigned char size_characters_h[] = { 24, 24, 24, 48, 45, 28, 23 };
const unsigned char size_characters_scale[] = { 7, 6, 7, 4, 4, 5, 7 };
const float size_characters_scale_selection[] = { 2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 1.5f, 2.0f };
const float size_characters_scale_selection_big[] = { 4.0f, 4.0f, 4.0f, 3.0f, 3.0f, 4.0f, 5.0f };
const unsigned char size_characters_x[] = { 58, 50, 58, 20, 28, 28, 20 };
const unsigned char characters_shop[6] = { 1, 2, 3, 4, 5, 6 };
const unsigned short characters_shop_price[6] = { 500, 50, 1600, 112, 400, 600 };
const unsigned char save_version = 2;

unsigned char characters_unlocked[8] = { 1, 0, 0, 0, 0, 0, 0 };
unsigned char characters_ogg_play[8] = { 0, 2, 0, 3, 3, 3, 3 };
unsigned char characters_wav_dies[8] = { 1, 6, 1, 7, 7, 7, 7 };

unsigned char current_character = 0;
unsigned char menu_state = 0;
unsigned short bird_y = 115;
unsigned char game_mode = 0;
unsigned char alpha_dies = 0;
unsigned char boss_unlocked = 0;

short logo_y;

short bird_move_y;
unsigned char birds_dead;
unsigned char birds_limit;

struct birdies 
{
	unsigned char status;
	short life;
	unsigned short time;
	unsigned char direction;
	float scale;
	unsigned short rot;
} birds[2];

unsigned short tolose;
#ifdef DEBUG
unsigned int points_shop = 5000;
#else
unsigned int points_shop;
#endif
unsigned char bird_title_frame;
unsigned char bird_title_time;
unsigned char minutestounlock;

#define HIGHSCORE 1
#define NORMAL 0

unsigned char hour[2];
unsigned char minutes[2];
unsigned char seconds[2];
unsigned char ms[2];
unsigned char microseconds;

void Bird_show_titlescreen(unsigned short y);

/*
 * 
 * Main loop
 * 
*/

static void Play_ogg(unsigned char i, unsigned char loop)
{
	if (i != 125)
	{
		const unsigned char song_list[32][32] =
		{
			{DATA"rubby.ogg"},
			{DATA"2014_02_wip.ogg"},
			{DATA"walrii.ogg"},
			{DATA"div.ogg"}
		};
		Stop_Music();
		Load_Music(song_list[i]);
		Play_Music(loop);
	}
}

int main(int argc, char* argv[])
{
	InitializeGame();
	srand(time(0));

	while (!done)
	{
		Controls();
		GameLoop();
		Update_video();
	}

	Clearing(0);

	return 0;
}

/*
 * 
 * This function is used only at startup
 * It loads all the resources needed for the game, plays the "music" (birds crying),
 * and it will also load the highscore.
 * 
*/
void InitializeGame()
{
	unsigned char i;
	Init_video();
	Init_sound();

	Load_Image(1,DATA"fond.bmp",0);
	Load_Image(4,DATA"logo.bmp",1);
	Load_Image(5,DATA"score.bmp",1);
	Load_Image(6,DATA"go.bmp",1);
	Load_Image(7,DATA"ddot.bmp",1);
	Load_Image(8,DATA"bird.bmp",1);
	Load_Image(9,DATA"limit_score.bmp",1);
	Load_Image(10,DATA"inst.bmp",1);
	Load_Image(11,DATA"tap.bmp",1);
	Load_Image(12,DATA"tweet.bmp",1);
	Load_Image(13,DATA"copyright.bmp",1);
	Load_Image(13,DATA"press.bmp",1);
	Load_Image(14,DATA"red.bmp",1);
	Load_Image(15,DATA"small.bmp",1);
	Load_Image(16,DATA"menu.bmp",1);
	Load_Image(17,DATA"play.bmp",1);
	Load_Image(18,DATA"back.bmp",1);
	Load_Image(19,DATA"buy.bmp",1);
	
	Load_Image(90,DATA"cross.bmp",1);
	Load_Image(91,DATA"cross_right.bmp",1);
	Load_Image(92,DATA"p1_wins.bmp",1);
	Load_Image(93,DATA"p2_wins.bmp",1);
	Load_Image(94,DATA"rubit.bmp",1);
	
	Load_Image(20,DATA"bird.bmp",1);
	Load_Image(21,DATA"walrii.bmp",1);
	Load_Image(22,DATA"bird2.bmp",1);
	Load_Image(23,DATA"dyna.bmp",1);
	Load_Image(24,DATA"skelchu.bmp",1);
	Load_Image(25,DATA"lila.bmp",1);
	Load_Image(26,DATA"haku.bmp",1);
	
	Load_SFX(1, DATA"dies.wav");
	Load_SFX(2, DATA"tick.wav");
	Load_SFX(3, DATA"alarm.wav");
	Load_SFX(4, DATA"select.wav");
	Load_SFX(5, DATA"buy.wav");
	Load_SFX(6, DATA"walrii.wav");
	Load_SFX(7, DATA"lose.wav");
	
	Load_Highscore_old();
	Load_Highscore();
	
	logo_y = -60;
	Init_birds();
}


/*
 * 
 * Game loop
 * Screens are handled using the game_mode variable.
 * Quite simple really.
 * 
*/
void GameLoop()
{
	if (BUTTON.QUIT) done = 1;
	
	switch(game_mode)
	{
		case TITLESCREEN:
			Titlescreen();
		break;
		
		case MENU:
			Menu_screen();
		break;
		
		case INSTRUCTIONS_INGAME:
			Instructions_screen();
		break;

		case INSTRUCTIONS_INGAME_2P:
			Instructions_screen_2P();
		break;

		case INGAME:
			thegame_ingame();
		break;
		
		case INGAME_2P:
			thegame_ingame_2p();
		break;
		
		case RESULTS:
			results_screen();
		break;
		
		case RESULTS_2P:
			results_screen_2p();
		break;
	}
}

/*
 * Titlescreen.
 * Nuff said.
 * 
*/
void Titlescreen()
{
	static unsigned char titlescreen_state = 0;
	static unsigned char press_time = 0;
	
	Put_image(1,0,0,1);

	Bird_show_titlescreen(115);

	Put_image(4,20,logo_y,1);
	Draw_time(logo_y+30, HIGHSCORE);
	
	switch(titlescreen_state)
	{
		case 0:	
			press_time++;
			if (press_time < 25)
			Put_image(13,27,280,1);
			else if (press_time > 50) press_time = 0;
			
			if (logo_y < 30) logo_y+=2;
			
			if (touch[0].state == 1)
			{
				titlescreen_state = 1;
			}
		break;
		case 1:
			if (logo_y > -60) 
				logo_y-=2;
			else 
			{
				Play_ogg(1, 1);
				game_mode = MENU;
				birds_limit = 0;
				titlescreen_state = 0;
			}
		break;
	}

}

void Bird_show_titlescreen(unsigned short y)
{
	static unsigned char bird_anim = 0, bird_lantecy = 0;
	static unsigned char latency = 0;
	const unsigned char birds_frame[] = { 0, 1, 2, 1, 126 };
	
	if (birds_frame[bird_anim] == 126) bird_anim = 0;
	Put_sprite(20+current_character, size_characters_x[current_character], y+bird_move_y, size_characters_w[current_character], size_characters_h[current_character], birds_frame[bird_anim], birds[0].scale);
	
	latency++;
	bird_lantecy++;
	
	if (bird_lantecy > 6)
	{
		bird_lantecy = 0;
		bird_anim++;
	}
	
	if (birds[0].direction == 0)
	{
		if (latency > 1)
		{
			bird_move_y--;
			latency = 0;
		}
		
		if (bird_move_y < -8)
		{
			birds[0].direction = 1;
		}
	}
	else if (birds[0].direction == 1)
	{
		if (latency > 1)
		{
			bird_move_y++;
			latency = 0;
		}
		if (bird_move_y > 8)
		{
			birds[0].direction = 0;
		}	
	}
}

void Bird_show_ingame(unsigned short y, unsigned char birds_state)
{
	static unsigned char bird_anim = 0, bird_lantecy = 0;
	const unsigned char birds_frame[] = { 0, 1, 2, 1, 126 };
	
	if (birds_frame[bird_anim] == 126) bird_anim = 0;
	Put_sprite(20+current_character, size_characters_x[current_character], y+bird_move_y, size_characters_w[current_character], size_characters_h[current_character], birds_frame[bird_anim], birds[0].scale);
	
	bird_lantecy++;

	if (bird_lantecy > 6)
	{
		bird_lantecy = 0;
		bird_anim++;
	}
	
	if (birds_state == 0)
	{
		if (bird_move_y > -8)
		{
			bird_move_y-=2;
		}
	}
	else if (birds_state == 1)
	{
		if (bird_move_y < 8)
		{
			bird_move_y+=2;
		}	
	}	
}

void Bird_show_selection(unsigned short x, unsigned short y)
{
	static unsigned char bird_anim = 0, bird_lantecy = 0;
	static unsigned char latency = 0;
	const unsigned char birds_frame[] = { 0, 1, 2, 1, 126 };
	
	if (birds_frame[bird_anim] == 126) bird_anim = 0;
	Put_sprite(20+current_character, size_characters_x[current_character]+x, y+bird_move_y, size_characters_w[current_character], size_characters_h[current_character], birds_frame[bird_anim], size_characters_scale_selection_big[current_character]);
	
	latency++;
	bird_lantecy++;
	
	if (bird_lantecy > 6)
	{
		bird_lantecy = 0;
		bird_anim++;
	}
	
	if (birds[0].direction == 0)
	{
		if (latency > 1)
		{
			bird_move_y--;
			latency = 0;
		}
		
		if (bird_move_y < -8)
		{
			birds[0].direction = 1;
		}
	}
	else if (birds[0].direction == 1)
	{
		if (latency > 1)
		{
			bird_move_y++;
			latency = 0;
		}
		if (bird_move_y > 8)
		{
			birds[0].direction = 0;
		}	
	}
}

void Bird_show_shop(unsigned short x, unsigned short y, unsigned char charac)
{
	static unsigned char bird_anim = 0, bird_lantecy = 0;
	static unsigned char latency = 0;
	const unsigned char birds_frame[] = { 0, 1, 2, 1, 126 };
	
	if (birds_frame[bird_anim] == 126) bird_anim = 0;
	Put_sprite(20+charac, size_characters_x[charac]+x, y+bird_move_y, size_characters_w[charac], size_characters_h[charac], birds_frame[bird_anim], size_characters_scale_selection_big[charac]);
	
	latency++;
	bird_lantecy++;
	
	if (bird_lantecy > 6)
	{
		bird_lantecy = 0;
		bird_anim++;
	}
	
	if (birds[0].direction == 0)
	{
		if (latency > 1)
		{
			bird_move_y--;
			latency = 0;
		}
		
		if (bird_move_y < -8)
		{
			birds[0].direction = 1;
		}
	}
	else if (birds[0].direction == 1)
	{
		if (latency > 1)
		{
			bird_move_y++;
			latency = 0;
		}
		if (bird_move_y > 8)
		{
			birds[0].direction = 0;
		}	
	}
}


void Bird_show_ingame_2p(unsigned char x, unsigned short y, unsigned char birds_state, unsigned char charac)
{
	const unsigned char birds_frame[] = { 0, 1, 2, 1, 126 };
	static unsigned char bird_anim = 0, bird_lantecy = 0;
	static short move_y[2], tochoose = 0;
	
	bird_lantecy++;
	tochoose = (charac == 2) ? 1 : 0;

	if (bird_lantecy > 12)
	{
		bird_lantecy = 0;
		bird_anim++;
	}
	
	if (birds_state == 0)
	{
		if (move_y[tochoose] > -8) move_y[tochoose]-=2;
	}
	else if (birds_state == 1)
	{
		if (move_y[tochoose] < 8) move_y[tochoose]+=2;
	}	
	
	if (birds_frame[bird_anim] == 126) bird_anim = 0;
	Put_sprite(20+charac, x-size_characters_x[charac], y+move_y[tochoose], size_characters_w[charac], size_characters_h[charac], birds_frame[bird_anim], 4.0f);
}

void Bird_show_ingame_2p_scale(unsigned char x, unsigned short y, unsigned char birds_state, unsigned char charac, float scale)
{
	const unsigned char birds_frame[] = { 0, 1, 2, 1, 126 };
	static unsigned char bird_anim = 0, bird_lantecy = 0;
	static short move_y[2], tochoose = 0;
	
	bird_lantecy++;
	tochoose = (charac == 2) ? 1 : 0;


	if (bird_lantecy > 12)
	{
		bird_lantecy = 0;
		bird_anim++;
	}
	
	if (birds_state == 0)
	{
		if (move_y[tochoose] > -8) move_y[tochoose]-=2;
	}
	else if (birds_state == 1)
	{
		if (move_y[tochoose] < 8) move_y[tochoose]+=2;
	}	
	
	if (birds_frame[bird_anim] == 126) bird_anim = 0;
	Put_sprite(20+charac, x-size_characters_x[charac], y+move_y[tochoose], size_characters_w[charac], size_characters_h[charac], birds_frame[bird_anim], scale);
}


void Bird_show_result()
{
	static unsigned char bird_anim = 0, bird_lantecy = 0;
	static unsigned char latency = 0;
	const unsigned char birds_frame[] = { 0, 1, 2, 1, 126 };
	
	if (birds[0].scale > 0.0f)
	{
		if (birds_frame[bird_anim] == 126) bird_anim = 0;
		Put_sprite_rot(20+current_character, size_characters_x[current_character], 115, size_characters_w[current_character], size_characters_h[current_character], birds_frame[bird_anim], birds[0].scale, birds[0].rot);
		
		birds[0].rot+=2;
		birds[0].scale-=0.1;
		
		latency++;
		bird_lantecy++;
		
		if (bird_lantecy > 6)
		{
			bird_lantecy = 0;
			bird_anim++;
		}
	}
}

/*
 * Screen that shows you what you're supposed to do ingame.
 * Tapping the screen will start the game. 
 * 
*/
void Menu_screen()
{
	static short x_menu = -64;
	static unsigned char shop_chosen = 0;
	
	Put_image(1,0,0,1);
	
	switch (menu_state)
	{
		case 0:
			if (x_menu < 0) x_menu+=4;
			Put_image(16,x_menu,0,1);
			Put_image(18,x_menu,300,1);
			if (touch[0].state == 1)
			{
				// Character selection
				if (Touch(8,230,125,170))
				{
					Play_SFX(4);
					x_menu = -64;
					menu_state = 1;
				}
				
				// Multiplayer
				else if (Touch(8,230,190,240))
				{
					x_menu = -64;
					game_mode = INSTRUCTIONS_INGAME_2P;
					Stop_Music();
				}
				
				// Shop
				else if (Touch(69,170,260,300))
				{
					Play_SFX(4);
					x_menu = -64;
					menu_state = 2;
					shop_chosen = 0;
				}
				
				// Back
				else if (Touch(0,70,290,321))
				{
					x_menu = -64;
					game_mode = TITLESCREEN;
					Stop_Music();
				}
			}
		break;
		case 1:
			Put_image(18,0,300,1);
			Put_image(17,180,301,1);
			Bird_show_selection(25,55);

			Show_character(10, 190, 0);
			Show_character(60, 187, 1);
			Show_character(122, 190, 2);
			Show_character(170, 190, 3);
			Show_character(5, 240, 4);
			Show_character(55, 240, 5);
			Show_character(110, 240, 6);
				
			if (touch[0].state == 1)
			{
				if (Touch(0,70,290,321))
				{
					x_menu = -64;
					menu_state = 0;
				}
				else if (Touch(180,240,290,321))
				{
					x_menu = -64;
					menu_state = 0;
					game_mode = INSTRUCTIONS_INGAME;
					Stop_Music();
				}
				
			}
		break;
		case 2:
			Put_image(18,0,300,1);
			
			Bird_show_shop(25,80, characters_shop[shop_chosen]);
			
			if (characters_unlocked[shop_chosen+1] == 0) Put_image(19,95,205,1);
			if (shop_chosen != 0) Put_image(90,20,200,1);
			if (shop_chosen != sizeof(characters_shop)-1) Put_image(91,195,200,1);
			
			Draw_score(points_shop, 70, 240);
			if (characters_unlocked[shop_chosen+1] == 0) Draw_score(characters_shop_price[shop_chosen], 70, 50);
			
			if (touch[0].state == 1)
			{
				if (Touch(0,65,190,230))
				{
					if (shop_chosen > 0) 
					{
						shop_chosen--;
						Play_SFX(4);
					}
				}
				else if (Touch(175,240,190,230))
				{
					if (shop_chosen < sizeof(characters_shop)-1)
					{ 
						shop_chosen++;
						Play_SFX(4);
					}
				}
				
				if (Touch(80,160,190,230))
				{
					if (characters_unlocked[shop_chosen+1] == 0 && points_shop > characters_shop_price[shop_chosen])
					{
						Play_SFX(5);
						points_shop = points_shop - characters_shop_price[shop_chosen];
						characters_unlocked[shop_chosen+1] = 1;
						Save_Highscore();
					}
					printf("BUY\n");
				}
				
				if (Touch(0,70,290,321) || BUTTON.SELECT)
				{
					Play_SFX(4);
					x_menu = -64;
					menu_state = 0;
					BUTTON.SELECT = 0;
				}
			}
		break;
		
	}

}

void Show_character(unsigned char x, unsigned char y, unsigned char charac)
{	
	if (characters_unlocked[charac] == 1 || charac == 0)
	{
		Put_sprite(20+charac, x, y, size_characters_w[charac], size_characters_h[charac], 0, size_characters_scale_selection[charac]);
		if (Touch(x,x+56,y-5,y+60))
		{
			if (touch[0].state == 1)
			Change_character(charac);	
		}
	}
	else
		Put_sprite(20+charac, x, y, size_characters_w[charac], size_characters_h[charac], 4, size_characters_scale_selection[charac]);
}

void Change_character(unsigned char charac)
{	
	current_character = charac;
	birds[0].scale = size_characters_scale[charac];	
	Play_SFX(4);
	// Play sound effect here
}


/*
 * Screen that shows you what you're supposed to do ingame.
 * Tapping the screen will start the game. 
 * 
*/
void Instructions_screen()
{
	static unsigned char appear_time;
	Put_image(1,0,0,1);
	Bird_show_titlescreen(115);
	if (current_character == 0 || current_character == 2) Put_image(10,20,50,1);
	else Put_image(94,70,50,1);
	
	if (appear_time < 30)
	Put_image(11,32,280,1);
	else if (appear_time > 60) appear_time = 0;
	appear_time++;
	
	if (touch[0].state == 1)
	{
		Play_ogg(characters_ogg_play[current_character], 1);
		game_mode = INGAME;
	}
}

void Instructions_screen_2P()
{
	static unsigned char appear_time;
	Put_image(1,0,0,1);
	Put_image(10,20,50,1);
	Bird_show_ingame_2p(25+58,170,0,0);
	Bird_show_ingame_2p(145+58,170,0,2);
	
	if (appear_time < 30)
	Put_image(11,32,280,1);
	else if (appear_time > 60) appear_time = 0;
	appear_time++;
	
	if (touch[0].state == 1)
	{
		game_mode = INGAME_2P;
	}
}

/*
 * The REAL game.
 * Does all sort of crazy stuff too.
 * 
*/
void thegame_ingame()
{
	static unsigned short old_touch_y = 0;
	static unsigned short count = 0;
	unsigned short real_touch_y;
	real_touch_y = (unsigned short)touch[0].y;
	
	Put_image(1,0,0,1);
	Draw_time(50, NORMAL);
	Bird_show_ingame(bird_y, birds[0].direction);
	
	if (alpha_dies > 0) Put_image_alpha(14, alpha_dies);
	if (seconds[0] > 9 && minutestounlock == 0) 
	{
		tolose = 4;
		minutestounlock = 1;
	}
	
	if (birds[0].life < 100)
	{
		if (alpha_dies < 248) alpha_dies+=6;
	}
	else
	{
		if (alpha_dies > tolose) alpha_dies-=tolose;
	}
	
	// Bird going up
	if (touch[0].inmotion)
	{
		if ( (real_touch_y > old_touch_y) || (real_touch_y < old_touch_y) )
		{
			count++;
		}
		if (count > 8)
		{
			birds[0].life = 256;
			old_touch_y = real_touch_y;
			count = 0;
			if (birds[0].direction == 0)  birds[0].direction = 1;
			else birds[0].direction = 0;
		}
	}
	else
	{
		if (birds[0].life > tolose) birds[0].life-=tolose;
		else if (birds[0].life < tolose+1)
		{
			Stop_Music();
			Play_death_sound(current_character);
			game_mode = RESULTS;
		}
	}
	
	Time();
}

void Play_death_sound(unsigned char i)
{
	if (i != 125) Play_SFX(characters_wav_dies[i]);
}

void thegame_ingame_2p()
{
	static unsigned short old_touch_y[2] = { 0, 0 };
	static unsigned short count[2] = { 0, 0 };
	unsigned short real_touch_y[2];
	
	real_touch_y[0] = (unsigned short)touch[0].y;
	real_touch_y[1] = (unsigned short)touch[1].y;
	
	Put_image(1,0,0,1);
	Draw_time(50, NORMAL);
	Bird_show_ingame_2p(25+58,170,birds[0].direction,0);
	Bird_show_ingame_2p(145+58,170,birds[1].direction,2);
	
	if (seconds[0] > 9 && minutestounlock == 0) 
	{
		tolose = 4;
		minutestounlock = 1;
	}

	// Bird going up
	if (touch[0].inmotion && touch[0].x < 120)
	{
		if ( (real_touch_y[0] > old_touch_y[0]) || (real_touch_y[0] < old_touch_y[0]) )
		{
			count[0]++;
		}
		if (count[0] > 8)
		{
			birds[0].life = 256;
			old_touch_y[0] = real_touch_y[0];
			count[0] = 0;
			if (birds[0].direction == 0)  birds[0].direction = 1;
			else birds[0].direction = 0;
		}
	}
	else
	{
		if (birds[0].life > tolose) birds[0].life-=tolose;
		else if (birds[0].life < tolose+1)
		{
			Stop_Music();
			Play_SFX(characters_wav_dies[0]);
			game_mode = RESULTS_2P;
		}
	}
	
	if (touch[1].inmotion && touch[1].x > 120)
	{
		if ( (real_touch_y[1] > old_touch_y[1]) || (real_touch_y[1] < old_touch_y[1]) )
		{
			count[1]++;
		}
		if (count[1] > 8)
		{
			birds[1].life = 256;
			old_touch_y[1] = real_touch_y[1];
			count[1] = 0;
			if (birds[1].direction == 0)  birds[1].direction = 1;
			else birds[1].direction = 0;
		}
	}
	else
	{
		if (birds[1].life > tolose) birds[1].life-=tolose;
		else if (birds[1].life < tolose+1)
		{
			Stop_Music();
			Play_SFX(characters_wav_dies[2]);
			game_mode = RESULTS_2P;
		}
	}
	
	Time();
}


/*
 * The results screen, when you die.
 * You can either decide to share your highscore (via Twitter)
 * or go back to the titlescreen.
 * 
*/
void results_screen()
{
	static short gameover_x = - 272;
	Put_image(1,0,0,1);
	Put_image(6,gameover_x,50,1);
	Put_image(12,gameover_x+15,180,1);
	Bird_show_result();
	
	if (gameover_x < 10) 
	{
		gameover_x+=5;
	}
	else
	{
		Draw_time(100, NORMAL);	
		if (touch[0].state == 1)
		{
			if (Touch(50,200,150,230))
			{
				share_url();
			}
			else if (Touch(25,220,241,300))
			{
				gameover_x = 0;
				toTitlescreen();
			}
		}
	}
}

void results_screen_2p()
{
	static short gameover_x = - 272;
	static float scale = 4.0f;
	
	Put_image(1,0,0,1);
	
	if (scale > 0.0f) scale -= 0.1f;
	
	if (birds[0].life == 0)
	{
		Bird_show_ingame_2p_scale(25+58,170,0,0,scale);
		Bird_show_ingame_2p(145+58,170,0,2);
		Put_image(93,gameover_x,45,1);
	}
	else
	{
		Bird_show_ingame_2p(25+58,170,0,0);
		Bird_show_ingame_2p_scale(145+58,170,0,2,scale);
		Put_image(92,gameover_x,45,1);
	}
		
	
	if (gameover_x < 64) 
	{
		gameover_x+=6;
	}
	else
	{
		if (touch[0].state == 1)
		{
			gameover_x = 0;
			scale = 4.0f;
			toTitlescreen();
		}
	}
}

/*
 * Tells the game to back to the Titlescreen.
 * It also makes sure everything is set to their default value.
 * (Or else you gonna have a bad time) 
 * 
*/

void Time()
{
	microseconds++;
	if (microseconds > 59)
	{
		points_shop++;
		seconds[NORMAL]++;
		microseconds = 0;
		if (seconds[NORMAL] > 59)
		{
			minutes[NORMAL]++;
			seconds[NORMAL] = 0;
			if (minutes[NORMAL] > 59)
			{
				hour[NORMAL]++;
				minutes[NORMAL] = 0;
			}
		}
	}	
}

inline void toTitlescreen()
{
	game_mode = TITLESCREEN;
	logo_y = -60;

	birds_dead = 0;
	birds_limit = 0;
	
	/*
	 * Checks if the user beat the current highscore. 
	 * If so, it will also make sure to save it.
	*/
	if ((hour[NORMAL] > hour[HIGHSCORE]) || (minutes[NORMAL] > minutes[HIGHSCORE]) || seconds[NORMAL] > seconds[HIGHSCORE])
	{
		hour[HIGHSCORE] = hour[NORMAL];
		minutes[HIGHSCORE] = minutes[NORMAL];
		seconds[HIGHSCORE] = seconds[NORMAL];
		ms[HIGHSCORE] = ms[NORMAL];
	}
	
	Save_Highscore();
	
	hour[NORMAL] = 0;
	minutes[NORMAL] = 0;
	seconds[NORMAL] = 0;
	ms[NORMAL] = 0;
	Init_birds();
	birds_dead = 0;
}

/*
 * This function handles collisions.
 * It is quite simple : it only checks if it is inside the collision box.
 * No color detection or some shit like that.
 * 
*/

int Collision(unsigned short x, unsigned short y, unsigned char w, unsigned char h, unsigned short x2, unsigned short y2, unsigned char w2, unsigned char h2)
{
	if ((x + w > x2) && (x < x2 + w2) && (y + h > y2) && (y < y2 + h2))
	{
		return 1;
	}	
	return 0;
}

int Touch(unsigned short n1, unsigned short n2, unsigned short n3, unsigned short n4)
{
	if (touch[0].x > n1 && touch[0].x < n2 && touch[0].y > n3 && touch[0].y < n4)
	{
		return 1;
	}	
	return 0;
}

/*
 * 
 * Draws the current time.
 * mode can take two parameter : HIGHSCORE and NORMAL.
 * HIGHSCORE means it will show your current highscore.
 * NORMAL will simply show your current score
 * 
*/


void Draw_time(unsigned short y, unsigned char mode)
{
	unsigned char minutes_toshow[2], sec_toshow[2], hour_toshow[2], ms_toshow[2];
	sec_toshow[0] = (seconds[mode] / 10) % 10;
	sec_toshow[1] = (seconds[mode]) % 10;
	minutes_toshow[0] = (minutes[mode] / 10) % 10;
	minutes_toshow[1] = (minutes[mode]) % 10;
	hour_toshow[0] = (hour[mode] / 10) % 10;
	hour_toshow[1] = (hour[mode]) % 10;
	if (mode == 0)
	{
		ms_toshow[0] = (microseconds / 10) % 10;
		ms_toshow[1] = (microseconds) % 10;
		Put_sprite(15, 101, y+24, 8, 8, ms_toshow[0], 2);	
		Put_sprite(15, 121, y+24, 8, 8, ms_toshow[1], 2);	
	}
	
	Put_sprite(5, 50, y, 10, 10, hour_toshow[0], 2);	
	Put_sprite(5, 69, y, 10, 10, hour_toshow[1], 2);	
	Put_image(7,87,y,2);
	Put_sprite(5, 100, y, 10, 10, minutes_toshow[0], 2);	
	Put_sprite(5, 119, y, 10, 10, minutes_toshow[1], 2);	
	Put_image(7,138,y,2);
	Put_sprite(5, 150, y, 10, 10, sec_toshow[0], 2);	
	Put_sprite(5, 170, y, 10, 10, sec_toshow[1], 2);	
}


void Draw_score(unsigned int score, unsigned char x, unsigned short y)
{
	unsigned char scoreto[6];
	scoreto[0] = (score / 100000) % 10;
	scoreto[1] = (score / 10000) % 10;
	scoreto[2] = (score / 1000) % 10;
	scoreto[3] = (score / 100) % 10;
	scoreto[4] = (score / 10) % 10;
	scoreto[5] = (score) % 10;
	
	Put_sprite(5, x, y, 10, 10, scoreto[0], 2);	
	Put_sprite(5, x+18, y, 10, 10, scoreto[1], 2);	
	Put_sprite(5, x+36, y, 10, 10, scoreto[2], 2);	
	Put_sprite(5, x+56, y, 10, 10, scoreto[3], 2);	
	Put_sprite(5, x+74, y, 10, 10, scoreto[4], 2);	
	Put_sprite(5, x+92, y, 10, 10, scoreto[5], 2);	
}

/*
 * 
 * Set all the birds to their defaults
 * 
*/

void Init_birds()
{
	unsigned char i;
	
	for(i=0;i<2;i++)
	{
		birds[i].time = 0;
		birds[i].direction = 0;
		birds[i].scale = (float)size_characters_scale[current_character];
		birds[i].status = 0;
		birds[i].life = 255;
		birds[i].rot = 0;
	}
	alpha_dies = 0;
	minutestounlock = 0;
	tolose = 2;
}


/*
 * Generate number between a and b 
 * 
*/

short rand_a_b(short a, short b)
{
    return rand()%(b-a) +a;
}


/*
 * 
 * Misc stuff : Share URL for Android/iOS, Save/Load Highscore...
 * 
*/

/*
 *  SDL_OpenURL from libSDL project
 *  https://bugzilla.libsdl.org/show_bug.cgi?id=2783
 *  My license doesn't cover this code.
*/
#ifndef ANDROID
static int SDL_OpenURL(const char *url)
{
    pid_t pid1;
    int status;

    // Usual fork. (cannot use "vfork()" here, because after we will use "vfork()" which is
    // different from "_exit()" and "exec*()" that are the only allowed functions).
    pid1 = fork();
    if (pid1 == 0) 
    {
        // Child process

        // ****************************************
        // Notice this is "vfork()" and not "fork()"
        // See restrictions in documentation
        // ****************************************
        pid_t pid2 = vfork();
        if (pid2 == 0) {
            // Grand child process will try to launch the url
            execlp("xdg-open", "xdg-open", url, (char *)0);
			_exit(EXIT_FAILURE);
        } else if (pid2 < 0) {
            // Error forking
			_exit(EXIT_FAILURE);
        } else {
            // Child process doesn't wait for Grand child process which *might* be blocking.
            // Success forking
            _exit(EXIT_SUCCESS);
        }
    } else if (pid1 < 0) {
        return -1; // Error forking
    } else {
        if (waitpid(pid1, &status, 0) == pid1) {
             if (WIFEXITED(status)) {
                 if (WEXITSTATUS(status) == 0) {
                     return 0; // Sucess
                 } else {
                     return -1; // Error xdg-open
                 }
             } else {
                return -1; // Error in Child process
             }
        } else {
            return -1; // Error waiting for Child process
        }
    }

    return 0;
}
#endif


void share_url()
{
	char tmp[300];
	/*
	 * This needs a patched SDL2 with SDL_OpenURL.
	 * As of my writing, it is still in the discussion stages unfortunely...
	 * On other platforms it could be replaced with some equivalents but on Android ?
	 * Not so easily... 
	*/
#ifdef ANDROID
	int size_str;
	if (hour[NORMAL] == 0 && minutes[NORMAL] == 0)
	{
		snprintf(tmp, sizeof(tmp), "twitter://post?message=I've rubbed the bird for %d seconds! ", seconds[NORMAL]);
		size_str = 62;
	}
	else if (hour[NORMAL] == 0 && minutes[NORMAL] > 0)
	{
		snprintf(tmp, sizeof(tmp), "twitter://post?message=I've rubbed the bird for %dm:%ds! ", minutes[NORMAL], seconds[NORMAL]);
		size_str = 60;
	}
	else
	{
		snprintf(tmp, sizeof(tmp), "twitter://post?message=I've rubbed the bird for %dh:%dm:%ds! ", hour[NORMAL], minutes[NORMAL], seconds[NORMAL]);
		size_str = 64;
	}
#else
	if (hour[NORMAL] == 0 && minutes[NORMAL] == 0)
	{
		snprintf(tmp, sizeof(tmp), "https://twitter.com/intent/tweet?text=I've rubbed the bird for %d seconds! ", seconds[NORMAL]);
	}
	else if (hour[NORMAL] == 0 && minutes[NORMAL] > 0)
	{
		snprintf(tmp, sizeof(tmp), "https://twitter.com/intent/tweet?text=I've rubbed the bird for %dm:%ds! ", minutes[NORMAL], seconds[NORMAL]);
	}
	else
	{
		snprintf(tmp, sizeof(tmp), "https://twitter.com/intent/tweet?text=I've rubbed the bird for %dh:%dm:%ds! ", hour[NORMAL], minutes[NORMAL], seconds[NORMAL]);
	}
#endif
	strcat(tmp, "%23rubbybird https://play.google.com/store/apps/details?id=com.gameblabla.rubbybird");
	Play_SFX(1);
	SDL_OpenURL(tmp);
}

static uint32_t ReadLongLittleEndian (FILE* output) 
{
    uint32_t temp_long;
    temp_long  = fgetc(output);
    temp_long |= fgetc(output) << 8;
    temp_long |= fgetc(output) << 16;
    temp_long |= fgetc(output) << 24;
    return temp_long;
}

static void WriteIntLittleEndian (uint32_t long_in, FILE* output)  
{
    fputc (((int)(long_in      )) & 0xFF, output);
    fputc (((int)(long_in >>  8)) & 0xFF, output);
    fputc (((int)(long_in >> 16)) & 0xFF, output);
    fputc (((int)(long_in >> 24)) & 0xFF, output);
    fflush(output);
}

void Load_Highscore()
{
	FILE* file;
	char directory[256];
	unsigned char i;
	unsigned char stub;
	int isdeleted;
		
	#if defined(ANDROID)
		/* Saves in the internal storage path for the game */
		snprintf(directory, sizeof(directory), "%s/rubby_bird.save", SDL_AndroidGetInternalStoragePath());
	#elif defined(GCW0)
		/* Saves in the home directory */
		char letsgohomedir[128];
		snprintf(letsgohomedir, sizeof(letsgohomedir), "%s/.rubbybird", getenv("HOME"));
		mkdir(letsgohomedir, 0755);
		snprintf(directory, sizeof(directory), "%s/rubby_bird.save", letsgohomedir);
	#elif defined(SELFISHOS)
		/* Saves in the config directory */
		char letsgohomedir[256];
		snprintf(letsgohomedir, sizeof(letsgohomedir), "%s/.config/harbour-rubbybird", getenv("HOME"));
		mkdir(letsgohomedir, 0755);
		snprintf(directory, sizeof(directory), "%s/rubby_bird.save", letsgohomedir);
	#elif defined(HOMEDIR)
		/* Saves in the config directory */
		char letsgohomedir[128];
		snprintf(letsgohomedir, sizeof(letsgohomedir), "%s/.config/rubbybird", getenv("HOME"));
		mkdir(letsgohomedir, 0755);
		snprintf(directory, sizeof(directory), "%s/rubby_bird.save", letsgohomedir);
	#else
		/* Relative */
		snprintf(directory, sizeof(directory), "rubby_bird.save");
	#endif
		
	file = fopen(directory, "r");
		
	if (file != NULL)
	{
		/* The order is very important, don't mess it up ! */
		hour[HIGHSCORE] = ReadLongLittleEndian(file);
		minutes[HIGHSCORE] = ReadLongLittleEndian(file);
		seconds[HIGHSCORE] = ReadLongLittleEndian(file);
		ms[HIGHSCORE] = ReadLongLittleEndian(file);
		points_shop = ReadLongLittleEndian(file);
		boss_unlocked = ReadLongLittleEndian(file);
		/* stub is the save version, not used (for now) */
		stub = ReadLongLittleEndian(file);
		for(i=0;i<sizeof(characters_unlocked);i++)
		{
			characters_unlocked[i] = ReadLongLittleEndian(file);
		}
	}
	else
	{
		Save_Highscore();
	}
	
	if (file) fclose(file);	
	
	#ifdef ANDROID
	if(access( "/data/data/com.gameblabla.rubbybird/files/savebird", F_OK ) != -1) 
	{
		points_shop = points_shop + 5000;
		isdeleted = unlink("/data/data/com.gameblabla.rubbybird/files/savebird");
		Save_Highscore();
	}
	#endif
	
}

void Save_Highscore()
{
	FILE* file;
	char directory[256];
	unsigned char i;
		
	#if defined(ANDROID)
		/* Saves in the internal storage path for the game */
		snprintf(directory, sizeof(directory), "%s/rubby_bird.save", SDL_AndroidGetInternalStoragePath());
	#elif defined(GCW0)
		/* Saves in the home directory */
		char letsgohomedir[128];
		snprintf(letsgohomedir, sizeof(letsgohomedir), "%s/.rubbybird", getenv("HOME"));
		mkdir(letsgohomedir, 0755);
		snprintf(directory, sizeof(directory), "%s/rubby_bird.save", letsgohomedir);
	#elif defined(SELFISHOS)
		/* Saves in the config directory */
		char letsgohomedir[256];
		snprintf(letsgohomedir, sizeof(letsgohomedir), "%s/.config/harbour-rubbybird", getenv("HOME"));
		mkdir(letsgohomedir, 0755);
		snprintf(directory, sizeof(directory), "%s/rubby_bird.save", letsgohomedir);
	#elif defined(HOMEDIR)
		/* Saves in the config directory */
		char letsgohomedir[128];
		snprintf(letsgohomedir, sizeof(letsgohomedir), "%s/.config/rubbybird", getenv("HOME"));
		mkdir(letsgohomedir, 0755);
		snprintf(directory, sizeof(directory), "%s/rubby_bird.save", letsgohomedir);
	#else
		/* Relative */
		snprintf(directory, sizeof(directory), "rubby_bird.save");
	#endif
	
	file = fopen(directory, "w");

	WriteIntLittleEndian((uint32_t)hour[HIGHSCORE], file);
	WriteIntLittleEndian((uint32_t)minutes[HIGHSCORE], file);
	WriteIntLittleEndian((uint32_t)seconds[HIGHSCORE], file);
	WriteIntLittleEndian((uint32_t)ms[HIGHSCORE], file);
	WriteIntLittleEndian((uint32_t)points_shop, file);
	WriteIntLittleEndian((uint32_t)boss_unlocked, file);
	WriteIntLittleEndian((uint32_t)save_version, file);

	for(i=0;i<sizeof(characters_unlocked);i++)
	{
		WriteIntLittleEndian(characters_unlocked[i], file);
	}

	if (file) fclose(file);
}

/* 
 * Old save format is now deprecated, because i broke it really hard lol 
 * The new save format now uses code by Lionel Debroux, thanks !
 * One note : Only the Android port is affected, the PC version was never officially released.
*/
void Load_Highscore_old()
{
	#if defined(ANDROID)
	FILE* file;
	char directory[256];
	unsigned char savefile[56];
	unsigned char i;
	int isdeleted;
		
	/* Saves in the internal storage path for the game */
	snprintf(directory, sizeof(directory), "%s/rubbybird.save", SDL_AndroidGetInternalStoragePath());

	if(access( directory, F_OK ) != -1)  
	{	
		file = fopen(directory, "r+");
			
		if (file != NULL)
		{
			for(i=0;i<sizeof(savefile);i++)
			{
				savefile[i] = fgetc(file);
			}
			hour[HIGHSCORE] = savefile[0];
			minutes[HIGHSCORE] = savefile[1];
			seconds[HIGHSCORE] = savefile[2];
			ms[HIGHSCORE] = savefile[3];
			points_shop = savefile[4];
			boss_unlocked = savefile[5];
			for(i=0;i<sizeof(characters_unlocked);i++)
			{
				characters_unlocked[i] = savefile[24+i];
			}
		}
		
		/* The old save format put an Int inside of a char...
		 * So players losts points if they had more than 255 pts...
		*/
		points_shop = points_shop * 10;
		
		if (file) fclose(file);	
		Save_Highscore();
		isdeleted = unlink(directory);
	}
	#endif
}
