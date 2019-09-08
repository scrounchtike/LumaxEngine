
#include "Player.hpp"

Player::Player(Camera* camera, float speed)
	: speed(speed), camera(camera)
{
	cameraAxis[FORWARD] = cameraAxis[BACKWARD] = &camera->walk;
	cameraAxis[RIGHT] = cameraAxis[LEFT] = &camera->right;
	Vec3* yAxis = new Vec3(0,1,0);
	cameraAxis[UP] = cameraAxis[DOWN] = yAxis; // y axis
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
