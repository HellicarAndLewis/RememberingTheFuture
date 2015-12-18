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
}

//--------
Costume::~Costume(void)
{
}

void Costume::init(std::string input) {
	std::map<std::string, std::pair<JointType, JointType>>::iterator bonePair;
	for(bonePair = bonePairs.begin(); bonePair != bonePairs.end(); bonePair++) {
		segments[bonePair->first] = std::make_pair(input, bonePair->first);
		myJointPositions[bonePair->second.first] = ofVec3f();
		myJointPositions[bonePair->second.second] = ofVec3f();
	}
}

void Costume::initRandSensible(std::map<std::pair<std::string, std::string>, Segment>* allSegments) {
	std::map<std::string, std::pair<JointType, JointType>>::iterator bonePair;
	for (bonePair = bonePairs.begin(); bonePair != bonePairs.end(); bonePair++) {
		auto it = allSegments->begin();
		std::advance(it, rand() % allSegments->size());
		segments[bonePair->first] = std::make_pair(it->first.first, bonePair->first);
		myJointPositions[bonePair->second.first] = ofVec3f();
		myJointPositions[bonePair->second.second] = ofVec3f();
	}
}

void Costume::initRandTotal(std::map<std::pair<std::string, std::string>, Segment>* allSegments) {
	std::map<std::string, std::pair<JointType, JointType>>::iterator bonePair;
	for(bonePair = bonePairs.begin(); bonePair != bonePairs.end(); bonePair++) {
		auto it = allSegments->begin();
		std::advance(it, rand() % allSegments->size());
		std::pair<std::string, std::string> newSegment = it->first;
		segments[bonePair->first] = std::make_pair(newSegment.first, bonePair->first);
		myJointPositions[bonePair->second.first] = ofVec3f();
		myJointPositions[bonePair->second.second] = ofVec3f();
	}
}

//--------
void Costume::draw(std::map<std::pair<std::string, std::string>, Segment>* allSegments) {
	if(visible) {
		std::map<std::string, std::pair<std::string, std::string>>::iterator segment;
		for(segment = segments.begin(); segment != segments.end(); segment++) {
			ofVec3f pos = myJointPositions[bonePairs[segment->first].first];
			ofVec3f tar = myJointPositions[bonePairs[segment->first].second];
			ofVec3f diff = pos - tar;
			float dist = diff.length();
			float scale;
			if(segment->first == "head")  scale = ofMap(dist, 0, 150, 0, 1); //MAGIC NUMBERS!!!!
			else scale = ofMap(dist, 0, 200, 0, 1);
			allSegments->find(segment->second)->second.draw(pos, tar, scale);
		}
		visible = false;
	}
}

//--------
void Costume::update(std::map<JointType, ofVec3f> _jointPositions) {
	std::map<JointType, ofVec3f>::iterator myJointPos;
	for(myJointPos = myJointPositions.begin();  myJointPos != myJointPositions.end(); myJointPos++) {
		ofVec3f target = _jointPositions.find(myJointPos->first)->second;
		myJointPos->second = target;
		myJointPos->second = target;
	}
	visible = true;
}

//--------
void Costume::set(std::string part, std::pair<std::string, std::string> seg) {
	segments[part] = seg;
}

std::map<std::string, std::pair<std::string, std::string>> Costume::getSegments() {
	return segments;
}

void Costume::setPositions(std::map<JointType, ofVec3f> _jointPositions) {
	std::map<JointType, ofVec3f>::iterator myJointPos;
	for(myJointPos = myJointPositions.begin();  myJointPos != myJointPositions.end(); myJointPos++) {
		ofVec3f target = _jointPositions.find(myJointPos->first)->second;
		myJointPos->second = target;
		myJointPos->second = target;
	}
}



