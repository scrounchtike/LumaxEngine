
#ifndef INPUT_CONTROL_HPP
#define INPUT_CONTROL_HPP

#include "input.hpp"

class InputControl {
public:
	InputControl(unsigned key, float weight = 1.0f)
		: key(key), weight(weight)
	{ }
	
	float getAmount() const { return (float)Input::keyDown(key) * weight; }
	bool getAction() const { return Input::keyDownOnce(key); }
	
	unsigned int key;
	float weight;
};

#endif
