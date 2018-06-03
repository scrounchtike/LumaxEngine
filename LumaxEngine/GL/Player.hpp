
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../RAL/InputControl.hpp"
#include "../math.hpp"
#include "../RL/Camera.hpp"

#include <vector>
#include <iostream>
#include <map>

class Player {
public:
	enum AxisControl {
		FORWARD, BACKWARD, RIGHT, LEFT, UP, DOWN, REAL_FORWARD, REAL_BACKWARD
	};
	Player(Camera* camera, float speed = 0.35f);

	void registerAxisControl(InputControl* control, AxisControl axis);

	void input();
	void update();

	Camera* getCamera() const { return camera; }
	float speed;
private:
	Camera* camera;
	
	Vec3* cameraAxis[8];
	std::vector<std::pair<InputControl*, AxisControl> > controls;
};

#endif
