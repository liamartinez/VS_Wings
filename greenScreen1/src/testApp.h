#pragma once

#include "ofMain.h"
#include "ofxGreenscreen.h"
#include "ofxTileSaver.h"

class testApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

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
	//ofVideoGrabber grabber;
	ofImage greenPic[4];

	ofImage angel;
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
	
	ofxTileSaver saver;
	bool saveHi; 
	
	ofPixels comp; 
	bool hires, oldRes; 
	string saveString; 
	
	ofDirectory hi, lo; 
	
};
