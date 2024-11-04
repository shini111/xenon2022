#pragma once
#include "Engine.h"
#include <string>

class Xenon2022
{
public:
	GameEngine::Engine engine;
	Input input;
	float* GetGlobalRotation();
	int main();
private:
	float globalRotation = 90.0f;
};
