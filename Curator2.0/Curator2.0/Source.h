#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include<iostream>
#include <vector>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
const int FRAMES_PER_SECOND = 20;
const int xTileSize = 100;
const int yTileSize = 100;

struct vec 
{
	float xVector;
	float yVector;
};

struct xy {
	int x;
	int y;
};

class Camera 
{	
	public:
		int xPos = 0;
		int yPos = 0;
};

class Player 
{	public:
		int xPos;
		int yPos;
		int xScreen = SCREEN_WIDTH / 2;
		int yScreen = SCREEN_HEIGHT / 2;
		int width = 100;
		int height = 100;
		SDL_Texture* image;
		SDL_Rect playerSrcRect = { 0,0,1000,1000 };
		Player();
		void drawPlayer();

};

class Wall
{
public:
	int wallNum;
	int xPos = 0;
	int yPos = 0;
	int width = yTileSize;
	int height = xTileSize;
	SDL_Rect WallSrcRect = { 0,0,xTileSize,yTileSize };
	bool DrawWall(SDL_Texture* image);
};

class Bullet 
{
	public:
		float xPos;
		float yPos;
		int screenX;
		int screenY;
		int width = 10;
		int height = 5 ;
		float xVelocity;
		float yVelocity;
		bool destroyMe;
		SDL_Rect SrcRect;
		SDL_Rect DestRect;
		SDL_Texture* image;
		Bullet();
		void DrawBullet();
		void BulletHit();
};



bool gPlay = true;


SDL_Window* gWindow = NULL;


SDL_Surface* gScreenSurface = NULL;

SDL_Renderer* gRenderer = NULL;

SDL_Texture* bitmapTex = NULL;

bool init();

void Close();

bool Event(Player);

void BackgroundDraw(string);

void DrawImage(SDL_Rect*, SDL_Rect*, SDL_Texture*);

void deleter(vector<Wall>*,int, int);

void deleterMove(int*, int*, int );

vec CalcDirectionalVector(int,int,int,int);


SDL_Texture* loadTexture(string);

Camera camera;
vector <Wall> wallVecP;
vector <Bullet> BulletVec;



void Player::drawPlayer() 
{
	xPos = camera.xPos + xScreen;
	yPos = camera.yPos + yScreen;
	SDL_Rect playerDestRect = {xScreen, yScreen, width, height };
	DrawImage(&playerSrcRect, &playerDestRect, image);
	
}

Player::Player()
{
	image = loadTexture("man.png");
}

bool Wall::DrawWall(SDL_Texture* image) 
{
	{
		int screenX = xPos - camera.xPos;
		int screenY = yPos - camera.yPos;
		if (screenX < SCREEN_WIDTH + xTileSize && screenX > -xTileSize)
		{
			if (screenY < SCREEN_HEIGHT + yTileSize && screenY > -yTileSize)
			{
				SDL_Rect WallDestRect = { xPos - camera.xPos, yPos - camera.yPos, width, height };
				DrawImage(&WallSrcRect, &WallDestRect, image);
				return true;
			}
		}
		return false;
	}
};

void Bullet::DrawBullet() 
{

	SrcRect = { 0,0,256,130 };
	xPos = xPos + xVelocity;
	yPos = yPos + yVelocity;
	screenX = xPos - camera.xPos;
	screenY = yPos - camera.yPos;
	DestRect = {screenX ,screenY , width, height };
	if (screenX < SCREEN_WIDTH + 100 && screenX > -100)
	{
		if (screenY < SCREEN_HEIGHT + 100 && screenY > -100)
		{
			DrawImage(&SrcRect, &DestRect, image);
		}
		else
		{
			destroyMe = true;
		}
	}
	else 
	{
		destroyMe = true;
	}
}

Bullet::Bullet() 
{
	image = loadTexture("bullet.png");
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
		}

		//Create window
		gWindow = SDL_CreateWindow("MyGame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					success = false;
				}
			}
		}
	}

	return success;
}

void Close()
{
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	gPlay = false;
	SDL_Quit();
}

bool Event(Player player)
{
	SDL_Event e;
	if (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			Close();
		}
		if (e.type == SDL_MOUSEBUTTONDOWN)
		{

			Bullet bullet;
			bullet.xPos = player.xPos;
			bullet.yPos = player.yPos;
			vec nDirectionVec = CalcDirectionalVector(player.xScreen, player.yScreen, e.button.x, e.button.y);
			bullet.xVelocity = -nDirectionVec.xVector*2;
			bullet.yVelocity = -nDirectionVec.yVector*2;
			BulletVec.push_back(bullet);
	
		}
		if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				camera.yPos -= 30;
				return true;

			case SDLK_DOWN:
				camera.yPos += 30;
				return true;

			case SDLK_LEFT:
				camera.xPos -= 30;
				return true;

			case SDLK_RIGHT:
				camera.xPos += 30;
				return true;
			}
		}
	}

}

SDL_Texture* loadTexture(string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

void BackgroundDraw(SDL_Texture* image)
{
	SDL_Rect SrcRect = { 0, 0,1920,1080 };
	SDL_Rect destRect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	DrawImage(&SrcRect, &destRect, image);
}

void DrawImage(SDL_Rect* SrcRect, SDL_Rect* DestRect, SDL_Texture* image)
{
	SDL_RenderCopy(gRenderer, image, SrcRect, DestRect);
}

void deleter(vector<Wall>* RawTerrain, int xSize, int ySize = 1000) 
{
	bool deleter = true;
	int xPos = xTileSize;
	int yPos = yTileSize;
	int itterations =0;
	vector<Wall>& RawTerrainRef = *RawTerrain;
	while (deleter)
	{

		itterations ++;
		deleterMove(&xPos, &yPos, ySize);
		for (int i = 0; i < RawTerrain->size(); i++)
		{
			if (xPos == RawTerrainRef[i].xPos)
			{
				if (yPos == RawTerrainRef[i].yPos)
				{
					RawTerrain->erase(RawTerrain->begin() + i);
				}
			}
		}
		if (yPos > ySize || xPos > xSize - 2* xTileSize)
		{
			deleter = false;
		}

	}
	cout << itterations;
}

void deleterMove(int* xPos,int* yPos, int ySize)
{

	int randNum = rand() % 100;
	float yProb = (*yPos* 100)/ySize;
	cout << "yprob:" << yProb << endl;
	int minYprob = yProb;
	int addYprob = 90;
	int moveXprob = 100;
	if (randNum < minYprob)
	{
		if (*yPos > yTileSize)
		{
			*yPos -= yTileSize;
		}
		return;
	}
	if (randNum < addYprob)
	{
		if (*yPos < ySize - yTileSize)
		{
			*yPos += yTileSize;
			
		}
		return;
	}
	if (randNum < moveXprob)
	{

		*xPos += xTileSize;
		return;
	}
}

vec CalcDirectionalVector(int xVecA, int yVecA, int xVecB, int yVecB) 
{
	vec DirectionalVector;
	int tempX = xVecA - xVecB;
	int tempY = yVecA - yVecB;
	int temp = (tempX*tempX) + (tempY*tempY);
	double magnitude = sqrt(temp);
	DirectionalVector.xVector = tempX / magnitude;
	DirectionalVector.yVector = tempY / magnitude;
	return DirectionalVector;
}

