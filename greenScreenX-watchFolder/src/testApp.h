#pragma once

#include "ofMain.h"
#include "ofxGreenscreen.h"
#include "ofxSimpleGuiToo.h"

#include "ofxUI.h"

class testApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();
	void exit(); 
	void setupGUI(int x, int y); 
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
	//ofVideoGrabber grabber;
	ofImage greenPic[4];
	ofImage greenPicOrig[4];
	void keyPhoto(); 
	void updateEditAdvanced(); 
	void saveComp(); 
	
	void toggleStates(); 

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
	
	ofFloatColor bgCol; 
	bool saveImgs;
	
	bool picOn; 
	bool go; 
	
	ofFbo greenFBO; 
	string imgPath, imgPathHi; 
	
	//int curPic;
	int totalFiles; 

	bool saveHi; 
	
	ofPixels comp; 
	bool hires, oldRes; 
	string saveString; 
	
	ofDirectory dir;
	int numFiles, oldFiles; 
	string newFile; 
	bool isEmpty; 
	void checkFiles(); 
	void resetFolderAndGetState(); 

	int wingXoff, wingYoff;
	int wingScale; 
	ofImage wings, wingsHI; 
	int wingState; 
	ofImage photo, photoDupe; 
	
	bool saveNow, isSaved; 
	
	bool quickEditMode; 
	
	ofImage angel;
	
	//ofxUI
	ofxUICanvas *quickGui;   	
	void guiEvent(ofxUIEventArgs &e);
	void hideAllGui(); 
	float red, green, blue; 	
	bool editing; 
	string status;
	bool quickToggle;
	bool guiToggle; 
	void toggleGUI(); 
	
	bool quickOn; 
	
	void drawInstructionsManual(int x, int y); 
	void drawMessages(int x, int y); 
	

	string statusMsg; 
	string newFileMsg; 
	string totalFilesMsg; 
	string savedFileMsg; 
	
};
