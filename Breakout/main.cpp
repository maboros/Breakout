#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "wtypes.h"
#include <vector>
#include <string>
#include <sstream>

#include <tinyxml2.h>


#pragma comment(lib, "tinyxml2.lib")

#define screenWidth 800
#define screenHeight 600
using namespace std;
	
class Brick {
private:
	int Health = 0;
	const char* BrickTexture;
	const char* HitSound;
	const char* BreakSound;
	int BreakScore;
public:
	Brick(const char*brickTexture, const char*hitSound, int breakScore=-1, int health = -1, const char*breakSound = "\0") {
		Health = health;
		BrickTexture = brickTexture;
		HitSound = hitSound;
		BreakSound = breakSound;
		BreakScore = breakScore;
	}
	Brick() {
		Health = 0;
		BrickTexture = "";
		HitSound = "bang";
		BreakSound = "";
		BreakScore = 0;
	}
	bool BrickHit(){
		Health -= 1;
		return true;
	}
	int BrickType() {
		return Health;
	}
};

std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}


int main() {
	int rowCount;
	int columnCount;
	int rowSpacing;
	int columnSpacing;
	const char* backgroundTexture;
	//Soft brick
	const char* softBrickId;
	const char* softBrickTexture;
	const char* softBrickHealth;
	const char* softHitSound;
	const char* softBreakSound;
	int softBreakScore;
	//Medium brick
	const char* mediumBrickId;
	const char* mediumBrickTexture;
	const char* mediumBrickHealth;
	const char* mediumHitSound;
	const char* mediumBreakSound;
	int mediumBreakScore;
	//Hard brick
	const char* hardBrickId;
	const char* hardBrickTexture;
	const char* hardBrickHealth;
	const char* hardHitSound;
	const char* hardBreakSound;
	int hardBreakScore;
	//Impenetrable brick
	const char* opBrickId;
	const char* opBrickTexture;
	const char* opBrickHealth;
	const char* opHitSound;
	
	const char* level;

	tinyxml2::XMLDocument doc;
	doc.LoadFile("Resources\\level.xml");
	doc.PrintError();

	tinyxml2::XMLElement* levelElements = doc.FirstChildElement("Level");
	levelElements->QueryIntAttribute("RowCount", &rowCount);
	levelElements->QueryIntAttribute("ColumnCount", &columnCount);
	levelElements->QueryIntAttribute("RowSpacing", &rowSpacing);
	levelElements->QueryIntAttribute("ColumnSpacing", &columnSpacing);
	levelElements->QueryStringAttribute("BackgroundTexture", &backgroundTexture);
	tinyxml2::XMLElement* brickTypeElements = doc.FirstChildElement("Level")->FirstChildElement("BrickTypes")->FirstChildElement("BrickType");
	brickTypeElements->QueryStringAttribute("Id", &softBrickId);
	brickTypeElements->QueryStringAttribute("Texture", &softBrickTexture);
	brickTypeElements->QueryStringAttribute("HitPoints", &softBrickHealth);
	brickTypeElements->QueryStringAttribute("HitSound", &softHitSound);
	brickTypeElements->QueryStringAttribute("BreakSound", &softBreakSound);
	brickTypeElements->QueryIntAttribute("BreakScore", &softBreakScore);
	brickTypeElements = doc.FirstChildElement("Level")->FirstChildElement("BrickTypes")->FirstChildElement("BrickType")->NextSiblingElement();
	brickTypeElements->QueryStringAttribute("Id", &mediumBrickId);
	brickTypeElements->QueryStringAttribute("Texture", &mediumBrickTexture);
	brickTypeElements->QueryStringAttribute("HitPoints", &mediumBrickHealth);
	brickTypeElements->QueryStringAttribute("HitSound", &mediumHitSound);
	brickTypeElements->QueryStringAttribute("BreakSound", &mediumBreakSound);
	brickTypeElements->QueryIntAttribute("BreakScore", &mediumBreakScore);
	brickTypeElements = doc.FirstChildElement("Level")->FirstChildElement("BrickTypes")->FirstChildElement("BrickType")->NextSiblingElement()->NextSiblingElement();
	brickTypeElements->QueryStringAttribute("Id", &hardBrickId);
	brickTypeElements->QueryStringAttribute("Texture", &hardBrickTexture);
	brickTypeElements->QueryStringAttribute("HitPoints", &hardBrickHealth);
	brickTypeElements->QueryStringAttribute("HitSound", &hardHitSound);
	brickTypeElements->QueryStringAttribute("BreakSound", &hardBreakSound);
	brickTypeElements->QueryIntAttribute("BreakScore", &hardBreakScore);
	brickTypeElements = doc.FirstChildElement("Level")->FirstChildElement("BrickTypes")->FirstChildElement("BrickType")->NextSiblingElement()->NextSiblingElement()->NextSiblingElement();
	brickTypeElements->QueryStringAttribute("Id", &opBrickId);
	brickTypeElements->QueryStringAttribute("Texture", &opBrickTexture);
	brickTypeElements->QueryStringAttribute("HitPoints", &opBrickHealth);
	brickTypeElements->QueryStringAttribute("HitSound", &opHitSound);

	tinyxml2::XMLElement* bricksElements = doc.FirstChildElement("Level")->FirstChildElement("BrickTypes")->NextSiblingElement();
	level=bricksElements->GetText();
	cout << "\n"<<opBrickTexture;
	cout << "\n"<<opBrickHealth;
	cout << "\n"<<opHitSound;
	cout << "\n" << level;

	

	const float FPS = 60.0;
	int windowResX = 0;
	int windowResY = 0;
	GetDesktopResolution(windowResX, windowResY);
	float ballSpeed=7;
	int random = rand() % (90 - 60) + 60;
	float angle;
	float ballAngleMovementX;
	float ballAngleMovementY;
	bool gameover = false;
	float paddlePosX = (windowResX / 2) - (windowResX / 16);
	float paddlePosY = (windowResY *(28.0 / 30));
	int paddleSpeed = 10;
	int direction = 0;
	bool draw = false;
	float paddleSizeX = (windowResX / 12);
	float paddleSizeY = (windowResY / 64);
	float paddleRound = (windowResY / 256);
	float ballRadius = (windowResY / 64);
	bool ballMoving = false;
	float ballPosX = 0;
	float ballPosY = 0;
	float brickWidth = (float)((float)windowResX/(float)rowCount)*1.43;
	//Adjustable, assuming this many bricks
	float brickHeight = ((float)((float)windowResY / (float)columnCount))*0.5;
	ALLEGRO_DISPLAY *display;
	ALLEGRO_COLOR BallColor = al_map_rgb(255, 30,30);

	if(!al_init())
	{
		al_show_native_message_box(NULL, "ERROR", NULL, "Allegro failed to initialise", NULL, NULL);
		return -1;
	}
	al_install_audio();
	al_init_acodec_addon();
	cout<<al_is_audio_installed();
	ALLEGRO_SAMPLE *soundSample = al_load_sample("Hit_05.wav");
	al_reserve_samples(2);//Tu ide broj zvukova

	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	display = al_create_display(windowResX, windowResY);
	//al_set_window_position(display, 150, 100);
	al_set_window_title(display, "Breakout: Now more broken than the bricks");
	if (!display) 
	{
		
		al_show_native_message_box(NULL, "ERROR", NULL, "Display Window is not good", NULL, NULL);
	}
	al_init_primitives_addon();
	
	al_install_keyboard();

	ALLEGRO_KEYBOARD_STATE keyState;

	al_init_image_addon();



	ALLEGRO_BITMAP*softBrickBitmap = al_load_bitmap("Textures/Bricks/Soft.png");
	ALLEGRO_BITMAP*mediumBrickBitmap = al_load_bitmap("Textures/Bricks/Medium.png");
	ALLEGRO_BITMAP*hardBrickBitmap = al_load_bitmap("Textures/Bricks/Hard.png");
	ALLEGRO_BITMAP*opBrickBitmap = al_load_bitmap("Textures/Bricks/Impenetrable.png");
	ALLEGRO_BITMAP*nullBrickBitmap = al_load_bitmap("Textures/Bricks/NullBrick.png");
	ALLEGRO_BITMAP*paddleBitmap = al_load_bitmap("Textures/Paddles/Paddle.png");
	ALLEGRO_BITMAP*backgroundBitmap = al_load_bitmap("Textures/Boards/Board_01.png");	
	ALLEGRO_EVENT_QUEUE *eventQueue = al_create_event_queue();
	
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
	ALLEGRO_TIMER *timer = al_create_timer(1 / FPS);
	al_register_event_source(eventQueue, al_get_timer_event_source(timer));

	int i = 0;
	vector <Brick> bricksVector(0);
	while (level[i] != 0) {

		if (isalpha(level[i])) {
			switch (level[i])
			{
			case 'H':
				bricksVector.push_back(Brick(hardBrickTexture, hardHitSound, hardBreakScore, stoi(hardBrickHealth), hardBreakSound));
				break;
			
			case 'M':
				bricksVector.push_back(Brick(mediumBrickTexture, mediumHitSound, mediumBreakScore, stoi(mediumBrickHealth), mediumBreakSound));
				break;
			
			case 'S':
				bricksVector.push_back(Brick(softBrickTexture, softHitSound, softBreakScore, stoi(softBrickHealth), softBreakSound));
				break;
			case 'I':
				bricksVector.push_back(Brick(opBrickTexture, opHitSound, -1, -1));
				break;
			}
		}
		if (level[i] == '_')
			bricksVector.push_back(Brick());
		i++;
	}
	bricksVector.resize(columnCount*rowCount);



	al_start_timer(timer);
	al_play_sample(soundSample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
	while (!gameover) {
		ALLEGRO_EVENT events;
		al_wait_for_event(eventQueue, &events);
		if (events.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (events.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				gameover = true;
				break;
			case ALLEGRO_KEY_SPACE:
				if (!ballMoving) {
					ballMoving = true;
					random = rand() % (90 - 60) + 60;
					angle=(float)(random / 100.00)*3.14159f*2.0f;
					ballAngleMovementX=cosf(angle)*ballSpeed;
					ballAngleMovementY = sinf(angle)*ballSpeed;
					ballPosX = paddlePosX + (paddleSizeX / 2);
					ballPosY = paddlePosY - paddleSizeY;
				}
			}
		}
		if (events.type == ALLEGRO_EVENT_TIMER) {
			al_get_keyboard_state(&keyState);
			if (al_key_down(&keyState, ALLEGRO_KEY_LEFT))
			{
				if (!(paddlePosX - paddleSpeed < 0)) {
					 paddlePosX -= paddleSpeed; 
				}
			}
			if(al_key_down(&keyState,ALLEGRO_KEY_RIGHT))
			{
				if (!(paddlePosX + paddleSpeed > windowResX - paddleSizeX))
					paddlePosX += paddleSpeed;
				
			}
			if(ballMoving){
				//Ball hits walls
				if (ballPosX + ballAngleMovementX - ballRadius<=0|| ballPosX + ballAngleMovementX>=windowResX-ballRadius)
					ballAngleMovementX = ballAngleMovementX * (-1);
				//Ball hits ceiling
				if (ballPosY + ballAngleMovementY - ballRadius < 0)
					ballAngleMovementY = ballAngleMovementY * (-1);
				//Ball hits paddle
				if (ballPosX<paddlePosX + paddleSizeX && ballPosX>paddlePosX&&ballPosY > paddlePosY - paddleSizeY&&ballPosY<paddlePosY) {
					random = rand() % (90 - 60) + 60;
					angle =(float)(random / 100.00)*3.14159f*2.0f;
					ballAngleMovementX = cosf(angle)*ballSpeed;
					ballAngleMovementY = sinf(angle)*ballSpeed;
					ballSpeed += 0.1;
					al_play_sample(soundSample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);

				}
				int j = 0;
				int k = 0;
				bool isNull = false;
				for (int j = 0; j < columnCount; j++) {
					for (int k = 0; k < rowCount; k++) {
						if (bricksVector[columnCount*k + j].BrickType() == 1|| bricksVector[columnCount*k + j].BrickType() == 2|| bricksVector[columnCount*k + j].BrickType() == 3||bricksVector[columnCount*k + j].BrickType() == -1)
							isNull = false;
						else
							isNull = true;
						if(ballPosX + ballAngleMovementX - 2 * ballRadius<=brickWidth*j+j*brickWidth&&!isNull&&
							ballPosY + ballAngleMovementY - 2 * ballRadius>= brickWidth * (j-1) + j * brickWidth)
							bricksVector[columnCount*k + j].BrickHit();
							ballAngleMovementX = ballAngleMovementX * (-1);
						if ((ballPosY + ballAngleMovementY - 2 * ballRadius) <= (columnSpacing + brickHeight * k + k * columnSpacing) && !isNull &&
							(ballPosY + ballAngleMovementY - 2 * ballRadius) >= (columnSpacing + brickHeight * (k - 1) + k * columnSpacing)) {
							bricksVector[columnCount*k + j].BrickHit();
							ballAngleMovementY = ballAngleMovementY * (-1);
						}

					}
				}
				ballPosX += ballAngleMovementX;
				ballPosY += ballAngleMovementY;
				//Ball misses
				if (ballPosY + ballAngleMovementY >= windowResY - ballRadius) {
					ballMoving = false;
					ballAngleMovementX = cosf(angle)*ballSpeed;
					ballAngleMovementY = sinf(angle)*ballSpeed;

				}
				
			}
			draw = true;
		}
		if (draw) {
			draw = false;
			al_draw_scaled_bitmap(backgroundBitmap, 0, 0, 600, 100, 0, 0, windowResX, windowResY, 0);
			int j = 0;
			int k = 0;
			for (int j  = 0; j < columnCount; j++) {
				for (int k = 0; k < rowCount; k++){
					switch (bricksVector[columnCount*k + j].BrickType()) {
					case(-1):
						al_draw_scaled_bitmap(opBrickBitmap, 0, 0, 203, 103, rowSpacing + brickWidth * j + j * rowSpacing, columnSpacing + brickHeight * k + k * columnSpacing, brickWidth, brickHeight, ALLEGRO_FLIP_VERTICAL);
						break;
					case 0:
						al_draw_scaled_bitmap(nullBrickBitmap, 0, 0, 203, 103, rowSpacing + brickWidth * j + j * rowSpacing, columnSpacing + brickHeight * k + k * columnSpacing, brickWidth, brickHeight, ALLEGRO_FLIP_VERTICAL);
						break;
					case 1:
						al_draw_scaled_bitmap(softBrickBitmap, 0, 0, 203, 103, rowSpacing + brickWidth * j + j * rowSpacing, columnSpacing + brickHeight * k + k * columnSpacing, brickWidth, brickHeight, ALLEGRO_FLIP_VERTICAL);
						break;
					case 2:
						al_draw_scaled_bitmap(mediumBrickBitmap, 0, 0, 203, 103, rowSpacing + brickWidth * j + j * rowSpacing, columnSpacing + brickHeight * k + k * columnSpacing, brickWidth, brickHeight, ALLEGRO_FLIP_VERTICAL);
						break;
					case 3:
						al_draw_scaled_bitmap(hardBrickBitmap, 0, 0, 203, 103, rowSpacing + brickWidth * j + j * rowSpacing, columnSpacing + brickHeight * k + k * columnSpacing, brickWidth, brickHeight, ALLEGRO_FLIP_VERTICAL);
						break;
					}
				}
				al_draw_scaled_bitmap(paddleBitmap, 0, 0, 600, 100, paddlePosX, paddlePosY, paddleSizeX, paddleSizeY, ALLEGRO_FLIP_VERTICAL);

				if (!ballMoving) {
					al_draw_filled_circle(paddlePosX + (paddleSizeX / 2), paddlePosY - paddleSizeY, ballRadius, BallColor);
				}
				else {
					al_draw_filled_circle(ballPosX, ballPosY, ballRadius, BallColor);
					
				}
				//bricksVector[i];
			}
			al_flip_display();
		}
	}
	
	al_destroy_sample(soundSample);
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(eventQueue);
	al_destroy_bitmap(softBrickBitmap);
	cin>>i;
	return 0;
}

