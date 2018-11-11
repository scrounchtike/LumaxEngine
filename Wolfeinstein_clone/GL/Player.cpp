
#include "Player.hpp"

#include "../lumax.hpp"

Player::Player(float speed)
	: speed(speed)
{
	cameraAxis[FORWARD] = cameraAxis[BACKWARD] = &getCamera()->walk;
	cameraAxis[RIGHT] = cameraAxis[LEFT] = &getCamera()->right;
	cameraAxis[UP] = cameraAxis[DOWN] = &getCamera()->up;
	cameraAxis[REAL_FORWARD] = cameraAxis[REAL_BACKWARD] = &getCamera()->forward;
}

void Player::registerAxisControl(InputControl *control, AxisControl axis){
	controls.push_back(std::pair<InputControl*, AxisControl>(control, axis));
}

void Player::input(){
	Vec3 movement(0, 0, 0);
	for(int i = 0; i < controls.size(); ++i){
		movement += *cameraAxis[controls[i].second] * speed * controls[i].first->getAmount();
	}

	getCamera()->update(movement);
	//getCamera()->position += movement;
}

void Player::update(){
	getCamera()->update();
}

Camera* Player::getCamera() const {
	return lmx::getStaticCamera();
}
