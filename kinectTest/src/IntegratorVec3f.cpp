#include "IntegratorVec3f.h"


IntegratorVec3f::IntegratorVec3f(void)
{
	mass = 1;
	damping = DAMPING;
	attraction = ATTRACTION;
}

IntegratorVec3f::IntegratorVec3f(ofVec3f _val) {
	val = _val;
	mass = 1;
	damping = DAMPING;
	attraction = ATTRACTION;
}

IntegratorVec3f::IntegratorVec3f(ofVec3f _val, float _damping, float _attraction) {
	val = _val;
	damping = _damping;
	attraction = _attraction;
	mass = 1;
}

void IntegratorVec3f::set(ofVec3f _val) {
	val = _val;
}

void IntegratorVec3f::setX(float _val) {
	val.x = _val;
}

void IntegratorVec3f::setY(float _val) {
	val.y = _val;
}

void IntegratorVec3f::setZ(float _val) {
	val.z = _val;
}

void IntegratorVec3f::update(void) {
	if(targeting) {
		force += attraction * (tar - val);
	}
	acc = force / mass;
	vel = (vel + acc) * damping;
	val += vel;

	force = ofVec3f(0, 0, 0);
}

void IntegratorVec3f::target(ofVec3f _tar) {
	targeting = true;
	tar = _tar;
}
	
void IntegratorVec3f::noTarget() {
	targeting = false;
}


IntegratorVec3f::~IntegratorVec3f(void)
{
}
