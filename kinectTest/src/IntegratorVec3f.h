#pragma once

#include "ofMain.h"

#define DAMPING 0.5f
#define ATTRACTION 0.1f

class IntegratorVec3f
{
public:
	IntegratorVec3f(void);
	IntegratorVec3f(ofVec3f _val);
	IntegratorVec3f(ofVec3f _val, float _damping, float _attraction);
	void set(ofVec3f _val);
	void setX(float _val);
	void setY(float _val);
	void setZ(float _val);

	void update(void);
	void target(ofVec3f _tar);
	void noTarget();
	~IntegratorVec3f(void);

	ofVec3f val;
	ofVec3f vel;
	ofVec3f acc;
	ofVec3f force;
	ofVec3f tar;
	float mass;

	float damping;
	float attraction;
	bool targeting;

};

