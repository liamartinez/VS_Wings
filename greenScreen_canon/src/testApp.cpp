#include "testApp.h"

#define USE_GUI

#ifdef USE_GUI
#include "ofxSimpleGuiToo.h"
//float bgColor[4];
ofFloatColor bgCol; 
bool saveImgs;
#endif

testApp::testApp() {
	texIsInitialised = false;
}

//--------------------------------------------------------------
void testApp::setup() { 
	
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
		ofAddListener(camera.evtImageDownloaded, this, &testApp::imageDownloaded);
	}
	
	curPic = 0; //current picture. start from default 0.
	hires = false; //start with lores as default 
	
	//ofdirectory
	dir.allowExt("jpg");
	dir.setShowHidden(false);
	
	
	//set image paths before loading
	imgPath = "pics/low/" + newFile; 
	imgPathHi = "pics/hi/" + newFile; 
	
	
	if (hires) {
		dir.listDir("pics/hi");

	} else {
		dir.listDir("pics/low");
	} 
	
	numFiles =  dir.numFiles();
	if (numFiles > 0 ) {
		wingState = 1; 
	} else {
		wingState = 4; 
	}

	if (wingState != 4) {
		setupImgs(); 	

	}
	
	

	
	setupGUI(); 
	ofBackground(0);
}

//--------------------------------------------------------------
void testApp::setupImgs() {
	
	totalFiles = numFiles; 
	newFile = dir.getName(totalFiles-1);
	for(int i = 0; i < numFiles; i++){
		cout << dir.getPath(i) << endl;
	}
	
	//load bg and wings
	bgImg.loadImage ("pics/stage-background.jpg"); 
	wings.loadImage ("pics/angelwings.png"); 
	wingsHI.loadImage ("pics/angelwingsHI.png"); 
	
	comp.allocate(512, 360, 4);
	origTex.allocate(512, 360, 4);
	texGreen.allocate(512, 360, 4);
	greenFBO.allocate(512, 360);
	tempImage.allocate(512, 360, OF_IMAGE_COLOR);
	
	
	/*
	if (hires) {
		greenPic[curPic].loadImage(imgPathHi); 
		greenPicOrig[curPic].loadImage(imgPathHi); 
	} else {
		greenPic[curPic].loadImage(imgPath); 
		greenPicOrig[curPic].loadImage(imgPath); 
	}
	*/
	//greenscreen the first frame
	/*
	greenscreen.clear();
	greenscreen.setPixels(greenPic[curPic].getPixelsRef());
	comp.allocate(greenPic[curPic].width, greenPic[curPic].height, 4);
	greenFBO.allocate(greenPic[curPic].width, greenPic[curPic].height);
	*/
	picOn = false; //original pic
	go = true; //make the greenscreening a one-burst event
	
	//does this work?
	ofColor startColor(20, 200, 20);
	greenscreen.setBgColor(startColor);
	
	saveString = "Nothing saved yet.";
	wingState = 1; 
	
	
	isEmpty = false; 

}


