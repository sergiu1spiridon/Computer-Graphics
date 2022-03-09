#pragma once
#include <stdio.h>
#include <iostream>
#define N 50

class Maze
{
protected:
	int directionsX[4] = { 0, 1, 0, -1 };
	int directionsY[4] = { 1, 0, -1, 0 };

	void AddPathsDFS(int **matrix, int posX, int posY, int* stop);
	void AddPathsBFS(int** matrix, int posX, int posY, bool isMainThread);

public:
	Maze();
	int** getMaze(int **matrix);
	int initialXPos;
	int initialYPos;
};

