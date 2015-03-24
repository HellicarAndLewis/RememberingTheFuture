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
	void init(std::map<std::string, Segment> _segments); //takes direct segments
	void init(Segment _segment); //takes one segment and uses that for everything
	void init(std::vector<Segment> _segments); //takes a list of segments
	void set(std::string part, Segment seg); //set a particular body part to a different segment
	void draw(); //draw the costume (only works if update has been called)
	void update(std::map<JointType, ofVec3f> _jointPositions); //update the costume with a set of joint positions
	std::map<std::pair<JointType, JointType>, Segment> getSegments();

private:

	std::map<std::pair<JointType, JointType>, Segment> segments; //all the segments
	std::map<std::string, std::pair<JointType, JointType>> bonePairs; //all the vone pairs
	bool visible; //whether or not the costume is visible (turned on by update turned off by draw so only draws if it's been updated)
};

