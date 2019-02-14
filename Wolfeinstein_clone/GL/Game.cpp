
#include "Game.hpp"

#include "../games/GameDemo.hpp"

// Insert your game class name here!
typedef GameDemo GameType;

// Game stored here
GameType Game::game;

void Game::staticInits(){
	// Statically init dependencies
	GameType::staticInits();
}

void Game::initGame(){
	// Initialized the game
	game.init();
}

void Game::input(){
	game.input();
}

void Game::update(){
	game.update();
}

void Game::render(){
	game.render();
}
