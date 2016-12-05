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

void InitializeGame();
void GameLoop();
short rand_a_b(short a, short b);

void Titlescreen();
void Instructions_screen();
void thegame_ingame();
void results_screen();

inline void toTitlescreen();

void Init_birds();
unsigned char Kill_all_birds();

void Draw_time(unsigned short y, unsigned char mode);

int Collision(unsigned short x, unsigned short y, unsigned char w, unsigned char h, unsigned short x2, unsigned short y2, unsigned char w2, unsigned char h2);
int Touch(unsigned short n1, unsigned short n2, unsigned short n3, unsigned short n4);

void Menu_screen();
void Instructions_screen_2P();
void thegame_ingame_2p();
void results_screen_2p();
void Show_character(unsigned char x, unsigned char y, unsigned char charac);
void Draw_score(unsigned int score, unsigned char x, unsigned short y);
void Change_character(unsigned char charac); 
void Instructions_screen_2P();
void Time();
void thegame_ingame_2p();
void results_screen_2p();

void Bird_show_ingame_2p_scale(unsigned char x, unsigned short y, unsigned char birds_state, unsigned char charac, float scale);

void share_url();
void Load_Highscore();
void Save_Highscore();
void Load_Highscore_old();

void Play_death_sound(unsigned char i);
