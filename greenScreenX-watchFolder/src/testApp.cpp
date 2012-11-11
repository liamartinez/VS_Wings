#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	
	//ofdirectory
	dir.allowExt("jpg");
	dir.setShowHidden(false);	

	imgPath = "pics/hi/";
	dir.listDir(imgPath);
	resetFolderAndGetState(); 

	setupImgs(); 	
	setupGUI(30, ofGetHeight() - 500); 
	
	ofBackground(0);
	picOn = false; //original pic
	saveNow = false; 
	go = true; //make the greenscreening a one-burst event
}

//--------------------------------------------------------------
void testApp::setupImgs() {

	//load bg and wings
	bgImg.loadImage ("pics/stage-background.jpg"); 
	wings.loadImage ("pics/angelwings.png"); 
	wingsHI.loadImage ("pics/angelwingsHI.png"); 

	//clear the first frame and key
	greenscreen.clear();	
	if (wingState !=0) keyPhoto(); 
		
	//does this work?
	ofColor startColor(20, 200, 20);
	greenscreen.setBgColor(startColor);
	
	savedFileMsg = "Nothing saved yet.";
	statusMsg = "Ready!"; 
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
		statusMsg = "K. Updated."; 
		wingState = 1; 
		go = true; 
	} else if (dir.numFiles() < numFiles) {
		statusMsg = "Files have been deleted. Reseting totals."; 
		resetFolderAndGetState(); 
	} else if (dir.numFiles() < 1 ) {
		totalFilesMsg = "Folder is empty. Please Load photo into: " + imgPath; 
	}
}

//--------------------------------------------------------------
void testApp::resetFolderAndGetState() {
	numFiles = dir.numFiles(); 
	if (dir.numFiles() > 0 ) {
		newFile = dir.getName(numFiles-1);
		newFileMsg = "Name of current file: " + newFile; 
		newFile = dir.getName(numFiles-1);
		for(int i = 0; i < numFiles; i++){
			cout << dir.getPath(i) << endl;
		}
		wingState = 1; 
		go = true; 
	} else {
		wingState = 0; 
		newFile = " "; 
		newFileMsg = "No new file.";
	}
	totalFilesMsg = "Total num of files in folder: " + ofToString(numFiles); 

}

//--------------------------------------------------------------
void testApp::keyPhoto() {	
	cout << "KEYING" << endl; 
	photo.loadImage (imgPath + newFile); 
	photoDupe.loadImage (imgPath + newFile); 
	
	comp.allocate(photo.width, photo.height, 4);
	greenFBO.allocate(photo.width, photo.height);
	
	greenscreen.setPixels(photo.getPixelsRef());
}

//--------------------------------------------------------------
void testApp::update() {
	
	checkFiles(); 

	switch (wingState) {
		case 1:
			if (go || editing) {
				keyPhoto(); 
				go = false; 
			}
			
			if(gui.isOn()) {
				updateEditAdvanced();
			}
			
			quickOn = true; 
			toggleGUI();
			isSaved = false; 
			break;
			
		case 2:
			hideAllGui();
			isSaved = false; 
			break;
			
		case 3:
			if (!isSaved) saveNow = true; 
	}

	
	if (saveNow) {
		saveComp();
		saveNow = false; 
	}
	
}
//--------------------------------------------------------------

void testApp::draw() {
	
	cout << "case " << wingState << endl; 
	wingScale = 1.0;
	ofPoint wingPos; 
	
	//set the FBO
	greenFBO.begin(); 
	ofEnableAlphaBlending();
	ofSetColor (255); 
	bgImg.draw(0,0, photo.width, photo.height); 
	
	switch (wingState) {
			
		case 0:
			//empty first page.
			break;
		case 1:
			//cout << "case 1: this is where editing is possible" << endl; 
			//this is where you can choose the background color. see mousePressed()
			break;
			
		case 2: 
			//cout << "case 2: position wings" << endl; 

			//ratio of highres to drawing
			wingXoff = (ofGetMouseX()*(3.8667)) - wingsHI.width/2; 
			wingYoff = (ofGetMouseY()*(3.8667)) - (wingsHI.height - wingsHI.height/3); 
			wingsHI.draw(wingXoff, wingYoff); 			
			break; 
			
		case 3:
			//cout << "case 3: save wing pos" << endl; 
			wingPos.x = wingXoff; 
			wingPos.y = wingYoff; 
			wingPos.z = 1.0; 

			wingsHI.draw(wingPos);	
			break; 
	}
	
	greenscreen.draw(0, 0, photo.width, photo.height);
	ofDisableAlphaBlending(); 
	greenFBO.end();
	
	//really draw
	ofSetColor (255); 
	if (wingState !=0) {
		ofPushMatrix();	
		ofTranslate(ofGetWidth() - 480, 0 );
		greenFBO.draw(0, 0 , 480, 720);
		greenscreen.drawBgColor();
		ofPopMatrix();
	}
	
	drawMessages(30, 30); 
	if (wingState == 1) {
		drawInstructionsManual(30, 200);
		gui.draw();
	}
	
}

