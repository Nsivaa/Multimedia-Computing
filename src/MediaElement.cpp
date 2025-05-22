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

void MediaElement::drawImageWithContour(int x, int y, ofColor contourColor, int thickness) const {
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

void MediaElement::drawHistogram(int x, int y, int width, int height) const {
    // Safety check
    if (redHist.empty() || greenHist.empty() || blueHist.empty()) {
        ofLogWarning() << "Histogram data not initialized for media element.";
        return;
    }

    // Draw the histogram for each channel (only first bin for now)
    ofSetColor(ofColor::red);
    ofDrawRectangle(x, y, width / 3, -height * redHist[0]);

    ofSetColor(ofColor::green);
    ofDrawRectangle(x + width / 3, y, width / 3, -height * greenHist[0]);

    ofSetColor(ofColor::blue);
    ofDrawRectangle(x + 2 * width / 3, y, width / 3, -height * blueHist[0]);

    ofSetColor(ofColor::white);
}


void MediaElement::computeNormalizedHistogram() {
	// Compute histograms for each channel
	ofPixels pixels = this->image.getPixels();
	int numPixels = pixels.size() / 3; // Assuming RGB image
    const int numBins = 256;
    redHist.assign(numBins, 0.0f);
    greenHist.assign(numBins, 0.0f);
    blueHist.assign(numBins, 0.0f);

    for (int y = 0; y < this->image.getHeight(); y++) {
        for (int x = 0; x < this->image.getWidth(); x++) {
            ofColor c = this->image.getColor(x, y);
            redHist[c.r]++;
            greenHist[c.g]++;
            blueHist[c.b]++;
        }
    }

    // Normalize each histogram
    for (int i = 0; i < numBins; i++) {
        redHist[i] /= numPixels;
        greenHist[i] /= numPixels;
        blueHist[i] /= numPixels;
    }
}