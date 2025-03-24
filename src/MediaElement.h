#pragma once
#include "ofMain.h"
#include "ofApp.h"


class MediaElement {
	// The MediaElement class is used to handle both videos and images in the gallery. 
	// The "image" field will contain the thumbnail of the video if the element is a video, 
	// otherwise the image itself (and the pointer to the video will be null). 


public:
	MediaElement(ofImage img) : image(img) {};
	MediaElement(string path) : videoPath(path) { generateThumbnail(); };

	void generateThumbnail();
	bool hasVideo() const;

	ofImage image;
	string videoPath = "";
	// string xml_data;

	
};

