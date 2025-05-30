#include "ofMain.h"
#include "MediaElement.h" 
#include "utils.h"

class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();
	void drawSelectedMediaFullscreen();
	void drawLegend();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void setMargin(int margin);
	void setImageIndex(int index);

	ofDirectory dir;
	std::vector<MediaElement> medias;
	ofImage videoIcon;

	int currentMedia = 0;
	int margin = 20;            
	int iconSize = 24;
	int scrollOffsetY = 0;  

	MediaElement* currentVideoPlaying = nullptr;
	bool fullscreenMode = false;
	bool showEdgeHist = false;
	bool showDominantColor = false;
	bool showLuminanceMap = false;
	bool showRGBHist = false;
	bool showLegend = false;

	std::pair<int, int> prevScreenSize = { 1024, 768 }; // to restore screen size when exiting fullscreen
	std::pair<int, int> standardImageSize = { 300, 300 }; // standard image size for the application
};
