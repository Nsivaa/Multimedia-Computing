#pragma once
#include "ofMain.h"
#include "utils.h"

class MediaElement {
	// The MediaElement class is used to handle both videos and images in the gallery. 
	// The "image" field will contain the thumbnail of the video if the element is a video, 
	// otherwise the image itself (and the path  to the video will be null). 

public:
	
	// CONSTRUCTORS

	MediaElement() {};
	MediaElement(ofImage img) : image(img) {};
	MediaElement(ofImage img, int width, int height) : image(img) { image.resize(width, height); }; // resize the image to the given width and height
	MediaElement(string path) : videoPath(path) {}; // constructor for video elements
	
	// MISC METHODS

	bool isVideo() const { return !(this->videoPath.empty()); };
	ofColor getHeatmapColor(float value); // Returns a color based on the luminance value for heatmap visualization

	// DRAWER METHODS 

	void drawImage(int x, int y) const { image.draw(x, y); };
	void drawImageWithContour(int x, int y, ofColor contourColor = ofColor::white, int thickness = 5) const;
	void drawEdgeMap(int x, int y, int width, int height) const;
	void drawNormalizedRGBHistogram(int x, int y, int width, int height) const;
	void drawLuminanceMap(int x, int y) const;

	// ATTRIBUTES

	bool isPaused = false; // needed as openFramework's "isPlaying()" returns true evern if the video is currently paused
	ofVideoPlayer videoPlayer; // Video player for the video element
	ofImage image;
	string videoPath = "";
	// string xml_data;
	ofColor dominantColor;
	ofImage luminanceMap; // Heatmap-style luminance visualization
	LuminanceGroup luminanceGroup = LOW; // Grouping of luminance values into LOW, MEDIUM, HIGH
	ColorGroup colorGroup = RED; // Grouping of colors into RED, GREEN, BLUE
	TextureGroup textureGroup = SMOOTH_TEXTURE; // Grouping of textures into SMOOTH, MEDIUM, COARSE
	RhythmGroup rhythmGroup = STATIC; // Grouping of rhythm into STATIC, MODERATE, FAST
	std::vector<float> redHist, greenHist, blueHist;
	std::vector<float> edgeHist; // Edge histogram
	int edgeGridRows = 32;
	int edgeGridCols = 32;
	float averageLuminance = 0;
	float textureVariance = 0.0f;
	float rhythmMetric = 0.0f; // Metric for rhythm analysis
};