//--------------------------------------------------------------
void testApp::update() {
	
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
	
	if (isEmpty) {
		setupImgs(); 
		toggleFunc();
		isEmpty = false; 
	}
	
	//check the directory to see if there is a new file.
	/*
	if (hires) {
		dir.listDir("pics/hi");
	} else {
		dir.listDir("pics/low");
	}
	
	if (dir.numFiles() > numFiles) {
		totalFiles++; 
		curPic = totalFiles - 1; 
		newFile = dir.getName(totalFiles-1);
		cout << "name of new file: " << newFile << endl; 
		numFiles = dir.numFiles(); 
		cout << "numFiles " << numFiles << endl; 
		wingState = 2; 
		go = true; 
		isEmpty = true; 
	}
	*/

	
	if (wingState !=4) {
		//if (go || editing) {
		cout << "                                go" << endl;
		

		//greenFBO.allocate(greenPic[curPic].width, greenPic[curPic].height);
		//comp.allocate(greenPic[curPic].width, greenPic[curPic].height, 4);

		tex.readToPixels(origTex);
		tex.readToPixels(texGreen);
	
		greenscreen.setPixels(texGreen);
		//greenscreen.setPixels(greenPic[curPic].getPixelsRef());

		go = false; 
			//}
	
#ifdef USE_GUI
	if(gui.isOn()) {

		greenscreen.setBgColor(ofColor(bgCol.r*255, bgCol.g*255, bgCol.b*255));
		//if(grabber.isFrameNew()) {
		//THIS PART IS REALLY SLOW!!!
		mask.setFromPixels(greenscreen.getMask());
		detailMask.setFromPixels(greenscreen.getDetailMask());
		baseMask.setFromPixels(greenscreen.getBaseMask());
		chromaMask.setFromPixels(greenscreen.getChromaMask());
		
		redSub.setFromPixels(greenscreen.getRedSub());
		greenSub.setFromPixels(greenscreen.getGreenSub());
		blueSub.setFromPixels(greenscreen.getBlueSub());
		//}
		
		if(saveImgs){
			mask.saveImage("save/mask.jpg", OF_IMAGE_QUALITY_BEST);
			detailMask.saveImage("save/detailMask.jpg", OF_IMAGE_QUALITY_BEST);
			baseMask.saveImage("save/baseMask.png", OF_IMAGE_QUALITY_BEST);
			redSub.saveImage("save/redSub.png" , OF_IMAGE_QUALITY_BEST);
			greenSub.saveImage("save/greenSub.png", OF_IMAGE_QUALITY_BEST);
			blueSub.saveImage("save/blueSub.png", OF_IMAGE_QUALITY_BEST);
			greenscreen.saveImage("save/composition.png", OF_IMAGE_QUALITY_BEST);
			saveImgs = false;
		}
		
	}
	
	if (saveNow) {
		greenFBO.readToPixels(comp); 

		string newName = camera.lastImagePath; 
		ofStringReplace(newName, ".JPG", "_");

		ofSaveImage(comp, saveString, OF_IMAGE_QUALITY_BEST);  
		saveNow = false; 
		isSaved = true;
	}
	}
	
#endif
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
		bgImg.draw(0,0, greenPic[curPic].width, greenPic[curPic].height); 

		switch (wingState) {
			case 1:
				//cout << "case 1: this is where editing is possible" << endl; 

				//this is where you can choose the background color. see mousePressed()
				break;
			
			case 2: 
				//cout << "case 2: position wings" << endl; 

				if (hires) {
				wingXoff = (ofGetMouseX()*(3.8667)) - wingsHI.width/2; 
				wingYoff = (ofGetMouseY()*(3.8667)) - (wingsHI.height - wingsHI.height/3); 
				wingsHI.draw(wingXoff, wingYoff); //greenscreen pic hires/lowres
				} else {
				wingXoff = ofGetMouseX() - wings.width/2; 
				wingYoff = ofGetMouseY() - (wings.height - wings.height/3); 
				wings.draw(wingXoff, wingYoff); //greenscreen pic hires/lowres	
				}
				
				break; 
				
			case 3:
				//cout << "case 3: save wing pos" << endl; 
				wingPos.x = wingXoff; 
				wingPos.y = wingYoff; 
				wingPos.z = 1.0; 
				
				if (hires) {
					wingsHI.draw(wingPos);	
				} else {
					wings.draw(wingPos);
				}

				break; 
		}

	//greenscreen.draw(0, 0, greenscreen.getWidth(), greenscreen.getHeight());
	greenscreen.draw(0, 0, 512, 360);
		
		//if (picOn) greenPic[curPic].draw(0, 0);
		greenscreen.drawBgColor();
		
		ofDisableAlphaBlending(); 
	greenFBO.end();

	//really draw
	ofSetColor (255); 
	//greenFBO.draw(0, 0, 480, 720);
	greenFBO.draw(0, 0, 512, 360);
	
	//tex.draw(0,0,512,360);
	ofDrawBitmapString(ofToString(camera.getLiveViewPixelWidth()), 0, 10);
	ofDrawBitmapString(ofToString(camera.getLiveViewPixelHeight()), 0, 20);
	
	if (testLoad.isAllocated())
	{
		cout << "allocated?" << endl; 
		//testLoad.draw(512,0,512,512.0f*2.0f/3.0f);
		testLoad.draw(0, 360, 512, 360); 
		ofDrawBitmapString(ofToString(testLoad.getWidth()), 512, 10);
		ofDrawBitmapString(ofToString(testLoad.getHeight()), 512, 20);
	}

	switch (wingState) {
			
		case 1:
		isSaved = false; 
		drawInstructionsManual();
		#ifdef USE_GUI
					gui.draw();
		#endif
			break;
			
		case 2:
		isSaved = false; 
		drawInstructionsAuto();
			break;
			
		case 3:
		//enable save. 
		if (!isSaved) saveNow = true; 
		drawInstructionsAuto();
			break;
			
		case 4: 
			ofSetColor(0);
			ofRect(0, 0, ofGetWidth(), ofGetHeight()); 
			ofSetColor(255);
			if (hires) {
				ofDrawBitmapString("File Folder is empty. Please Load photo into: " + imgPathHi, ofGetWidth()/2 - 100, ofGetHeight()/2);
			} else {
				ofDrawBitmapString("File Folder is empty. Please Load photo into: " + imgPath, ofGetWidth()/2 - 100, ofGetHeight()/2);
			}
	}

	//ofSetColor(255);
	//tempImage.draw(0, 360, 512, 360);
}

