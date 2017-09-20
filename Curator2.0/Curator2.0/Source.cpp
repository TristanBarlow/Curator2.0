#include "Source.h"


xy TerrainGenerate(int Seed, int xSize, int ySize)
{
	int tempX =0;
	int tempY= 0;
	for (int j = 0; j < ySize; j++) 
	{
		for (int i = 0; i < xSize; i++)
		{
			Wall wall;
			wall.xPos = i * xTileSize;
			wall.yPos = j * yTileSize;
			if (wall.xPos > tempX)
			{
				tempX = wall.xPos;
			}
			if (wall.yPos > tempY)
			{
				tempY = wall.yPos;
			}
			wall.wallNum = i;
			wallVecP.push_back(wall);

		}
	}
	cout << tempX <<endl ;
	cout << tempY <<endl ;
	return xy{ tempX,tempY };
}
int main(int argc, char* args[])
{
	if (!init()) 
	{
		cout << "init failed";
	}
	int xSize = 100;
	int ySize = 10;
	xy max = TerrainGenerate(0,xSize, ySize);
	deleter(&wallVecP, max.x, max.y);
	Player playerOne;
	SDL_Texture* wallImage = loadTexture("Wall.png");
	SDL_Texture* background = loadTexture("background.jpg");

	while (gPlay) 
	{
		SDL_RenderClear(gRenderer);
		BackgroundDraw(background);
		Event(playerOne);
		playerOne.drawPlayer();
		for (int i = 0; i < wallVecP.size(); i++)
		{
			if (wallVecP[i].DrawWall(wallImage))
			{
			}
		}
		for (int i = 0; i < BulletVec.size(); i++)
		{
			BulletVec[i].DrawBullet();
			if (BulletVec[i].destroyMe) 
			{
				BulletVec.erase(BulletVec.begin()+i);
				cout << BulletVec.size() << endl;
			}
		}

		SDL_RenderPresent(gRenderer);
	}
	return 0;
}
