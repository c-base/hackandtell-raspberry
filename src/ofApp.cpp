#include <ctime>
#include <iostream>
#include <iterator>
#include <locale>
#include <regex>
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
#if defined(WITH_GPIO)
    if(wiringPiSetup() == -1){
    	printf("Error on wiringPi setup");
    }
    pinMode(8,OUTPUT);
#endif
    ofSetVerticalSync(true);
    resetButton.addListener(this,&ofApp::resetButtonPressed);
    
    matelightFont.loadFont("OSP-DIN.ttf", 68, true, true, true);
    signsFont.loadFont("OSP-DIN.ttf", 100, true, true, true );
    textFont.loadFont("OSP-DIN.ttf", 32, true, true, true);
    
    //ofxGuiSetFont("OSP-DIN.ttf", 14, true, true, true);
    //ofxGuiSetDefaultHeight(22);
    
    lastTime = 0;
    isMenuHidden = false;
    isTimerRunning = false;
    gui.setup("Main menu (h)");
    // gui.add(screenSize.setup("Main menu", ""));
    gui.add(resetButton.setup("Reset timer (r)"));
    gui.add(paused.setup("Pause (p/space)", true));
    gui.add(minutes.setup("Minutes", 5, 1, 120));
    gui.add(showApplause.setup("Show applause (a)", false));
    gui.add(showWinners.setup("Show winners (w)", false));
    gui.add(showMatelightPreview.setup("Show ML preview", false));

    
    millisecondsTotal = minutes;
    millisecondsLeft = minutes * 60 * 1000;
    
    matelightFbo.allocate(160, 64, GL_RGBA);
    matelightPixels.allocate(160, 64, OF_PIXELS_RGBA);
    matelightSmall.allocate(40, 16, OF_PIXELS_RGBA);
    
    udpConnection.Create();
    udpConnection.Connect("10.0.1.39", 1337);
    udpConnection.SetNonBlocking(true);

    ofRegisterURLNotification(this);
}

void ofApp::resetButtonPressed() {
    paused = true;
    showApplause = false;
    millisecondsLeft = minutes * 60 * 1000;
}


//--------------------------------------------------------------
void ofApp::update(){
    std::string s1;

    updateTimeLeft();

    if (showWinners) {
        s1 = winners;
    } else {
        // update time left display
        int minutes = millisecondsLeft / 1000 / 60;
        int seconds = (millisecondsLeft - (minutes * 60 * 1000)) / 1000;
        int millis = millisecondsLeft - (minutes * 1000 * 60) - (seconds * 1000);

        std::stringstream ss;
        ss << setfill('0') << setw(2) << seconds;
        std::stringstream ms;
        ms << setfill('0') << setw(3) << millis;

        s1 = ofToString(minutes) + ":" + ofToString(ss.str());
        std::string s2 = s1 + "." + ofToString(ms.str());

        strncpy(timeLeftStr, s2.c_str(), sizeof(timeLeftStr));
        // update the local time
        std::time_t t = std::time(NULL);
        std::strftime(localTimeStr, sizeof(localTimeStr), "%H:%M:%S", std::localtime(&t));
    }
    
    updateMatelight(s1);
}

//--------------------------------------------------------------
void ofApp::updateTimeLeft(){
    if (showApplause || showWinners) {
        return;
    }
    if (paused) {
        lastTime = 0;
        return;
    }
    int now = ofGetElapsedTimeMillis();
    int elapsed = now - lastTime;
    if (lastTime == 0) {
        lastTime = now;
        return;
    }
    millisecondsLeft = millisecondsLeft - elapsed;
    lastTime = now;
    if (millisecondsLeft <= 0) {
        showApplause = true;
    }

}

