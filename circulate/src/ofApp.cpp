#include "ofApp.h"
using namespace ofxKFW2;

const char ofApp::keys[] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm'};

void ofApp::setup() {
	bDrawGui = false;
	bClapsActive = false;
	bHighFivesActive = false;
	ticker.loadFromFile("Predictions/future.txt"); \
	ofHideCursor();
	//Set control Booleans
	loaded = false;
	nextModelChosen = false;
	string logFile = "log." + ofGetTimestampString("%H") + "." + ofGetTimestampString("%M")
		+ "." + ofGetTimestampString("%d") + "." + ofGetTimestampString("%m") + "."
		+ ofGetTimestampString("%y") + ".txt";
	ofLogToFile(logFile, false);

	//set log level, mostly for logging ofxAssimpModelLoader
	//ofSetLogLevel(OF_LOG_VERBOSE);

	//load venueIndex
	venueIndex = 0; // CHANGE THIS NUMBER TO 3!

	//setup kinect streams
	kinect.open();
	kinect.initColorSource();
	kinect.initBodySource();

	//Disable Arb Tex, necessary for ofxAssimp Texture mapping
	ofDisableArbTex();

	//enable alpha blending for alpha colors
	ofEnableAlphaBlending();

	//load Background Image
	ofDirectory Backgrounds("Backgrounds");
	Backgrounds.allowExt("jpg");
	Backgrounds.listDir();
	for (int i = 0; i < Backgrounds.numFiles(); i++) {
		string name;
		vector<string> names = ofSplitString(Backgrounds.getPath(i), "\\");
		name = names[1];
		names = ofSplitString(name, ".");
		name = names[0];
		ofImage image;
		image.loadImage(Backgrounds.getPath(i));
		backgroundImages[name] = image;
		backgroundNames.push_back(name);
	}
	loadImage.loadImage("loadingScreen.jpg");
	bLoadImageDrawn = false;
}
//--------------------------------------------------------------
void ofApp::load(){
	//this->splashScreen.init("splashScreen.jpg");
	//this->splashScreen.begin(); 

	//populate costumesLib map by going through venues directory recursively
	//only works if the names of the files are exactly the same as those inside bonePairs in Costume.cpp
	//will skip over missing files and simply not draw them
	std::string venuesPath = "venues/";
	activeVenue = "All";
	ofDirectory venues(venuesPath);
	venues.listDir();
	//open venues dir
	for(int i=0; i<venues.numFiles(); i++) {
		ofDirectory venue(venues.getPath(i));
		if(venue.isDirectory()) {
			venue.listDir();
			 //open each venue dir
			for(int j=0;j< venue.numFiles();j++) {
				if (activeVenue != "All") {
					vector<std::string> venueString = ofSplitString(venue.getPath(j), "\\");
					if (venueString[1] != activeVenue) break;
				}
				ofDirectory costume(venue.getPath(j));
				if(costume.isDirectory()) {
					//for each costume:
					//costume.allowExt("dae"); //only look at .dae files (ie ignore texture files)
					costume.listDir();
					std::string cos; //initialize string to name the costume (will get the name late from the absolute path of each file so we have fewer calls to ofSplitString
					std::vector<Segment> segs; //create a vector of segments for each costume that we will populate
					 //open each costume inside each venue
					for(int k=0; k<costume.numFiles();k++) {
						ofDirectory part(costume.getPath(k));
						//initialize model loader for each model
						ofxAssimpModelLoader model; 
						part.allowExt("dae");
						part.listDir();
						//sort name of each costume out
						vector<std::string> splitString = ofSplitString(costume.getPath(k), "\\"); //split path string on "\" to get venue and costume names
						cos = splitString[2];
						std::vector<std::string> splitPart = ofSplitString(splitString[3], "."); //get the nameof the body part by splitting the string on "." to remove .dae extension
						std::string partName = splitPart[0];
						std::pair<std::string, std::string> pair = std::make_pair(cos, partName);
						//load each .dae file
						model.loadModel(part.getPath(0), true);
						//save the meshes and textures for each file into blocks
						vector<std::pair<ofMesh, ofTexture>> blocks;
						for(int l=0;l<model.getMeshCount();l++) {
							ofMesh mesh = model.getMesh(l); //get each mesh
							ofTexture tex = model.getTextureForMesh(l); //get each texture
							std::pair<ofMesh, ofTexture> block = make_pair(mesh, tex); //associate the two values
							blocks.push_back(block); //add it to blocks vector
						}
						//initialize segment with meshes textures and location
						Segment seg;
						seg.init(blocks, partName);
						segs.push_back(seg); //populate segs vector for this costume
						segmentsLib[pair] = seg; // populate segments map for random costumes
					}
					//for each costume create a costume, initialize it with the loaded segments and save it to the costumesLib
					Costume newCostume;
					newCostume.init(cos);
					costumesLib[cos] = newCostume;
					preloadedCostumes.push_back(cos);
				}
			}
		}
	}
	cout<<"List of all "<<costumesLib.size()<<" loaded costumes:"<<endl;
	int i=1;
	map<std::string, Costume>::iterator cos;
	for(cos = costumesLib.begin(); cos != costumesLib.end(); cos++) {
		cout<<i<<": "<<cos->first<<endl;
		i++;
	}
	this->splashScreen.end(); 
	loaded = true;
}

