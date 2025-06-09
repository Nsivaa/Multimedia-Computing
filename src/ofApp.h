#include "ofMain.h"
#include "MediaElement.h" 
#include "FeatureHandler.h"
#include "utils.h"

class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();
	void drawSelectedMediaFullscreen();
	void drawLegend();
	void keyPressed(int key);
	
	ofDirectory dir;
	std::vector<MediaElement> medias;
	ofImage videoIcon;

	int currentMedia = 0;
	int margin = 20;            
	int iconSize = 24;
	int scrollOffsetX = 0;  

	MediaElement* currentVideoPlaying = nullptr;
	bool fullscreenMode = false;
	bool showEdgeHist = false;
	bool showDominantColor = false;
	bool showLuminanceMap = false;
	bool showRGBHist = false;
	bool showLegend = false;

	bool luminanceGrouping = false; 
	bool colorGrouping = false;
	bool textureGrouping = false;

	std::pair<int, int> prevScreenSize = { 1024, 768 }; // to restore screen size when exiting fullscreen
	std::pair<int, int> standardImageSize = { 300, 300 }; // standard image size for the application
};
