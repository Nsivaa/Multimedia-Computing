#pragma once
#include "ofMain.h"


class MediaElement {
	// The MediaElement class is used to handle both videos and images in the gallery. 
	// The "image" field will contain the thumbnail of the video if the element is a video, 
	// otherwise the image itself (and the path  to the video will be null). 


public:
	MediaElement() {};
	MediaElement(ofImage img) : image(img) {};
	MediaElement(string path) : videoPath(path) { generateThumbnail(); };

	void generateThumbnail();
	void drawImage(int x, int y) { image.draw(x, y); };
	void drawImageWithContour(int x, int y, ofColor contourColor = ofColor::white, int thickness = 5) const;
	bool isVideo() const { return !(this->videoPath.empty()); };
	void computeNormalizedHistogram();
	void drawHistogram(int x, int y, int width, int height) const;

	bool isPaused = false; // needed as openFramework's "isPlaying()" returns true evern if the video is currently paused

	ofVideoPlayer videoPlayer; // Video player for the video element
	ofImage image;
	string videoPath = "";
	// string xml_data;

	std::vector<float> redHist, greenHist, blueHist;

};

