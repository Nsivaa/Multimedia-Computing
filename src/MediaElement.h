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
	bool isVideo() const;

	ofImage image;
	string videoPath = "";
	// string xml_data;


};

