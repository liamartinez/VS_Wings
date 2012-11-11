#pragma once

#include "ofMain.h"
#include "ofxGreenscreen.h"
#include "CanonCameraWrapper.h"
#include "ofxUI.h"

class testApp : public ofBaseApp {

public:
		testApp();
	void setup();
	void update();
	void draw();
	void exit(); 
	void setupGUI(); 
	void setupImgs(); 

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxGreenscreen greenscreen;
	ofxGreenscreen keyPic; 
	
	
	//ofVideoGrabber grabber;
	ofImage greenPic[4];
	ofImage greenPicOrig[4];

	ofImage bgImg;
	ofImage mask;
	ofImage baseMask;
	ofImage detailMask;
	ofImage chromaMask;

	ofImage redSub;
	ofImage greenSub;
	ofImage blueSub;

	ofPoint dragStart;
	ofPoint greenPixelPos;
	
	ofFloatColor tempC;
	
	bool picOn; 
	bool go; 
	
	ofFbo greenFBO; 
	string imgPath, imgPathHi; 
	
	int curPic;
	int totalFiles; 

	bool saveHi; 
	
	ofPixels comp; 
	bool hires, oldRes; 
	string saveString; 
	
	ofDirectory dir;
	int numFiles, oldFiles; 
	string newFile; 
	bool isEmpty; 

	int wingXoff, wingYoff;
	int wingScale; 
	ofImage wings, wingsHI; 
	int wingState; 
	
	bool saveNow, isSaved; 
	
	bool quickEditMode; 
	
	ofImage angel;
	
	//ofxUI
	ofxUICanvas *quickGui;   	
	void guiEvent(ofxUIEventArgs &e);
	float red, green, blue; 	
	bool editing; 
	string status;
	bool quickToggle;
	bool guiToggle; 
	void toggleFunc(); 
	
	void drawInstructionsManual(); 
	void drawInstructionsAuto(); 
	
	//canoncamera
	void imageDownloaded(string &path);
	CanonCameraWrapper	camera;
	
	ofTexture			tex;
	ofImage				testLoad, testLoadKey;
	bool				texIsInitialised;
	ofPixels texGreen; 
	ofPixels origTex;
	ofImage tempImage;
};
