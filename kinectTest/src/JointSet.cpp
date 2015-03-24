#pragma once

#include "JointSet.h"

JointSet::JointSet(void) {
	jointLocs = std::map<JointType, IntegratorVec3f>();
}

JointSet::JointSet(std::map<JointType, ofVec3f> _locs, bool bzero) {
	std::map<JointType, ofVec3f>::iterator loc;
	for(loc = _locs.begin(); loc != _locs.end(); loc++) {
		if(bzero) jointLocs[loc->first] = IntegratorVec3f(ofVec3f(0, 0, 0));
		else jointLocs[loc->first] = IntegratorVec3f(loc->second);
	}
}

void JointSet::update(std::map<JointType, ofVec3f> _locs) {
	std::map<JointType, IntegratorVec3f>::iterator jointLoc;
	for(jointLoc = jointLocs.begin(); jointLoc != jointLocs.end(); jointLoc++) {
		jointLoc->second.target(_locs.find(jointLoc->first)->second);
		jointLoc->second.update();
	}
}

std::map<JointType, ofVec3f> JointSet::getVals() {
	std::map<JointType, ofVec3f> returnMap;
	std::map<JointType, IntegratorVec3f>::iterator jointLoc;
	for(jointLoc = jointLocs.begin(); jointLoc != jointLocs.end(); jointLoc++) {
		returnMap[jointLoc->first] = jointLoc->second.val;
	}
	return returnMap;
}

void JointSet::setAllZs(float val) {
	std::map<JointType, IntegratorVec3f>::iterator jointLoc;
	 for(jointLoc = jointLocs.begin(); jointLoc != jointLocs.end(); jointLoc++) {
		 jointLoc->second.setZ(val);
	 }
}

void JointSet::setRandomStartingPoints() {
	 std::map<JointType, IntegratorVec3f>::iterator jointLoc;
	 for(jointLoc = jointLocs.begin(); jointLoc != jointLocs.end(); jointLoc++) {
		 jointLoc->second = IntegratorVec3f(getRandOffscreenLoc());
	 }
}

ofVec3f JointSet::getRandOffscreenLoc() {
	ofVec3f returnVec;
	returnVec.x = getRandOffscreenVal(ofGetWidth());
	returnVec.y = getRandOffscreenVal(ofGetHeight());
	returnVec.z = 0;
	return returnVec;
}

float JointSet::getRandOffscreenVal(float dimension) {
	float deciderator = ofRandom(1.0f);
	float val;
	if(deciderator < 0.5f) val = ofRandom(-dimension, 0);
	else val = ofRandom(dimension, 2*dimension);
	cout<<val<<endl;
	return val;
}

std::pair<bool, ofVec3f> JointSet::detectHighFives(JointSet otherJoints, float threshold) {

	ofVec3f thisLeftHand;
	ofVec3f thisRightHand;
	ofVec3f thatLeftHand;
	ofVec3f thatRightHand;

	thisLeftHand = jointLocs.find(JointType::JointType_HandLeft)->second.val;
	thisRightHand = jointLocs.find(JointType::JointType_HandRight)->second.val;
	
	thatLeftHand = otherJoints.jointLocs.find(JointType::JointType_HandLeft)->second.val;
	thatRightHand = otherJoints.jointLocs.find(JointType::JointType_HandRight)->second.val;

	ofVec3f diff = thisLeftHand - thatRightHand;
	float dist1 = diff.length();

	diff = thisLeftHand - thatLeftHand;
	float dist2 = diff.length();

	diff = thisRightHand - thatRightHand;
	float dist3 = diff.length();

	diff = thisRightHand - thatLeftHand;
	float dist4 = diff.length();

	float minDist = min(min(dist1, dist2), min(dist3, dist4));

	if(minDist < threshold) {
		ofVec3f mid;
		if(minDist == dist1) {
			mid = ofVec3f(	(thisLeftHand.x + thatRightHand.x)/2, 
							(thisLeftHand.y + thatRightHand.y)/2,  
							(thisLeftHand.z + thatRightHand.z)/2);
		} else if(minDist == dist2) {
			mid = ofVec3f(	(thisLeftHand.x + thatLeftHand.x)/2, 
							(thisLeftHand.y + thatLeftHand.y)/2,  
							(thisLeftHand.z + thatLeftHand.z)/2);
		} else if(minDist == dist3) {
			mid = ofVec3f(	(thisRightHand.x + thatRightHand.x)/2, 
							(thisRightHand.y + thatRightHand.y)/2,  
							(thisRightHand.z + thatRightHand.z)/2);
		} else if(minDist == dist4) {
			mid = ofVec3f(	(thisRightHand.x + thatLeftHand.x)/2, 
							(thisRightHand.y + thatLeftHand.y)/2,  
							(thisRightHand.z + thatLeftHand.z)/2);
		}
		return std::make_pair(true, mid);
	} else return std::make_pair(false, ofVec3f(0, 0, 0));
}

std::pair<bool, ofVec3f> JointSet::detectClap(float threshold) {
	ofVec3f leftHand;
	ofVec3f rightHand;

	leftHand = jointLocs.find(JointType::JointType_HandTipLeft)->second.val;
	rightHand = jointLocs.find(JointType::JointType_HandTipRight)->second.val;

	ofVec3f diff = leftHand - rightHand;
	ofVec3f mid = leftHand.middle(rightHand);
	float dist = diff.length();

	if(dist < threshold) {
		clapping = true;
		return std::make_pair(clapping, mid);
	} else {
		clapping = false;
		return std::make_pair(clapping, ofVec3f(0, 0, 0));
	}
}

bool JointSet::isClapping() {
	return clapping;
}

