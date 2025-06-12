#include "ofMain.h"
#include "MediaElement.h" 
#include "FeatureHandler.h"
#include "MotionDetection.h"
#include "utils.h"


class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();
	void drawSelectedMediaFullscreen();
	void drawLegend();
	void ofApp::drawMediaXMLInfo(const MediaElement& media, int screenW, int screenH);
	void keyPressed(int key);
	void updateMediaMatrix();

	MotionDetection motionDetection;
	ofDirectory dir;
	std::vector<MediaElement> medias;
	std::vector<std::vector<MediaElement*>> mediaMatrix;

	ofImage videoIcon;

	int currentMedia = 0;
	int margin = 20;            
	int iconSize = 24;
	int scrollOffsetX = 0;  
	int selectedRow = 0;
	int selectedCol = 0;


	MediaElement* currentVideoPlaying = nullptr;
	bool fullscreenMode = false;
	bool showEdgeHist = false;
	bool showDominantColor = false;
	bool showLuminanceMap = false;
	bool showRGBHist = false;
	bool showLegend = false;
	bool showInfoWindow = false;

	bool groupByLuminance = false; 
	bool groupByColor = false;
	bool groupByTexture = false;

	std::pair<int, int> prevScreenSize = { 1024, 768 }; // to restore screen size when exiting fullscreen
	std::pair<int, int> standardImageSize = { 280, 280 }; // standard image size for the application
};
