#pragma once
#include "components/simple_scene.h"
#include <iostream>

class Enemy
{
protected:
	glm::vec3 initialPos;
	glm::vec3 position;
	glm::vec3 direction;
	int time;
public:
	Enemy(glm::vec3 initialPosition);
	void SetNewPosition(int deltaTimeSeconds);
	glm::vec3 GetPosition();
	int GetTime();
	void DecreaseTime();
	
	bool isActive;
};

