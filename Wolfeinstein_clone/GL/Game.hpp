
#ifndef GAME_HPP
#define GAME_HPP

class GameDemo;

class Game {
public:
	static void staticInits();
	static void initGame();
	
	static void input();
	static void update();
	static void render();
	
private:
	static GameDemo game;
};

#endif
