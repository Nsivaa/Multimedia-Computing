#include "MediaElement.h"

void MediaElement::generateThumbnail() {
	if (!this->isVideo()) {
		return;
	}
    try {
        ofVideoPlayer tempVideo;
        tempVideo.load(this->videoPath);
        while (!tempVideo.isFrameNew()) {
			tempVideo.update(); // Wait for the first frame to be loaded
        }
            this->image.setFromPixels(tempVideo.getPixels());
            this->image.resize(300, 300); // Resize to image size
            // cout << "Thumbnail successfully created for: " << videoPath << endl;
    }
    catch (const std::exception& e) {
        cout << "Error generating thumbnail: " << e.what() << endl;
    }
}

void MediaElement::drawImageWithContour(int x, int y, ofColor contourColor, int thickness) {
    // Draw the image itself
    this->image.draw(x, y);

    // Set the color for the contour
    ofSetColor(contourColor);
    ofNoFill(); // No fill for the contour—just an outline

    for (int i = 0; i < thickness; i++) {
        ofDrawRectangle(x - i, y - i, this->image.getWidth() + 2 * i, this->image.getHeight() + 2 * i);
    }

    // Reset color back to white (for other drawings after this)
    ofSetColor(ofColor::white);
}
