#include "testApp.h"

#define USE_GUI

#ifdef USE_GUI
#include "ofxSimpleGuiToo.h"
ofFloatColor bgCol; 
bool saveImgs;
#endif

//--------------------------------------------------------------
void testApp::setup() {
	
	//ofdirectory
	dir.allowExt("jpg");
	dir.setShowHidden(false);	

	imgPath = "pics/hi/";
	dir.listDir(imgPath);
	resetFolderAndGetState(); 

	if (wingState != 0) {
		setupImgs(); 	
	}

	setupGUI(); 
	ofBackground(0);
	picOn = false; //original pic
	go = true; //make the greenscreening a one-burst event
}

//--------------------------------------------------------------
void testApp::setupImgs() {

	//load bg and wings
	bgImg.loadImage ("pics/stage-background.jpg"); 
	wings.loadImage ("pics/angelwings.png"); 
	wingsHI.loadImage ("pics/angelwingsHI.png"); 

	//greenscreen the first frame
	greenscreen.clear();	
	comp.allocate(greenPic[curPic].width, greenPic[curPic].height, 4);
	greenFBO.allocate(greenPic[curPic].width, greenPic[curPic].height);
	
	keyPhoto(); 
		
	//does this work?
	ofColor startColor(20, 200, 20);
	greenscreen.setBgColor(startColor);
	
	savedFileMsg = "Nothing saved yet.";
	statusMsg = "Ready!"; 
	wingState = 1; 
}

//--------------------------------------------------------------
void testApp::checkFiles() {
	dir.listDir(imgPath);
	if (dir.numFiles() > numFiles) {
		numFiles++; 
		newFile = dir.getName(numFiles-1);
		newFileMsg = "Name of current file: " + newFile; 
		numFiles = dir.numFiles(); 
		totalFilesMsg = "Total num of files in folder: " + ofToString(numFiles); 
		wingState = 2; 
		go = true; 
	} else if (dir.numFiles() < numFiles) {
		statusMsg = "Files have been deleted. Reseting totals."; 
		resetFolderAndGetState(); 
	} else if (dir.numFiles() < 1 ) {
		statusMsg = "Folder is empty."; 
	}
}

//--------------------------------------------------------------
void testApp::resetFolderAndGetState() {
	numFiles = dir.numFiles(); 
	if (dir.numFiles() > 0 ) {
		wingState = 1; 
		newFile = dir.getName(numFiles-1);
		newFileMsg = "Name of current file: " + newFile; 
		newFile = dir.getName(numFiles-1);
		for(int i = 0; i < numFiles; i++){
			cout << dir.getPath(i) << endl;
		}
	} else {
		wingState = 0; 
		newFileMsg = "No new file.";
	}
	totalFilesMsg = "Total num of files in folder: " + ofToString(numFiles); 
	
	

}

//--------------------------------------------------------------
void testApp::keyPhoto() {	
	photo.loadImage (imgPath + newFile); 
	photoDupe.loadImage (imgPath + newFile); 
	greenscreen.setPixels(greenPic[curPic].getPixelsRef());
}

//--------------------------------------------------------------
void testApp::update() {
	checkFiles(); 

	
}
//--------------------------------------------------------------

void testApp::draw() {
	ofSetColor(255);
	ofDrawBitmapString(statusMsg, 30, 30);
	ofDrawBitmapString(newFileMsg, 30, 60);
	ofDrawBitmapString(totalFilesMsg, 30, 90);
	ofDrawBitmapString(savedFileMsg, 30, 110);

};
//--------------------------------------------------------------
void testApp::exit(){}; 
void testApp::setupGUI(){}; 

void testApp::keyPressed  (int key){};
void testApp::keyReleased(int key){};
void testApp::mouseMoved(int x, int y ){};
void testApp::mouseDragged(int x, int y, int button){};
void testApp::mousePressed(int x, int y, int button){};
void testApp::mouseReleased(int x, int y, int button){};
void testApp::windowResized(int w, int h){};
void testApp::dragEvent(ofDragInfo dragInfo){};
void testApp::gotMessage(ofMessage msg){};