//--------------------------------------------------------------
void ofApp::updateMatelight(std::string text) {
    matelightFbo.begin();
    ofClear(255,255,255, 0);
    ofBackground(0, 0, 0);
    ofSetColor(150, 0, 150);
    drawStringMono(&matelightFont, text, 20, 62, 34);
    ofSetColor(255, 255, 255);
    matelightFbo.end();
    
    matelightFbo.readToPixels(matelightPixels);
    matelightPixels.resizeTo(matelightSmall, OF_INTERPOLATE_NEAREST_NEIGHBOR);
    matelightPreview.setFromPixels(matelightSmall);
    
    
    char message[1924];

    int size = matelightSmall.size();
    int idx = 0;
    for(int y = 0; y < 16; y++){
        for(int x = 0; x < 40; x++) {
            ofColor_<unsigned char> c = matelightSmall.getColor(x, y);
            char r = c.r;
            char g = c.g;
            char b = c.b;
            message[idx + 0] = r;
            message[idx + 1] = g;
            message[idx + 2] = b;
            idx = idx + 3;
        }
    }
    message[1920] = 0x0;
    message[1921] = 0x0;
    message[1922] = 0x0;
    message[1923] = 0x0;
    udpConnection.Send(message, 1924);
}

void ofApp::urlResponse(ofHttpResponse & response) {
    if (response.status == 200 && response.request.name == "async_req") {
        std::string s = response.data.getText();
        std::regex regex("[0-9]+");
        winners.clear();
        for (std::sregex_iterator i = std::sregex_iterator(s.begin(), s.end(), regex); i != std::sregex_iterator(); ++i) {
            std::smatch match = *i;
            winners += match.str() + " ";
        }
        // if there are no winners this will show a blank screen
        // allowing the game master to retry getting the winners
        showWinners = true;
    } else {
        ofRemoveAllURLRequests();
        ofStopURLLoader();
        // cerr << response.status << " " << response.error << endl;
    }
}

//--------------------------------------------------------------
void ofApp::drawStringMono(ofTrueTypeFont* font, std::string text, float x, float y, float w) {
    for (int i = 0 ; i < text.length(); i++) {
        std::string c = ofToString(text[i]);
        
        // some characters (e.g. '.' and ':' are very small and need to be
        // shifted to the center of the character. The variable 'add' is added
        //
        float add = 0;
        if (c == ":" || c == ".") {
            add = w * 0.24;
        }
        font->drawString(c, x + (i * w)+ add, y);
    }
    
}

//--------------------------------------------------------------
void ofApp::drawApplause() {
    int now = ofGetSeconds();
    if (now % 2 == 0) {
#if defined(WITH_GPIO)
        digitalWrite(8, HIGH) ;
#endif
        ofSetColor(0, 0, 0);
        ofBackground(255, 255, 255);
        signsFont.drawString("APPLAUSE", 155, 250);
    } else {
#if defined(WITH_GPIO)
        digitalWrite(8, LOW) ;
#endif
        ofBackground(0, 0, 0);
        ofSetColor(255, 255, 255);
        signsFont.drawString("APPLAUSE", 155, 250);
    }
}

//--------------------------------------------------------------
void ofApp::drawCountDown() {
    ofBackground(0, 0, 0);
    textFont.drawString("Time left", 104, 150);
    drawStringMono(&signsFont, timeLeftStr, 100, 260, 50);
    ofSetColor(255, 255, 255);
    textFont.drawString("Current local time", 104, 380);
    drawStringMono(&signsFont, localTimeStr, 100, 490, 50);
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (showApplause) {
        drawApplause();
    } else {
#if defined(WITH_GPIO)
        digitalWrite(8, LOW);
#endif
        drawCountDown();
    }
    if (showMatelightPreview) {
        matelightPreview.draw(680, 560);
    }
    if (!isMenuHidden) {
        gui.draw();
    }
}


//--------------------------------------------------------------
void ofApp::exit()
{
    resetButton.removeListener(this,&ofApp::resetButtonPressed);
    ofUnregisterURLNotification(this);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
    case 'h':
    case 'H':
        isMenuHidden = !isMenuHidden;
        break;
    case 'a':
    case 'A':
        showApplause = !showApplause;
        paused = true;
        break;
    case 'r':
    case 'R':
        resetButtonPressed();
        break;
    case 'w':
    case 'W':
        // if not shown, async HTTP GET winners
        if (!showWinners)
            ofLoadURLAsync("http://localhost:80/winners", "async_req");
        // if shown, hide
        showWinners = false;
        break;
    case 'p':
    case 'P':
    case ' ':
        paused = !paused;
        break;
    case 'q':
    case 'Q':
        ofExit();
        break;
    default:
        /* do nothing with unknown keys */
        break;
    }
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