//--------------------------------------------------------------
void testApp::drawInstructionsManual() {
	//interaface 
	ofSetColor(255,0,0);
	ofDrawBitmapString("FPS "+ofToString(ofGetFrameRate()), 500, 20);
	if (hires) {
		ofDrawBitmapString("HIRES IS ON - H TO TOGGLE", 500, 35);
	} else {
		ofDrawBitmapString("HIRES IS OFF - H TO TOGGLE", 500, 35);
	}
	ofSetColor(255);
	ofDrawBitmapString("1. CLICK AN AREA TO KEY", 500, 100);
	ofDrawBitmapString("2. Q FOR INTENSE EDIT", 500, 115);
	ofDrawBitmapString("3. SPACEBAR TO TOGGLE QUICK EDIT AND WING MODE", 500, 130);
	ofDrawBitmapString("4. S TO SAVE MANUALLY", 500, 145);
	ofDrawBitmapString("5. H TO TOGGLE RES", 500, 160);
	
	ofSetColor(0, 255, 0); 
	ofDrawBitmapString("Loading image: " + newFile, 500, 185);
	ofDrawBitmapString("Saved to data folder: " + saveString, 500, 200);
	
		ofDrawBitmapString("YOU ARE IN MANUAL MODE. SPACEBAR TO GO TO AUTO MODE", 500, ofGetHeight() - 100); 
}

//--------------------------------------------------------------
void testApp::drawInstructionsAuto() {
	ofSetColor(255,0,0);

	ofSetColor(0, 255, 0); 
	ofDrawBitmapString("Loading image: " + newFile, 500, 185);
	ofDrawBitmapString("Saved to data folder: " + saveString, 500, 200);
	
	ofDrawBitmapString("YOU ARE IN AUTO MODE. SPACEBAR TO GO TO MANUAL MODE", 500, ofGetHeight() - 100); 

	
}
//--------------------------------------------------------------

void testApp::keyPressed(int key) {
	if(key == OF_KEY_DOWN)
		greenscreen.clipBlackEndMask -= .01;
	if(key == OF_KEY_UP)
		greenscreen.clipBlackEndMask += .01;
	if(key == OF_KEY_LEFT)
		//greenscreen.clipWhiteEndMask -= .01;
		if (curPic > 0) {
			curPic --; 
		} else {
			curPic = 0; 
		}
	if(key == OF_KEY_RIGHT)
		//greenscreen.clipWhiteEndMask += .01;
		if (curPic < totalFiles-1 ) {
			curPic ++; 
		} else {
			curPic = totalFiles-1; 
		}
	
	
	cout << "curpic: " << curPic << endl; 
	
	go = true; 
	
	if (key == 'w') {
		wingState = 2; 
	}

	/*
	if (key == 'e') {
		wingState = wingState % 3; 
		wingState ++; 
	}
	 */
	
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
#ifdef USE_GUI
	if(key == 'q') {
		if (quickToggle) quickGui->toggleVisible(); 
		gui.toggleDraw();
		guiToggle = !guiToggle; 
	}
	
#endif
	
	if (key == 'p') picOn = !picOn; 
	if(key=='s') {
		
		greenFBO.readToPixels(comp); 
		
		if (hires) {
			saveString = "saved_" + ofToString(curPic) + "_hi.tif"; 
		} else {
			saveString = "saved_" + ofToString(curPic) + "_lo.tif"; 
		}
		
		ofSaveImage(comp, saveString, OF_IMAGE_QUALITY_BEST);  
		
	}
	
	
	if (key == 'h') {
		hires = !hires; 
		go = true;
	}
	
	if (key == ' ') {
		toggleFunc();
		
	}
	
	if (quickToggle) {
		wingState = 1; 
	} else {
		wingState = 2; 
	}
	
	if (key=='1')
		camera.takePicture();


}

