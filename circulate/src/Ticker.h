/**
 *  Description: 
 *				 
 *  Trail.h, created by Jamers Bentley on 07/05/2014.
 */

#pragma once
#include "ofMain.h"

class Ticker {
public:
	Ticker(int _fontSize = 40, float _speed = 0.5f, int width = ofGetWidth(), int height = 80);
	~Ticker();
	void draw(int _x, int _y);
	void update();
	bool loadFromFile(string _dataLoc);
	void setStepsPerSecond(float);
	float getStepsPerSecond();

private:
	unsigned long timer;
	float stepsPerSecond;
	int getFullTextWidth();
	ofImage graphic;
	int fullTextWidth;
	float step;
	float speed;
	ofFbo buffer;
	ofBuffer textBuffer;
	vector<string> events;
	ofTrueTypeFont* font;
};
