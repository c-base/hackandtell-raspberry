#pragma once

#include <string>
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxNetwork.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void updateTimeLeft();
    void updateMatelight(std::string minutes);
    void drawStringMono(ofTrueTypeFont* font, std::string text, float x, float y, float w);
    void drawApplause();
    void drawCountDown();
    void resetButtonPressed();
    
    // fonts
    ofTrueTypeFont textFont;
    ofTrueTypeFont signsFont;
    ofTrueTypeFont matelightFont;
    
    ofxButton resetButton;
    ofxToggle paused;
    ofxToggle showMatelightPreview;
    ofxLabel screenSize;
    ofxIntSlider minutes;
    ofxPanel gui;
    
    ofFbo matelightFbo;
    ofPixels matelightPixels;
    ofPixels matelightSmall;
    ofImage matelightPreview;
    
    ofxUDPManager udpConnection;

    bool isMenuHidden;
    bool isTimerRunning;
    bool showApplause;
    int applauseTextColor;
    int applauseBackgroundColor;
    // buffers for variable strings on the display
    char localTimeStr[100];
    char timeLeftStr[100];
    // application state
    
    int millisecondsTotal;
    int millisecondsLeft;
    int lastTime;
    
};