void testApp::imageDownloaded(string &path) {
	testLoad.loadImage(camera.lastImagePath);
}

//--------------------------------------------------------------

void testApp::toggleFunc() {
	if (gui.isOn()) gui.hide();
		quickToggle = !quickToggle; 
		quickGui->toggleVisible(); 
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	//greenPic.loadImage ("pics/IMG_1482_small.JPG"); 
	/*
	 if(x== dragStart.x || y==dragStart.y)
	 return;
	 greenscreen.learnBgColor(greenPic.getPixelsRef(), dragStart.x, dragStart.y, x-dragStart.x, y-dragStart.y);
	 */
	
	if (wingState == 0) editing = true; 
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
#ifdef USE_GUI
	if(!gui.isOn())
#endif
	if (wingState == 1) {
		//greenscreen.setBgColor(greenPicOrig[curPic].getPixelsRef().getColor(x, y));
		//tempImage.setFromPixels(origTex, 512, 360, OF_IMAGE_COLOR, true);

		
		tempImage.setFromPixels(origTex);

		greenscreen.setBgColor(tempImage.getPixelsRef().getColor(x, y));
	cout << "get color" << endl; 
	} else if (wingState == 2) {
		wingState = 3; 
	}
	ofColor pc; 
#ifdef USE_GUI
	
	ofColor c = greenscreen.getBgColor();
	bgCol.r = c.r/255.;
	bgCol.g = c.g/255.;
	bgCol.b = c.b/255.;
	
	
#endif
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
	
	if (hires != oldRes) {
		oldRes = hires; 
	}
	
	go = true;
	editing = false; 
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
void testApp::windowResized(int w, int h) {
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg) {
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo) {
	
}

//--------------------------------------------------------------
void testApp::setupGUI() {
#ifdef USE_GUI
	gui.addTitle("SETTINGS");
	gui.addToggle("detail mask", greenscreen.doDetailMask);
	gui.addToggle("base mask", greenscreen.doBaseMask);
	gui.addToggle("chroma mask", greenscreen.doChromaMask);
	gui.addToggle("greenspill", greenscreen.doGreenSpill);
	
	//gui.addColorPicker("key color", bgColor[0]);
	gui.addColorPicker("key color", bgCol);
	gui.addSlider("base mask strength", greenscreen.strengthBaseMask, 0.0, 1.f);
	gui.addSlider("chroma mask strength", greenscreen.strengthChromaMask, 0.0, 1.f);
	gui.addSlider("green spill strength", greenscreen.strengthGreenSpill, 0.0, 1.f);
	
	
	//gui.addTitle("CLIPPING");
	gui.addSlider("base mask black", greenscreen.clipBlackBaseMask, 0.0, 1.f);
	gui.addSlider("base mask white", greenscreen.clipWhiteBaseMask, 0.0, 1.f);
	gui.addSlider("detail mask black", greenscreen.clipBlackDetailMask, 0.0, 1.f);
	gui.addSlider("detail mask white", greenscreen.clipWhiteDetailMask, 0.0, 1.f);
	gui.addSlider("end mask black", greenscreen.clipBlackEndMask, 0.0, 1.f);
	gui.addSlider("end mask white", greenscreen.clipWhiteEndMask, 0.0, 1.f);
	
	gui.addTitle("OUTPUT");
	gui.addFPSCounter();
	gui.addButton("save images", saveImgs);
	
	//gui.addContent("camera", grabber);
	//gui.addContent("picture", greenPic[curPic]);
	gui.addContent("base mask", baseMask);
	gui.addContent("detail mask", detailMask);
	gui.addContent("chroma mask", chromaMask);
	gui.addContent("mask", mask);
	
	gui.addContent("red sub", redSub);
	gui.addContent("green sub", greenSub);
	gui.addContent("blue sub", blueSub);
	gui.addContent("keyed", greenscreen);
	
	
#endif
	
	//ofxGUI
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 320-xInit; 
	float dim = 24; 
	
	quickGui = new ofxUICanvas(ofGetWidth()/2 - 20, ofGetHeight()/2 - 170, length+xInit, ofGetHeight());
	
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
