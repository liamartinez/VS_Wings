#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	
	ofSetWindowShape(1200, 800);
	msg.loadFont("GUI/NewMedia Fett.ttf", 10);
	msgBig.loadFont("GUI/NewMedia Fett.ttf", 12);
	msg.setLetterSpacing(1.2);
	
	//ofdirectory
	dir.allowExt("jpg");
	dir.setShowHidden(false);	
	
	imgPath = "pics/hi/";
	savePath = "pics/saved/";
	dir.listDir(imgPath);
	resetFolderAndGetState(); 
	
	setupImgs(); 	
	setupGUI(800, ofGetHeight() - 400); 
	setupCamera(); 
	
	ofBackground(0);
	picOn = false; //original pic
	saveNow = false; 
	go = false; //make the greenscreening a one-burst event	
	
	dragOut = false; 
	dragIn = false; 
}
//--------------------------------------------------------------
void testApp::update() {
	
	//checkFiles(); 
	
	switch (wingState) {
		case 0:
			updateCamera(); 
			break; 
			
		case 1:
			stateMsg = "You are in EDIT Mode. Spacebar for WINGS"; 
			if (go || editing) {
				keyPhoto(); 
				go = false; 
			}
			
			if(gui.isOn()) {
				updateEditAdvanced();
			}
			
			isSaved = false; 
			break;
			
		case 2:
			stateMsg = "You are in WINGS mode. Spacebar for EDIT"; 
			hideAllGui();
			isSaved = false; 
			break;
			
		case 3:
			if (!isSaved) saveNow = true; 
	}
	
	
	if (saveNow) {
		saveComp();
		checkSavedFiles();
		saveNow = false; 
	}	
}
//--------------------------------------------------------------

