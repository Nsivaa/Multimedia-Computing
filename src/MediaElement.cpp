#include "MediaElement.h"
#include "ofxOpenCv.h"
void MediaElement::generateThumbnail(int width, int height) {
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
            this->image.resize(width, height); // Resize to image size
            // cout << "Thumbnail successfully created for: " << videoPath << endl;
    }
    catch (const std::exception& e) {
        cout << "Error generating thumbnail: " << e.what() << endl;
    }
}

void MediaElement::computeNormalizedRGBHistogram() {
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

void MediaElement::computeEdgeHistogram(int gridX , int gridY ) {
    vector<float> histogram(gridX * gridY, 0.0f);
	ofImage img = this->image; // Assuming image is already loaded
    // 1. Convert to grayscale
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImg, edgeImg;
    colorImg.allocate(img.getWidth(), img.getHeight());
    grayImg.allocate(img.getWidth(), img.getHeight());
    edgeImg.allocate(img.getWidth(), img.getHeight());

    colorImg.setFromPixels(img.getPixels());
    grayImg = colorImg;

    // 2. Use Sobel filter (via ofxCv) to compute edges
    cv::Mat grayMat = cv::cvarrToMat(grayImg.getCvImage());
    cv::Mat edges;
    cv::Canny(grayMat, edges, 50, 150);  // You can tweak thresholds

    edgeImg.setFromPixels(edges.data, edges.cols, edges.rows);

    // 3. Divide into grid
    int cellW = img.getWidth() / gridX;
    int cellH = img.getHeight() / gridY;

    for (int y = 0; y < gridY; y++) {
        for (int x = 0; x < gridX; x++) {
            int count = 0;

            for (int j = 0; j < cellH; j++) {
                for (int i = 0; i < cellW; i++) {
                    int px = x * cellW + i;
                    int py = y * cellH + j;

                    if (px < img.getWidth() && py < img.getHeight()) {
                        if (edgeImg.getPixels()[py * img.getWidth() + px] > 0) {
                            count++;
                        }
                    }
                }
            }

            histogram[y * gridX + x] = count;
        }
    }

    // 4. Normalize (optional)
    float maxCount = *max_element(histogram.begin(), histogram.end());
    if (maxCount > 0) {
        for (auto& h : histogram) {
            h /= maxCount;
        }
    }

    edgeHist = histogram;


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

void MediaElement::drawNormalizedRGBHistogram(int x, int y, int width, int height) const {
    // Safety check
    if (redHist.empty() || greenHist.empty() || blueHist.empty()) {
        ofLogWarning() << "Histogram data not initialized for media element.";
        return;
    }
	ofFill(); // Fill mode for drawing
    // Draw the histogram for each channel (only first bin for now)
    ofSetColor(ofColor::red);
    ofDrawRectangle(x, y, width / 3, -height * redHist[0]);

    ofSetColor(ofColor::green);
    ofDrawRectangle(x + width / 3, y, width / 3, -height * greenHist[0]);

    ofSetColor(ofColor::blue);
    ofDrawRectangle(x + 2 * width / 3, y, width / 3, -height * blueHist[0]);

    ofSetColor(ofColor::white);
}


void MediaElement::drawEdgeHistogram( int x, int y, int width, int height, int gridX, int gridY) const {
    
    // Safety check
    if (edgeHist.empty()) {
        ofLogWarning() << "Edge histogram data not initialized for media element.";
        return;
    }
	ofFill(); // Fill mode for drawing
    int cellW = width / gridX;
    int cellH = height / gridY;

    for (int j = 0; j < gridY; j++) {
        for (int i = 0; i < gridX; i++) {
            float value = this -> edgeHist [j * gridX + i];
            ofSetColor(255 * value); // brightness proportional to edge density
            ofDrawRectangle(x + i * cellW, y + j * cellH, cellW, cellH);
        }
    }

    ofSetColor(255); // Reset color

}

