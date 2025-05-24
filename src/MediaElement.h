#pragma once
#include "ofMain.h"


class MediaElement {
	// The MediaElement class is used to handle both videos and images in the gallery. 
	// The "image" field will contain the thumbnail of the video if the element is a video, 
	// otherwise the image itself (and the path  to the video will be null). 


public:
	MediaElement() {};
	MediaElement(ofImage img) : image(img) {};
	MediaElement(ofImage img, int width, int height) : image(img) { image.resize(width, height); }; // resize the image to the given width and height
	MediaElement(string path) : videoPath(path) { generateThumbnail(); };

	void generateThumbnail(int width = 300, int height = 300);
	void drawImage(int x, int y) const { image.draw(x, y); };
	void drawImageWithContour(int x, int y, ofColor contourColor = ofColor::white, int thickness = 5) const;
	bool isVideo() const { return !(this->videoPath.empty()); };
	void computeNormalizedRGBHistogram();
	void drawNormalizedRGBHistogram(int x, int y, int width, int height) const;
	void computeEdgeHistogram(int gridX = 4, int gridY = 4);
	void drawEdgeHistogram(int x, int y, int width, int height, int gridX, int gridY) const;

	bool isPaused = false; // needed as openFramework's "isPlaying()" returns true evern if the video is currently paused
	ofVideoPlayer videoPlayer; // Video player for the video element
	ofImage image;
	string videoPath = "";
	// string xml_data;

	std::vector<float> redHist, greenHist, blueHist;
	std::vector<float> edgeHist; // Edge histogram
};

