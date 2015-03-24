#include "Costume.h"

//--------
Costume::Costume(void)
{
	//each Costume maps the name sof each segment to the pair of joints that it is connected to
	//the first value of the Joint pair is the position and the seconf value is the target
	bonePairs["head"] = std::make_pair(JointType::JointType_Neck, JointType::JointType_Head);
	bonePairs["body"] = std::make_pair(JointType::JointType_SpineBase, JointType::JointType_Neck);
	bonePairs["leftUpperArm"] = std::make_pair(JointType::JointType_ShoulderLeft, JointType::JointType_ElbowLeft);
	bonePairs["leftLowerArm"] = std::make_pair(JointType::JointType_ElbowLeft, JointType::JointType_WristLeft);
	bonePairs["leftHand"] = std::make_pair(JointType::JointType_WristLeft, JointType::JointType_HandTipLeft);
	bonePairs["rightUpperArm"] = std::make_pair(JointType::JointType_ShoulderRight, JointType::JointType_ElbowRight);
	bonePairs["rightLowerArm"] = std::make_pair(JointType::JointType_ElbowRight, JointType::JointType_WristRight);
	bonePairs["rightHand"] = std::make_pair(JointType::JointType_WristRight, JointType::JointType_HandTipRight);
	bonePairs["leftUpperLeg"] = std::make_pair(JointType::JointType_HipLeft, JointType::JointType_KneeLeft);
	bonePairs["leftLowerLeg"] = std::make_pair(JointType::JointType_KneeLeft, JointType::JointType_AnkleLeft);
	bonePairs["rightUpperLeg"] = std::make_pair(JointType::JointType_HipRight, JointType::JointType_KneeRight);
	bonePairs["rightLowerLeg"] = std::make_pair(JointType::JointType_KneeRight, JointType::JointType_AnkleRight);
	//initialize the map of segments
	segments = std::map<std::pair<JointType, JointType>, Segment>();
}

//--------
Costume::~Costume(void)
{
}

//--------
void Costume::init(std::map<std::string, Segment> _segments) {
	std::map<std::string, Segment>::iterator segment;
	for(segment = _segments.begin(); segment != _segments.end(); segment++) {
		segments[bonePairs[segment->first]] = segment->second;
	}
}

//--------
void Costume::init(std::vector<Segment> _segments) {
	std::vector<Segment>::iterator segment;
	for(segment = _segments.begin(); segment != _segments.end(); segment++) {
		segments[bonePairs[segment->getPart()]] = *segment;
	}
}

//--------
void Costume::init(Segment _segment) {
	std::map<std::string, std::pair<JointType, JointType>>::iterator bonePair;
	for(bonePair = bonePairs.begin(); bonePair != bonePairs.end(); bonePair++) {
		segments[bonePair->second] = _segment;
	}
}

//--------
void Costume::draw() {
	if(visible) {
		std::map<std::pair<JointType, JointType>, Segment>::iterator segment;
		for(segment = segments.begin(); segment != segments.end(); segment++) {
			segment->second.draw();
		}
		visible = false;
	}
}

//--------
void Costume::update(std::map<JointType, ofVec3f> _jointPositions) {
	std::map<std::pair<JointType, JointType>, Segment>::iterator segment;
	for(segment = segments.begin(); segment != segments.end(); segment++) {
		ofVec3f pos = _jointPositions.find(segment->first.first)->second;
		ofVec3f tar = _jointPositions.find(segment->first.second)->second;
		ofVec3f diff = pos - tar;
		float dist = diff.length();
		float scale = ofMap(dist, 0, 200, 0, 1); //MAGIC NUMBERS!!!!
		segment->second.update(pos, tar, scale);
	}
	visible = true;
}

//--------
void Costume::set(std::string part, Segment seg) {
	seg.setPos(segments[bonePairs[part]].pos);
	seg.setTar(segments[bonePairs[part]].target);
	seg.setScale(segments[bonePairs[part]].scale);

  	segments[bonePairs[part]] = seg;
}

std::map<std::pair<JointType, JointType>, Segment> Costume::getSegments() {
	return segments;
}



