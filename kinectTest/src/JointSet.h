#pragma once

#include "ofMain.h"
#include "IntegratorVec3f.h"
#include "ofxKinectForWindows2.h"

using namespace ofxKFW2;

class JointSet
{
public:
	JointSet(void);
	JointSet(std::map<JointType, ofVec3f> _locs, bool bzero = false);
	void setRandomStartingPoints();
	void update(std::map<JointType, ofVec3f> _locs);
	std::map<JointType, ofVec3f> getVals(void);
	void setAllZs(float val);
	std::pair<bool, ofVec3f> detectHighFives(JointSet otherJoints, float threshold = 10.0f);
	std::pair<bool, ofVec3f> detectClap(float threshold = 10.0f);
	bool isClapping(void);
	std::map<JointType, ofVec3f> jointLocs;
private:
	ofVec3f getRandOffscreenLoc();
	float getRandOffscreenVal(float dimension);
	bool clapping;
};