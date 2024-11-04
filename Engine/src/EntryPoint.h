#pragma once

ifdef ENGINE_PLATFORM_WINDOWS
extern GameEngine::Engine* GameEngine::CreateApplication();
int main(int argc, char* argv) {
	auto game = GameEngine::CreateApplication();
	game->start();
	delete game;
}
#else
	#error Engine only supports Windows!
#endif