void testApp::draw() {
	
	wingScale = 1.0;
	ofPoint wingPos; 
	
	//set the FBO
	greenFBO.begin(); 
	ofEnableAlphaBlending();
	ofSetColor (255); 
	bgImg.draw(0,0, photo.width, photo.height); 
	
	switch (wingState) {
			
		case 1:
			
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
		greenFBO.draw(0, 0 , 720, 480);
		greenscreen.drawBgColor();
	}
	
	drawMessages(800, 30); 
	drawFileMsgs (800, 650); 
	
	if (wingState == 0) {
		tex.draw(0,0,720,480);
		ofDrawBitmapString(ofToString(camera.getLiveViewPixelWidth()), 0, 10);
		ofDrawBitmapString(ofToString(camera.getLiveViewPixelHeight()), 0, 20);
	}
	else if (wingState == 1) {
		drawInstructionsManual(800, 300);
		if (photo.isAllocated())
		{
			//photo.draw(0,480 , 512,360);
			ofDrawBitmapString(ofToString(testLoad.getWidth()), 512, 10);
			ofDrawBitmapString(ofToString(testLoad.getHeight()), 512, 20);
		}
		gui.draw();
	} else if (wingState == 2 || wingState == 3) {
		drawInstructinsAuto(800, 300);
	}
	
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
void testApp::setupCamera() {
	if (!camera.setup(0))
	{
		ofLogError() << "no camera detected. type 'damn it' and then enter to quit.";
		string dummy;
		cin >> dummy;
		std::exit(1);
	} else {
		camera.openSession();
		camera.beginLiveView();
		camera.setDeleteFromCameraAfterDownload(true);
		camera.enableDownloadOnTrigger();
		camera.setDownloadPath(imgPath);
		ofAddListener(camera.evtImageDownloaded, this, &testApp::imageDownloaded);
	}
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
bool testApp::checkSavedFiles() {
	dir.listDir(savePath); 
	if (dir.numFiles() > numSavedFiles) {
		numSavedFiles = dir.numFiles(); 
		statusMsg = "File saved!";
		return true; 
	} else if (dir.numFiles() < numSavedFiles) {
		statusMsg = "Files have been deleted from save folder. Resetting totals.";
		numSavedFiles = dir.numFiles(); 
		return false; 
	} else {
		return false; 
	}
	
}

//--------------------------------------------------------------
void testApp::resetFolderAndGetState() {
	numFiles = dir.numFiles(); 
	if (dir.numFiles() > 0 ) {
		statusMsg = "New File! Loading ..."; 
		newFile = dir.getName(numFiles-1);
		newFileMsg = "Name of current file: " + newFile; 
		newFile = imgPath + dir.getName(numFiles-1);
		for(int i = 0; i < numFiles; i++){
			cout << dir.getPath(i) << endl;
		}
		go = true; 
	} else {
		newFile = " "; 
		newFileMsg = "No new file.";
	}
	totalFilesMsg = "Total num of files in folder: " + ofToString(numFiles); 
	statusMsg = "K. Updated."; 
	wingState = 0; 
}

//--------------------------------------------------------------
void testApp::keyPhoto() {	
	photo.loadImage (newFile); 
	photoDupe.loadImage (newFile); 
	photoDupe.resize(720, 480);
	
	comp.allocate(photo.width, photo.height, 4);
	greenFBO.allocate(photo.width, photo.height);
	
	greenscreen.setPixels(photo.getPixelsRef());
}

//--------------------------------------------------------------
void testApp::updateCamera(){
	if (camera.grabPixelsFromLiveView())
	{
		if (!texIsInitialised && camera.getLiveViewPixelWidth() > 0)
		{
			tex.allocate(camera.getLiveViewPixelWidth(), camera.getLiveViewPixelHeight(), GL_RGB);
			texIsInitialised = true;
		}
		if (texIsInitialised)
			tex.loadData(camera.getLiveViewPixels(), camera.getLiveViewPixelWidth(), camera.getLiveViewPixelHeight(), GL_RGB);
	}
}

//--------------------------------------------------------------

void testApp::imageDownloaded(string &path) {
	cout << "took a photo!" << endl; 
	photo.clear(); 
	newFile = camera.lastImagePath;
	cout << "new file: " << newFile << endl; 
	photo.loadImage(newFile);
	photoDupe.loadImage(newFile);
	wingState = 1; 
	go = true; 
	
	
}
//--------------------------------------------------------------
void testApp::saveComp() {
	greenFBO.readToPixels(comp); 
	
	string newName = newFile; 
	ofStringReplace(newName, ".JPG", "");
	//ofStringReplace (newName,"pics/hi/", ""); 
	ofStringReplace (newName,imgPath, ""); 
	saveString = "saved_" + newName + ".tif";
	cout << "save to: " <<  savePath + saveString << endl; 
	ofSaveImage(comp,  savePath + saveString, OF_IMAGE_QUALITY_BEST);  
	if (checkSavedFiles()) {
		savedFileMsg = "Last photo saved to folder: " + saveString;
		statusMsg = "saved successfully.";
		cout << "saved?" << endl; 
	}
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
	
	if (key == 'i') dragIn = !dragIn; 
	if (key == 'o') dragOut = !dragOut; 
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	
	if(!gui.isOn()) {
		if (wingState == 1) { 
			if (x < photoDupe.width && y < photoDupe.height) {
				greenscreen.setBgColor(photoDupe.getPixelsRef().getColor(x, y));
				go = true; 
			}
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
	if (wingState == 0) {
		camera.takePicture();
		//wingState = 1; 
	}
	
}
//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	if (wingState == 1) editing = true; 
}
//--------------------------------------------------------------
void testApp::toggleGUI() {
	cout << "toggle gui" << endl; 
	if (!quickGui->isEnabled()) {
		quickGui->enable();
		gui.setDraw(false);
	} else {
		quickGui->disable(); 
		gui.setDraw(true);
		gui.loadFromXML();
	}
}
//--------------------------------------------------------------
void testApp::toggleStates() {
	if (wingState == 1) {
		wingState = 2; 
	} else if (wingState == 2) {
		wingState = 1; 
		quickOn = true; 
		toggleGUI();
	} else if (wingState == 3) { // if you want to edit again, something else
		wingState = 0;
	}
}

//--------------------------------------------------------------
void testApp::hideAllGui() {
	quickGui->disable();
	//gui.hide();
	gui.setDraw(false);
}

//--------------------------------------------------------------

void testApp::dragEvent(ofDragInfo dragInfo) {
	if (dragIn) {
		imgPath = dragInfo.files[0]+ "/"; 
		dragIn = false; 
	}
	if (dragOut) {
		savePath = dragInfo.files[0]+ "/"; 
		dragOut = false; 
	}
	
	cout << dragInfo.files[0] << endl; 
	
}

//--------------------------------------------------------------

void testApp::drawFileMsgs(int x, int y) {
	
	if (dragIn) {
		ofSetColor(255, 0, 0);
		inputMsg = "Drag new input folder";
		dragOut = false;
	} 	else {
		ofSetColor(255);
		inputMsg = "I to enter a new input folder"; 
	}
	ofDrawBitmapString(inputMsg, x,y + 15);
	ofSetColor(255);
	inPathMsg = "Input folder is: " + imgPath; 
	ofDrawBitmapString(inPathMsg, x, y );
	
	if (dragOut) {
		ofSetColor(255, 0, 0);
		outputMsg = "Drag new output folder";
		dragIn = false;
	} else {
		ofSetColor(255);
		outputMsg = "O to enter a new output folder"; 
	}
	ofDrawBitmapString (outputMsg, x, y + 60);
	ofSetColor(255);
	outPathMsg = "Output folder is: " + savePath; 
	ofDrawBitmapString (outPathMsg, x, y + 45);
	
}
//--------------------------------------------------------------

void testApp::mouseMoved(int x, int y ){};
void testApp::windowResized(int w, int h){};
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
	msgBig.drawString("FPS "+ofToString(ofGetFrameRate()), x, y);	
	msg.drawString(statusMsg, x, y + (inc*2));
	
	ofSetColor(0, 255, 0);
	msg.drawString(totalFilesMsg, x, y+(inc*4));
	msg.drawString(newFileMsg, x, y+(inc*5));
	msg.drawString(savedFileMsg, x, y+(inc*6));
	
	ofSetColor (255); 
	ofNoFill(); 
	ofRect(x - 10, 250 - 20, 350, 30);
	msg.drawString(stateMsg, x, 250);
}
//--------------------------------------------------------------
void testApp::drawInstructionsManual(int x, int y) {
	//interaface 
	
	int inc = 15; 
	ofSetColor(255);
	msg.drawString("Click an area to key.", x, y);
	msg.drawString("Q to toggle Quick Edit/ Detail edit.", x, y + (inc));
}

//--------------------------------------------------------------
void testApp::drawInstructinsAuto(int x, int y) {
	//interaface 
	
	int inc = 15; 
	ofSetColor(255);
	msg.drawString("Click where you want the wings to go.", x, y);
	msg.drawString("Spacebar to change position.", x, y + inc);
	msg.drawString("An image is saved every time.", x, y + (inc*2));
	
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	
	
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
	
	if(name == "Clip Black")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		greenscreen.clipBlackEndMask = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}
	
	if(name == "Clip White")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		greenscreen.clipWhiteEndMask = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}
	
	if(name == "Detail Black")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		greenscreen.clipBlackDetailMask = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}
	
	if(name == "Detail White")
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
	
	quickGui = new ofxUICanvas(x-10, y, length+xInit, ofGetHeight());
	
	
	//quickGui->addLabelToggle("QUICK EDIT", false, length-xInit);	
	
	
	quickGui->addSpacer(length-xInit, 1); 	
	//quickGui->addWidgetDown(new ofxUILabel("Detail Mask Black", OFX_UI_FONT_SMALL)); 	
	quickGui->addSlider("Detail Black", 0.0, 1.f, greenscreen.clipBlackDetailMask, length-xInit,dim);
	
	quickGui->addSpacer(length-xInit, 1); 
	//quickGui->addWidgetDown(new ofxUILabel("Detail Mask White", OFX_UI_FONT_SMALL)); 	
	quickGui->addSlider("Detail White", 0.0, 1.f, greenscreen.clipWhiteDetailMask, length-xInit,dim);
	
	quickGui->addSpacer(length-xInit, 1); 
	//quickGui->addWidgetDown(new ofxUILabel("End Mask Black", OFX_UI_FONT_SMALL)); 	
	quickGui->addSlider("Clip Black", 0.0, 1.f, greenscreen.clipBlackEndMask, length-xInit,dim);
	
	quickGui->addSpacer(length-xInit, 1); 
	//quickGui->addWidgetDown(new ofxUILabel("End Mask White", OFX_UI_FONT_SMALL)); 	
	quickGui->addSlider("Clip White", 0.0, 1.f, greenscreen.clipWhiteEndMask, length-xInit,dim);
	ofAddListener(quickGui->newGUIEvent,this,&testApp::guiEvent);	
	quickGui->enable();
	
	gui.setAutoSave(false);
	gui.loadFromXML();
}
