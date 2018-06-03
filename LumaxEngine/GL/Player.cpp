
#include "Player.hpp"

Player::Player(Camera* camera, float speed)
	: camera(camera), speed(speed)
{
	cameraAxis[FORWARD] = cameraAxis[BACKWARD] = &camera->walk;
	cameraAxis[RIGHT] = cameraAxis[LEFT] = &camera->right;
	cameraAxis[UP] = cameraAxis[DOWN] = &camera->up;
	cameraAxis[REAL_FORWARD] = cameraAxis[REAL_BACKWARD] = &camera->forward;
}

void Player::registerAxisControl(InputControl *control, AxisControl axis){
	controls.push_back(std::pair<InputControl*, AxisControl>(control, axis));
}

void Player::input(){
	Vec3 movement(0, 0, 0);
	for(int i = 0; i < controls.size(); ++i){
		movement += *cameraAxis[controls[i].second] * speed * controls[i].first->getAmount();
	}
	
	camera->position += movement;
}

void Player::update(){
	camera->update();
}
