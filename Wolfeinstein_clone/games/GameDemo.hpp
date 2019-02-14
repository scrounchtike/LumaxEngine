
#ifndef GAME_DEMO_HPP
#define GAME_DEMO_HPP

class Model3D;
class Shader;
class Camera;
class Texture;

class GameDemo {
public:
	static void staticInits();
  void init();
	
	void input();
	void update();
	void render() const;
	
private:
	Model3D* cube;
	Shader* shader3D;
	Texture* textureTest;
	
	Camera* camera;
};

#endif