//--------------------------------------------------------------
void ofApp::update(){
	if (bLoadImageDrawn && !loaded){
		this->load();
	} else if(loaded) {
		this->kinect.update(); //update all the kinect streams
		ticker.update();

		//get all the bodies and find which ones are tracked
		auto trackables = this->kinect.getBodySource()->getBodies(); //get bodies
		std::vector<int> trackedIds; //list of tracked Ids
		std::vector<Data::Body>::iterator trackable; //iterator for iterating over bodies
		for(trackable = trackables.begin(); trackable != trackables.end(); trackable++) {
			if(trackable->tracked){
				trackedIds.push_back(trackable->trackingId); //populate tracked Ids vector with each tracked body
			}
		}

		//look through the tracked Ids and check if any are not represented in the activeCostumes, if they aren't add them.
		std::vector<int>::iterator trackedId; //iterator for iterating over trackedIds
		for(trackedId = trackedIds.begin(); trackedId != trackedIds.end(); trackedId++) {
			bool exists = false;
			std::map<int, std::string>::iterator activeCostume; //iterator for itertating over activeCostumes
			for(activeCostume = activeCostumes.begin(); activeCostume != activeCostumes.end(); activeCostume++) {
				if(activeCostume->first == *trackedId) exists = true; //if the tracked id already has an costume do not add it
			}
			if(!exists) {
				if(!nextModelChosen) {
					auto it = costumesLib.begin();
					std::advance(it, rand() % costumesLib.size());
					nextCostume = it->first;
				}
				activeCostumes[*trackedId] = nextCostume; //add an active costume if the id is not found in the list of already active costumes
				JointSet newJointSet = JointSet(this->kinect.getBodySource()->getPositions3D(*trackedId)); //add an active jointset if the id is not found in the list of already active jointSets
				activeJointSets[*trackedId] = newJointSet;
				ofLog() << "[" << ofGetTimestampString() << "]" << " " << *trackedId << " entered the frame" << endl;
			}
		}

		//look through the activeCostumes and remove any that are connected to bodies that are no longer tracked
		std::vector<int> costumesToErase; //list of costumes that need to be erased
		std::map<int, std::string>::iterator activeCostume; //iterator for iterating over activeCostumes (again for simplicity)
		for(activeCostume = activeCostumes.begin(); activeCostume != activeCostumes.end(); activeCostume++) {
			bool stillTracked = false;
			std::vector<int>::iterator trackedId; //iteratir for iterating over trackedIds (again for simplicity)
			for(trackedId = trackedIds.begin(); trackedId != trackedIds.end(); trackedId++) {
				if(*trackedId == activeCostume->first) {
					stillTracked = true; //if the body is still tracked then do nothing
				}
			}
			if(!stillTracked) {
				costumesToErase.push_back(activeCostume->first); //if the body is no longer tracked then add it to the list of costumes to be removed
				ofLog() << "[" << ofGetTimestampString() << "]" << " " << activeCostume->first << " left the frame" << endl;
			}
		}

		//erase costumes with bodies that are no longer being tracked
		std::vector<int>::iterator costumeToErase; //iterator for iterating over costumesToErase
		for(costumeToErase = costumesToErase.begin(); costumeToErase != costumesToErase.end(); costumeToErase++) {
			activeCostumes.erase(*costumeToErase); //erase each missing costume
			activeJointSets.erase(*costumeToErase); //erase each missing set of joints
		}

		//look through the costumesLib to find costumes that we can delete.
		vector<string> costumesToDelete;
		vector<string>::iterator preloadedCostume;
		map<std::string, Costume>::iterator costume;
		for(costume = costumesLib.begin(); costume != costumesLib.end(); costume++) {
			bool found = false;
			for(trackedId = trackedIds.begin(); trackedId != trackedIds.end(); trackedId++) {
				if(ofToString(*trackedId) == costume->first) {
					found = true;
					break;
				}
			}
			if(!found) {
				for(preloadedCostume = preloadedCostumes.begin(); preloadedCostume != preloadedCostumes.end(); preloadedCostume++) {
					if(*preloadedCostume == costume->first) {
						found = true;
						break;
					}
				}
			}
			if(!found) costumesToDelete.push_back(costume->first);
		}

		//erase all costumes not found
		vector<string>::iterator costumeToDelete;
		for(costumeToDelete = costumesToDelete.begin(); costumeToDelete != costumesToDelete.end(); costumeToDelete++) {
			costumesLib.erase(*costumeToDelete);
		}

		// update the activeJonts array, look for claps and high-fives
		for(trackedId = trackedIds.begin(); trackedId != trackedIds.end(); trackedId++) {
			std::map<JointType, ofVec3f> body = this->kinect.getBodySource()->getPositions3D(activeJointSets.find(*trackedId)->first);
			activeJointSets.find(*trackedId)->second.update(body);
			bool existsClapping = false;
			std::pair<bool, ofVec3f> clapped = activeJointSets.find(*trackedId)->second.detectClap(50.0f);
			if(clapped.first) {
				std::map<int, ofVec3f>::iterator clappingBody;
				for(clappingBody = clappingBodies.begin(); clappingBody != clappingBodies.end(); clappingBody++) {
					if(clappingBody->first == *trackedId) {
						existsClapping = true;
						break;
					}
				}
				if(!existsClapping) {
					clappingBodies[*trackedId] = clapped.second;
					// INSERT ON CLAP CODE HERE!
					if(bClapsActive) onClap(*trackedId);
				}
			} else {
				std::map<int, ofVec3f>::iterator clappingBody;
				for(clappingBody = clappingBodies.begin(); clappingBody != clappingBodies.end(); clappingBody++) {
					if(clappingBody->first == *trackedId) {
						clappingBodies.erase(clappingBody);
						break;
					}
				}
			}
			std::vector<int>::iterator remainingId;
			for(remainingId = trackedId + 1; remainingId != trackedIds.end(); remainingId++) {
				bool exists = false;
				std::pair<bool, ofVec3f> highFived = activeJointSets.find(*trackedId)->second.detectHighFives(activeJointSets.find(*remainingId)->second, 50.0f);
				if(highFived.first) {
					std::map<std::pair<int, int>, ofVec3f>::iterator highFivingPair;
					for(highFivingPair = highFivingPairs.begin();highFivingPair != highFivingPairs.end(); highFivingPair++) {
						if(highFivingPair->first == make_pair(*trackedId, *remainingId)) {
							exists = true;
							break;
						}
					}
					if(!exists) {
						highFivingPairs[std::make_pair(*trackedId, *remainingId)] = highFived.second;
						//INSERT ON HIGH FIVE CODE HERE! (some has already been inserted)
						if(bHighFivesActive) onHighFive(*trackedId, *remainingId);
					}
				} else {
					std::map<std::pair<int, int>, ofVec3f>::iterator highFivingPair;
					for(highFivingPair = highFivingPairs.begin(); highFivingPair != highFivingPairs.end(); highFivingPair++) {
						if(highFivingPair->first == make_pair(*trackedId, *remainingId)) {
							highFivingPairs.erase(highFivingPair);
							break;
						}
					}
				}
			}
		}

		//delete untracked high-five locations
		std::vector<std::pair<int, int>> pairsToDelete;
		std::map<std::pair<int, int>, ofVec3f>::iterator highFivingPair;
		for(highFivingPair = highFivingPairs.begin(); highFivingPair != highFivingPairs.end(); highFivingPair++) {
			bool tracked1 = false;
			bool tracked2 = false;
			for(trackedId = trackedIds.begin(); trackedId != trackedIds.end(); trackedId++) {
				if(highFivingPair->first.first == *trackedId) tracked1 = true;
				if(highFivingPair->first.second == *trackedId) tracked2 = true;
			}
			if(!tracked1 || !tracked2) {
				pairsToDelete.push_back(highFivingPair->first);
			}
		}
		std::vector<std::pair<int, int>>::iterator pairToDelete;
		for(pairToDelete = pairsToDelete.begin(); pairToDelete != pairsToDelete.end(); pairToDelete++) {
			highFivingPairs.erase(*pairToDelete);
		}

		//delete untracked clap locations
		std::vector<int> clappersToDelete;
		std::map<int, ofVec3f>::iterator clappingBody;
		for(clappingBody = clappingBodies.begin(); clappingBody != clappingBodies.end(); clappingBody++) {
			bool tracked = false;
			for(trackedId = trackedIds.begin(); trackedId != trackedIds.end(); trackedId++) {
				if(clappingBody->first == *trackedId) tracked = true;
			}
			if(!tracked) {
				clappersToDelete.push_back(clappingBody->first);
			}
		}
		std::vector<int>::iterator clapperToDelete;
		for(clapperToDelete = clappersToDelete.begin(); clapperToDelete != clappersToDelete.end(); clapperToDelete++) {
			clappingBodies.erase(*clapperToDelete);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (!loaded) {
		loadImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		bLoadImageDrawn = true;
	} else {
		ofSetColor(255);
		glDisable(GL_DEPTH_TEST);
		backgroundImages[backgroundNames[venueIndex]].draw(0, 0, ofGetWidth(), ofGetHeight());
		ofPushMatrix();
		ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			glEnable(GL_DEPTH_TEST);
			glShadeModel(GL_SMOOTH);
			ofSetColor(255);
			int spacing = 0;
			std::map<int, std::string>::iterator costume;
			for(costume = activeCostumes.begin(); costume != activeCostumes.end(); costume++) {
				std::map<JointType, ofVec3f> body = this->kinect.getBodySource()->getPositions3D(costume->first);
				costumesLib[costume->second].update(activeJointSets.find(costume->first)->second.getVals());
				costumesLib[costume->second].draw(&segmentsLib);
				spacing += 10;
			}
			ofDisableSeparateSpecularLight();
		ofPopStyle();
   		ofPopMatrix();

		/*if(highFivingPairs.size() > 0) {
			ofPushStyle();
			std::map<std::pair<int, int>, ofVec3f>::iterator highFivingPair;
			for(highFivingPair = highFivingPairs.begin(); highFivingPair != highFivingPairs.end(); highFivingPair++) {
				ofEnableAlphaBlending();
				ofSetColor(0, 255, 0, 127);
				ofPushMatrix();
				ofCircle(highFivingPair->second, 100.0f);
				ofPopMatrix();
				ofDisableAlphaBlending();
			}
			ofPopStyle();
		}

		if(clappingBodies.size() > 0) {
			ofPushStyle();
			std::map<int, ofVec3f>::iterator clappingBody;
			for(clappingBody = clappingBodies.begin(); clappingBody != clappingBodies.end(); clappingBody++) {
				ofEnableAlphaBlending();
				ofSetColor(255, 0, 0, 127);
				ofPushMatrix();
				ofCircle(clappingBody->second, 100.0f);
				ofPopMatrix();
				ofDisableAlphaBlending();
			}
			ofPopStyle();
		}*/
		ofPushStyle();
			ticker.draw(0, ofGetHeight() - 120);
		ofPopStyle();
		if (bDrawGui) {
			ofPushStyle();
			ofSetColor(255, 255, 255, 127);
			ofRect(0, 0, ofGetWidth(), ofGetHeight());
			ofSetColor(0);
			//int max = preloadedCostumes.size() > 9 ? 9 : preloadedCostumes.size();
			ofDrawBitmapString("venue loaded: " + activeVenue, 20, 20);
			for (int i = 0; i < preloadedCostumes.size(); i++) {
				ofDrawBitmapString(ofToString(keys[i]) + ": " + preloadedCostumes[i], 20, 12 * i + 40);
				if (i == preloadedCostumes.size() - 1) {
					string val = (nextModelChosen == false) ? "random" : nextCostume;
					ofDrawBitmapString("next costume: " + val, 20, 12 * (i + 1) + 40);
				}
			}
			ofDrawBitmapString("Claps Active: " + ofToString(bClapsActive) + " press 'c' to toggle", 10, ofGetHeight() - 50 - 120);
			ofDrawBitmapString("Highfives Active: " + ofToString(bHighFivesActive) + " press 'h' to toggle", 10, ofGetHeight() - 70 - 120);
			ofDrawBitmapString("Ticker Scroll Speed: " + ofToString(ticker.getStepsPerSecond()) + " control with up and down arrows", 10, ofGetHeight() - 90 - 120);
			ofDrawBitmapString("Background Venue is currently: " + ofToString(backgroundNames[venueIndex]) + ", press 'v' to cycle through venues", 10, ofGetHeight() - 110 - 120);
			ofDrawBitmapString("Set the next Costume to 'Random' by pressing '0'", 10, ofGetHeight() - 130 - 120);

			ofPopStyle();
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == ' ') {
		if (bDrawGui) ofHideCursor();
		else ofShowCursor();
		bDrawGui = !bDrawGui;
	}
	if (key == 'c') {
		bClapsActive = !bClapsActive;
	}
	if (key == 'h') {
		bHighFivesActive = !bHighFivesActive;
	}
	if (key == 'v') {
		venueIndex++;
		venueIndex %= backgroundImages.size();
	}
	for (int i = 0; i < preloadedCostumes.size(); i++) {
		if (key == keys[i]) {
			nextCostume = preloadedCostumes[i];
			nextModelChosen = true;
			break;
		}
	}
	if (key == '0') {
		nextModelChosen = false;
	}
	if (key == OF_KEY_UP) {
		ticker.setStepsPerSecond(ticker.getStepsPerSecond() + 1);
	}
	if (key == OF_KEY_DOWN) {
		ticker.setStepsPerSecond(ticker.getStepsPerSecond() - 1);
	}
}

ofMesh ofApp::generateWallMesh(float depth) {

	ofMesh walls;
	// left wall
	walls.addVertex(ofVec3f(0, 0, 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, ofGetHeight(), 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, ofGetHeight(), -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, 0, -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, 0, 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, ofGetHeight(), -depth));
	walls.addColor(ofColor(255));

	// back wall
	walls.addVertex(ofVec3f(0, 0, -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, ofGetHeight(), -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), ofGetHeight(), -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, 0, -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), 0, -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), ofGetHeight(), -depth));
	walls.addColor(ofColor(255));

	// top wall (otherwise known as a ceiling)
	walls.addVertex(ofVec3f(0, 0, 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, 0, -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), 0, 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), 0, -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, 0, -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), 0, 0));
	walls.addColor(ofColor(255));

	// right wall
	walls.addVertex(ofVec3f(ofGetWidth(), ofGetHeight(), 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), 0, 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), 0, -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), ofGetHeight(), -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), 0, -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), ofGetHeight(), 0));
	walls.addColor(ofColor(255));

	// bottom wall (otherwise known as a floor)
	walls.addVertex(ofVec3f(0, ofGetHeight(), 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), ofGetHeight(), 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), ofGetHeight(), -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, ofGetHeight(), 0));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(0, ofGetHeight(), -depth));
	walls.addColor(ofColor(255));
	walls.addVertex(ofVec3f(ofGetWidth(), ofGetHeight(), -depth));
	walls.addColor(ofColor(255));

	walls.setMode(OF_PRIMITIVE_TRIANGLES);

	return walls;
}

void ofApp::onHighFive(int trackingId1, int trackingId2) {
	std::string cosName1 = activeCostumes.find(trackingId1)->second;
	std::string cosName2 = activeCostumes.find(trackingId2)->second;
	activeCostumes.find(trackingId1)->second = cosName2;
	activeCostumes.find(trackingId2)->second = cosName1;
}

void ofApp::onClap(int trackingId) {
	Costume newCos;
	newCos.initRandSensible(&segmentsLib);
	std::string cosName = ofToString(trackingId);
	newCos.setPositions(activeJointSets[trackingId].getVals());
	costumesLib[cosName] = newCos;
	activeCostumes.find(trackingId)->second = cosName;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
