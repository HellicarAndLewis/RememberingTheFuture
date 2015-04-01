#pragma once

#include "ofMain.h"
#include "Segment.h"
#include "ofxKinectforWindows2.h"

using namespace ofxKFW2;

class Costume
{
public:
	Costume(void);
	~Costume(void);
	//several initialization functions
	void init(std::string input); //initialize with one costume
	void initRandSensible(std::map<std::pair<std::string, std::string>, Segment>* allSegments); //initialize with random parts, sensible means shoulder -> shoulder head -> head etc.
	void initRandTotal(std::map<std::pair<std::string, std::string>, Segment>* allSegments); //inititalize with random parts, total means full random so any part can go anywhere
	void setPositions(std::map<JointType, ofVec3f> _jointPositions); //set the positions to a particular place
	void set(std::string part, std::pair<std::string, std::string> seg); //set a particular body part to a different segment
	void draw(std::map<std::pair<std::string, std::string>, Segment>* allSegments); //draw the costume (only works if update has been called)
	void update(std::map<JointType, ofVec3f> _jointPositions); //update the costume with a set of joint positions
	std::map<std::string, std::pair<std::string, std::string>> getSegments(); //return the segments

private:
	std::map<std::string, std::pair<std::string, std::string>> segments; //all the segments
	std::map<std::string, std::pair<JointType, JointType>> bonePairs; //all the bone pairs
	std::map<JointType, IntegratorVec3f> myJointPositions;
	bool visible; //whether or not the costume is visible (turned on by update turned off by draw so only draws if it's been updated)
};

