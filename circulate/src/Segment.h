//
//  Segment.h
//  assimpLoaderExample
//
//  Created by James Bentley on 3/2/15.
//
//

#ifndef __assimpLoaderExample__Segment__
#define __assimpLoaderExample__Segment__

#include "ofMain.h"
#include "IntegratorVec3f.h"
#include "Integratorf.h"
#include "ofxAssimpModelLoader.h"

class Segment {
public:
    Segment();
    void draw(); //draw each segment
	void draw(ofVec3f _pos, ofVec3f _tar, float _scale);
	void update(ofVec3f _pos, ofVec3f _tar, float _scale); //update each segment with a position, a target and a scale
    void drawDebug(); //draw the target and position for debugging
    void init(vector<std::pair<ofMesh, ofTexture>> _blocks, std::string _part, ofVec3f _pos = ofVec3f(0, 0, 0), ofVec3f _target = ofVec3f(1, 1, 1), float _scale = 1.0f); //initialize the segment

	//return the name of the body part associated with this segment
	std::string getPart(void);

	//set the position, target and scale
	void setPos(ofVec3f newPos);
	void setTar(ofVec3f newTar);
	void setScale(float newScale);

	ofVec3f pos, target;
	float scale;

private:
	//target the positions target and scale
	void tarPos(ofVec3f newPos);
	void tarTar(ofVec3f newTar);
	void tarScale(float newScale);

    ofQuaternion getQuaternion(ofVec3f target, ofVec3f up); //get the rotation needed to orient the part correctly
    vector<std::pair<ofMesh, ofTexture>> blocks; //list of meshes and their associated textures for drawing
	std::string part; //what part of the body this segment is associated with
};

#endif /* defined(__assimpLoaderExample__Segment__) */