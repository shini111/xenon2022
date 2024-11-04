#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <array>
#include <Windows.h>

#include <SDL.h>
#include <box2d.h>

#include "Core.h"

#include "EntryPoint.h"


#include "Animator.h"
#include "GameLevel.h"
#include "GameObjects.h"

class Input {

public:

	std::vector <SHORT> ShootAction;
	std::vector <SHORT> MoveRight;
	std::vector <SHORT> MoveLeft;
	std::vector <SHORT> MoveUp;
	std::vector <SHORT> MoveDown;

	void init();
};


class Game {
public:

	void start();


private:
	int prevTime = currentTime;
	int currentTime = 0;
};


class GameWindow
{
public:
	const char* windowName = "Xenon 2022";
	int windowWidth = 640;
	int windowHeight = 480;
};

namespace GameEngine {
	class ENGINE_API Engine
	{
	public:
		float deltaTime = 0.0f;

		void setLevel(GameLevel level);
		GameLevel& getLevel();
		void print(std::string printText);
		void printFloat(float printFloat);
		void printFloatPointer(float* printFloatPointer);
		void Update();
		void Initialize(GameWindow windowSettings);


		//Defined in client
		Application* CreateApplication();

	private:
		GameLevel mainLevel;
		GameWindow windowDisplay;
		int prevTime = currentTime;
		int currentTime = 0;

	};
}