//--------------------------------------------------------------
void testApp::saveComp() {
	greenFBO.readToPixels(comp); 
	
	string newName = newFile; 
	ofStringReplace(newName, ".JPG", "_");
	saveString = "saved_" + newName + ".tif";
	ofSaveImage(comp, saveString, OF_IMAGE_QUALITY_BEST);  
	savedFileMsg = "Last photo saved to data folder: " + saveString;
	isSaved = true;
}

//--------------------------------------------------------------

void testApp::keyPressed(int key) {
	/*
	 //use these for scale and rotate
	if(key == OF_KEY_DOWN)

	if(key == OF_KEY_UP)

	if(key == OF_KEY_LEFT)

	if(key == OF_KEY_RIGHT)
*/	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {

	if(key == 'q') {
		toggleGUI();
		wingState = 1; 
	}

	//if (key == 'p') picOn = !picOn; 
	if(key=='s') saveComp();

	if (key == ' ') {
		toggleStates(); 
	}

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	
	if(!gui.isOn()) {
		if (wingState == 1) {
			greenscreen.setBgColor(photoDupe.getPixelsRef().getColor(x, y));
		} else if (wingState == 2) {
			wingState = 3; 
		}
	}
	else {
	
	ofColor pc; 
	ofColor c = greenscreen.getBgColor();
	bgCol.r = c.r/255.;
	bgCol.g = c.g/255.;
	bgCol.b = c.b/255.;
	}
	
	go = true; 
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
	editing = false; 
}
//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	if (wingState == 0) editing = true; 
}
//--------------------------------------------------------------
void testApp::toggleGUI() {
	
	if (quickOn) {
		quickGui->enable();
		gui.hide();
	} else {
		quickGui->disable(); 
		gui.show();
	}
}
//--------------------------------------------------------------
void testApp::toggleStates() {
	if (wingState == 1) {
		wingState = 2; 
	} else if (wingState == 2) {
		wingState = 1; 
	} else if (wingState == 3) { // if you want to edit again.
		wingState = 2;
	}
}

//--------------------------------------------------------------
void testApp::hideAllGui() {
	quickGui->disable();
	gui.hide();
}

//--------------------------------------------------------------


	void testApp::mouseMoved(int x, int y ){};
	void testApp::windowResized(int w, int h){};
	void testApp::dragEvent(ofDragInfo dragInfo){};
	void testApp::gotMessage(ofMessage msg){};


//--------------------------------------------------------------
void testApp::updateEditAdvanced(){
	greenscreen.setBgColor(ofColor(bgCol.r*255, bgCol.g*255, bgCol.b*255));
	mask.setFromPixels(greenscreen.getMask());
	detailMask.setFromPixels(greenscreen.getDetailMask());
	baseMask.setFromPixels(greenscreen.getBaseMask());
	chromaMask.setFromPixels(greenscreen.getChromaMask());
	
	redSub.setFromPixels(greenscreen.getRedSub());
	greenSub.setFromPixels(greenscreen.getGreenSub());
	blueSub.setFromPixels(greenscreen.getBlueSub());
}

