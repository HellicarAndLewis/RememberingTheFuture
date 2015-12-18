/**
 *  Ticker.cpp
 *
 *  Created by James Bentley on 07/05/2015.
 */

#include "Ticker.h"

Ticker::Ticker(int _fontSize, float _speed, int _width, int _height) {
	font = new ofTrueTypeFont();
	bool loaded = font->loadFont("Fonts/Lato-Black.ttf", _fontSize);
	if (loaded) cout << "Ticker(): Font loaded successfully" << endl;
	else cout << "Ticker(): Font failed to load" << endl;
	buffer.allocate(_width, _height );
	if (!buffer.isAllocated()) {
		cout << "Ticker(): Buffer not allocated" << endl;
	}
	step = 0.0f;
	speed = 0.25f;
	stepsPerSecond = 180;
}

bool Ticker::loadFromFile(string fileLoc) {
	textBuffer = ofBufferFromFile(fileLoc);
	events = ofSplitString(textBuffer.getText(), "\n");
	for (auto event : events) {
		cout << event << endl;
	}
	fullTextWidth = getFullTextWidth();
	return true;
}

void Ticker::update() {
	buffer.begin();
		ofClear(0, 0, 0, 0);
		ofBackground(0);
		ofSetColor(255);
		ofTranslate(-step, 60);
		float offset = 0;
		for (auto event : events) {
			if (event.size() != 0) {
				font->drawString(event, offset, 0);
				float width = font->getStringBoundingBox(event, 0, 0).width;
				float height = font->getStringBoundingBox(event, 0, 0).height;
				offset += width;
				offset += 40;
				ofCircle(offset, -height / 3, 20);
				offset += 40;
			}
		}
	buffer.end();
	step += stepsPerSecond * 1/ofGetFrameRate();
	if (step > fullTextWidth) step = (speed > 0) ? -ofGetWidth() : ofGetWidth();
}

void Ticker::draw(int x, int y) {
	buffer.draw(x, y);
}

int Ticker::getFullTextWidth() {
	float offset = 0;
	for (auto event : events) {
		if (event.size() != 0) {
			float width = font->getStringBoundingBox(event, 0, 0).width;
			offset += width;
			offset += 40;
			offset += 40;
		}
	}
	return offset;
}

void Ticker::setStepsPerSecond(float input) {
	stepsPerSecond = input;
}

float Ticker::getStepsPerSecond() {
	return stepsPerSecond;
}

Ticker::~Ticker() {
	delete font;
}
