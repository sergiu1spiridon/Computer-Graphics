#include "Enemy.h"

Enemy::Enemy(glm::vec3 initialPositionGiven)
{
	initialPos = initialPositionGiven;
	position = initialPositionGiven;
	direction = glm::vec3(0.01f, 0, 0.01f);
	isActive = true;
	time = 40;
}

void Enemy::SetNewPosition(int deltaTimeSeconds)
{
	float dirX[7] = { 0.01, -0.01, 0.008f, 0.02f, -0.02f, -0.08f, 0.4f };
	float dirY[7] = { 0.01, -0.01, 0.09f, 0.02f, -0.02f, -0.06f, -0.04f };

	position += direction;

	while (sqrt(pow(position[0] - initialPos[0], 2) + pow(position[2] - initialPos[2], 2)) > 1.2f) {
		position -= direction;

		int x = rand() % 7;
		int y = rand() % 7;
		direction = glm::vec3(dirX[x], 0, dirY[y]);
		
		position += direction;
	}

	return;
}

glm::vec3 Enemy::GetPosition()
{
	return position;
}

int Enemy::GetTime()
{
	return time;
}

void Enemy::DecreaseTime()
{
	time--;
}