//--------------------------------------------------------------
void testApp::drawMessages(int x, int y){
	int inc = 15; 
	ofSetColor(255,0,0);
	ofDrawBitmapString("FPS "+ofToString(ofGetFrameRate()), x, y);	
	ofDrawBitmapString(statusMsg, x, y + (inc*2));
	ofDrawBitmapString(newFileMsg, x, y+(inc*3));
	ofDrawBitmapString(totalFilesMsg, x, y+(inc*4));
	ofDrawBitmapString(savedFileMsg, x, y+(inc*5));
}
//--------------------------------------------------------------
void testApp::drawInstructionsManual(int x, int y) {
	//interaface 
	
	int inc = 15; 
	ofSetColor(255);
	ofDrawBitmapString("CLICK AN AREA TO KEY", x, y);
	ofDrawBitmapString("Q TO TOGGLE DETAIL EDIT/ QUICK EDIT", x, y + (inc*2));
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	
	
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
	
	if(name == "CLIPBLACK")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		greenscreen.clipBlackEndMask = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}
	
	if(name == "CLIPWHITE")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		greenscreen.clipWhiteEndMask = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}
	
	if(name == "DETAILBLACK")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		greenscreen.clipBlackDetailMask = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}
	
	if(name == "DETAILWHITE")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		greenscreen.clipWhiteDetailMask = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}
	
	if(name == "DETAILWHITE")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		greenscreen.clipWhiteDetailMask = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}
	
	
}
//--------------------------------------------------------------
void testApp::exit()
{
    delete quickGui; 
}
//--------------------------------------------------------------
void testApp::setupGUI(int x, int y) {
	gui.addTitle("SETTINGS");
	gui.addToggle("detail mask", greenscreen.doDetailMask);
	gui.addToggle("base mask", greenscreen.doBaseMask);
	gui.addToggle("chroma mask", greenscreen.doChromaMask);
	gui.addToggle("greenspill", greenscreen.doGreenSpill);

	gui.addColorPicker("key color", bgCol);
	gui.addSlider("base mask strength", greenscreen.strengthBaseMask, 0.0, 1.f);
	gui.addSlider("chroma mask strength", greenscreen.strengthChromaMask, 0.0, 1.f);
	gui.addSlider("green spill strength", greenscreen.strengthGreenSpill, 0.0, 1.f);

	gui.addSlider("base mask black", greenscreen.clipBlackBaseMask, 0.0, 1.f);
	gui.addSlider("base mask white", greenscreen.clipWhiteBaseMask, 0.0, 1.f);
	gui.addSlider("detail mask black", greenscreen.clipBlackDetailMask, 0.0, 1.f);
	gui.addSlider("detail mask white", greenscreen.clipWhiteDetailMask, 0.0, 1.f);
	gui.addSlider("end mask black", greenscreen.clipBlackEndMask, 0.0, 1.f);
	gui.addSlider("end mask white", greenscreen.clipWhiteEndMask, 0.0, 1.f);
	
	gui.addTitle("OUTPUT");
	gui.addFPSCounter();

	gui.addContent("base mask", baseMask);
	gui.addContent("detail mask", detailMask);
	gui.addContent("chroma mask", chromaMask);
	gui.addContent("mask", mask);
	
	gui.addContent("red sub", redSub);
	gui.addContent("green sub", greenSub);
	gui.addContent("blue sub", blueSub);
	gui.addContent("keyed", greenscreen);
	

	//ofxGUI
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 320-xInit; 
	float dim = 24; 
	
	quickGui = new ofxUICanvas(x, y, length+xInit, ofGetHeight());
	
	quickGui->addLabelToggle("QUICK EDIT", false, length-xInit);	
	
	quickGui->addSpacer(length-xInit, 1); 	
	quickGui->addWidgetDown(new ofxUILabel("DETAIL MASK BLACK", OFX_UI_FONT_SMALL)); 	
	quickGui->addSlider("DETAILBLACK", 0.0, 1.f, greenscreen.clipBlackDetailMask, length-xInit,dim);
	
	quickGui->addSpacer(length-xInit, 1); 
	quickGui->addWidgetDown(new ofxUILabel("DETAIL MASK WHITE", OFX_UI_FONT_SMALL)); 	
	quickGui->addSlider("DETAILWHITE", 0.0, 1.f, greenscreen.clipWhiteDetailMask, length-xInit,dim);
	
	quickGui->addSpacer(length-xInit, 1); 
	quickGui->addWidgetDown(new ofxUILabel("END MASK BLACK", OFX_UI_FONT_SMALL)); 	
	quickGui->addSlider("CLIPBLACK", 0.0, 1.f, greenscreen.clipBlackEndMask, length-xInit,dim);
	
	quickGui->addSpacer(length-xInit, 1); 
	quickGui->addWidgetDown(new ofxUILabel("END MASK WHITE", OFX_UI_FONT_SMALL)); 	
	quickGui->addSlider("CLIPWHITE", 0.0, 1.f, greenscreen.clipWhiteEndMask, length-xInit,dim);
	ofAddListener(quickGui->newGUIEvent,this,&testApp::guiEvent);	
	
	quickToggle = true; 
	gui.loadFromXML();
}
