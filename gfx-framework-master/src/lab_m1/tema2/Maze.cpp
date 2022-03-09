#include "Maze.h"
#include <queue>  

Maze::Maze() {
	initialXPos = 2 + rand() % 8;
	initialYPos = 2 + rand() % 8;
}

int** Maze::getMaze(int **matrix) {

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			matrix[i][j] = 1;
		}
	}

	int stop = 1;
	//AddPathsDFS(matrix, initialXPos, initialYPos, &stop);
	AddPathsBFS(matrix, initialXPos, initialYPos, true);
	AddPathsBFS(matrix, 10 + rand() % 10, 10 + rand() % 10, false);
	matrix[(int)initialXPos][(int)initialYPos] = 2;

	return matrix;
}

void Maze::AddPathsDFS(int **matrix, int posX, int posY, int* stop) {
	bool createdAPath = false;

	if (posX == 0 || posY == 0) {
		return;
	}

	if (posX == N - 1 || posY == N - 1) {
		*stop = 0;
	}

	if (*stop == 0) {
		matrix[posX][posY] = 0;
		return;
	}

	matrix[posX][posY] = 0;
	int seted = 0;

	int dirValid[4] = { 0, 0, 0, 0 };
	for (int i = 0; i < 4; i++) {
		if (matrix[posX + directionsX[i]][posY + directionsY[i]] == 1 &&
			posX + directionsX[i] != 0 &&
			posY + directionsY[i] != 0)
		{
			seted = 1;;
			dirValid[i] = 1;
		}
	}

	if (seted == 1) {
		while (!createdAPath)
		{
			for (int i = 0; i < 4; i++) {
				if (dirValid[i] && rand() % 2 == 1)
				{
					createdAPath = true;

					AddPathsDFS(matrix, posX + directionsX[i], posY + directionsY[i], stop);
				}
			}
		}
	}
}

void Maze::AddPathsBFS(int** matrix, int posX, int posY, bool isMainThread) {
	std::queue<int> qx = *(new std::queue<int>());
	std::queue<int> qy = *(new std::queue<int>());
	bool foundTheExit = false;

	qx.push(posX);
	qy.push(posY);


	while (!qx.empty())
	{
		int createdPaths = 0;
		posX = qx.front();
		qx.pop();
		posY = qy.front();
		qy.pop();

		if (posX == 0 || posY == 0 || posY == N - 1) {
			continue;
		}

		if (posX == N - 1) {
			matrix[posX][posY] = 0;
			foundTheExit = true;
			break;
		}

		matrix[posX][posY] = 0;
		int seted = 0;

		if (rand() % 100 < 5 && sqrt(pow(posX - initialXPos, 2) + pow(posY - initialYPos, 2)) > 4) {
			matrix[posX][posY] = 3;
		}

		int dirValid[4] = { 0, 0, 0, 0 };
		for (int i = 0; i < 4; i++) {
			if (matrix[posX + directionsX[i]][posY + directionsY[i]] == 1 &&
				posX + directionsX[i] != 0 &&
				posY + directionsY[i] != 0)
			{
				seted = 1;;
				dirValid[i] = 1;
			}
		}


		int max;

		if (rand() % 100 < 2) {
			max = 3;
		}
		else if (rand() % 100 < 5) {
			max = 2;
		}
		else
		{
			max = 1;
		}
		
		if (seted == 1) {
			while (createdPaths == 0)
			{
				for (int i = 0; i < 4; i++) {
					if (dirValid[i] && rand() % 2 == 1 && createdPaths < max)
					{
						createdPaths++;

						qx.push(posX + directionsX[i]);
						qy.push(posY + directionsY[i]);
					}
				}
			}
		}
	}

	if (foundTheExit == false && isMainThread) {
		getMaze(matrix);
	}
}