#pragma once

#include "ofMain.h"
#include "ofxKinectforWindows2.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSplashScreen.h"
#include "ofxTween.h"
#include "ofxXmlSettings.h"
#include "JointSet.h"
#include "Segment.h"
#include "Costume.h"
#include "Ticker.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void load();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void detectContact(void);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		ofMesh generateWallMesh(float depth);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void onHighFive(int trackingId1, int trackingId2);
		void onClap(int trackingId);
		
		map<string, ofImage> backgroundImages;
		vector<string> backgroundNames;
		map<char, int> CostumeButtons;
		const static char keys[26];
		ofImage loadImage;
		int venueIndex;
		ofxKFW2::Device kinect;
		vector<std::string> preloadedCostumes;
		vector<std::string> generatedCostumes;
		map<std::string, std::string> venueCostumeLib;
		map<std::string, Costume> costumesLib;
		map<int, JointSet> activeJointSets;
		map<int, std::string> activeCostumes;
		std::map<std::pair<int, int>, ofVec3f> highFivingPairs;
		std::map<int, ofVec3f> clappingBodies;
		std::map<std::pair<std::string, std::string>, Segment> segmentsLib;
		std::string nextCostume;
		ofxSplashScreen splashScreen;
		bool loaded, nextModelChosen, bClapsActive, bHighFivesActive, bDrawGui, bLoadImageDrawn;
		Ticker ticker;
		std::string activeVenue;
};
