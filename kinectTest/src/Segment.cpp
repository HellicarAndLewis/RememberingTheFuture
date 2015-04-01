//
//  Segment.cpp
//  assimpLoaderExample
//
//  Created by James Bentley on 3/2/15.
//
//

#include "Segment.h"

//------ initialize integrators
Segment::Segment() {
 //   pos = IntegratorVec3f();
	//target = IntegratorVec3f();
	//scale = Integratorf();
}

//------ set initial values
void Segment::init(vector<std::pair<ofMesh, ofTexture>> _blocks, std::string _part, ofVec3f _pos, ofVec3f _target, float _scale) {
	blocks = _blocks;
	pos = _pos;
	target = _target;
	scale = _scale;
 //   pos = IntegratorVec3f(_pos);
 //   target = IntegratorVec3f(_target);
	//scale = Integratorf(_scale);
	part = _part;
}

//------ draw the segment
void Segment::draw() {
	ofPushStyle();
    ofPushMatrix();
		ofTranslate(pos.x, pos.y, pos.z); //translate to position
		ofScale(scale, scale, scale); //scale the the appropriate size
		//get the necessary rotation for the angle between positiona nd target
		ofQuaternion q = getQuaternion(pos-target, ofVec3f(0, 0, 1));
		float qangle;
		ofVec3f qvec;
		q.getRotate(qangle, qvec);
		ofRotate(qangle, qvec.x, qvec.y, qvec.z); //apply rotation
		//iterate over meshes and draw each one with the appropriate texture
		vector<std::pair<ofMesh, ofTexture>>::iterator blockIt;
		for(blockIt = blocks.begin(); blockIt != blocks.end(); blockIt++) {
			blockIt->second.bind();
			blockIt->first.draw();
			blockIt->second.unbind();
		}
    ofPopMatrix();
	ofPopStyle();
}

//------ draw the segment
void Segment::draw(ofVec3f _pos, ofVec3f _tar, float _scale) {
	ofPushStyle();
    ofPushMatrix();
		ofTranslate(_pos.x, _pos.y, _pos.z); //translate to position
		ofScale(_scale, _scale, _scale); //scale the the appropriate size
		//get the necessary rotation for the angle between positiona nd target
		ofQuaternion q = getQuaternion(_pos-_tar, ofVec3f(0, 0, 1));
		float qangle;
		ofVec3f qvec;
		q.getRotate(qangle, qvec);
		ofRotate(qangle, qvec.x, qvec.y, qvec.z); //apply rotation
		//iterate over meshes and draw each one with the appropriate texture
		vector<std::pair<ofMesh, ofTexture>>::iterator blockIt;
		for(blockIt = blocks.begin(); blockIt != blocks.end(); blockIt++) {
			blockIt->second.bind();
			blockIt->first.draw();
			blockIt->second.unbind();
		}
    ofPopMatrix();
	ofPopStyle();
}

//------
void Segment::setScale(float newScale) {
	//scale.set(newScale);
	scale = newScale;
}

//------
void Segment::setPos(ofVec3f newPos) {
	//pos.set(newPos);
	pos = newPos;
}

//------
void Segment::setTar(ofVec3f newTar) {
	//target.set(newTar);
	target = newTar;
}

//------
void Segment::tarPos(ofVec3f newPos) {
	//pos.target(newPos);
	pos = newPos;
}

//------
void Segment::tarTar(ofVec3f newTar) {
	//target.target(newTar);
	target = newTar;
}

//------
void Segment::tarScale(float newScale ) {
	//scale.target(newScale);
	scale = newScale;
}

//------ update each segment (use Integrator target for smoother movement)
void Segment::update(ofVec3f _pos, ofVec3f _tar, float _scale) {
	//tarPos(_pos);
	//tarTar(_tar);
	//tarScale(_scale);
	//target.update();
	//pos.update();
	//scale.update();
	setPos(_pos);
	setTar(_tar);
	setScale(_scale);
	//target.update();
	//pos.update();
	//scale.update();
}

//------ internal function for finding rotation
ofQuaternion Segment::getQuaternion(ofVec3f target, ofVec3f up) {
    ofVec3f zaxis = -target.getNormalized();
    if(zaxis.length() > 0) {
        ofVec3f xaxis = up.getCrossed(zaxis).getNormalized();
        ofVec3f yaxis = zaxis.getCrossed(xaxis);

        ofMatrix4x4 m;
        m._mat[0].set(xaxis.x, xaxis.y, xaxis.z, 0);
        m._mat[1].set(yaxis.x, yaxis.y, yaxis.z, 0);
        m._mat[2].set(zaxis.x, zaxis.y, zaxis.z, 0);
        return m.getRotate();
    }
}

//------ draw the debug mode (position in red target in green)
void Segment::drawDebug() {
    ofPushMatrix();
    ofPushStyle();
		ofSetColor(255, 0, 0);
		ofDrawSphere(pos.x, pos.y, pos.z, 10);
		ofSetColor(0, 255,  0);
		ofDrawSphere(target.x, target.y, target.z, 10);
		ofLine(pos.x, pos.y, pos.z, target.x, target.y, target.z);
    ofPopStyle();
}

//------ return the name of the body part each segment is associated with
std::string Segment::getPart() {
	return part;
}
