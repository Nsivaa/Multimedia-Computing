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

void MediaElement::computeEdgeMap() {
	int gridX = this->edgeGridCols;
	int gridY = this->edgeGridRows;
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

void MediaElement::computeDominantColor() {
    if (!image.isAllocated()) return;

    ofPixels& pixels = image.getPixels();
    uint64_t r = 0, g = 0, b = 0;
    int count = 0;

    for (int i = 0; i < pixels.size(); i += 3) {
        r += pixels[i];
        g += pixels[i + 1];
        b += pixels[i + 2];
        count++;
    }

    if (count > 0) {
        r /= count;
        g /= count;
        b /= count;
        dominantColor = ofColor(r, g, b);
    }
}

void MediaElement::computeLuminanceMap() {
    ofPixels& pixels = image.getPixels();
    int w = pixels.getWidth();
    int h = pixels.getHeight();

    ofPixels heatmapPixels;
    heatmapPixels.allocate(w, h, OF_PIXELS_RGB);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            ofColor color = pixels.getColor(x, y);
            float luminance = color.getLightness(); 
            // Normalize luminance to [0,1]
            float normLum = ofClamp(luminance / 255.0f, 0.0f, 1.0f);

            // Convert to heatmap color
            ofColor heatColor = getHeatmapColor(normLum);
            heatmapPixels.setColor(x, y, heatColor);
        }
    }

    luminanceMap.setFromPixels(heatmapPixels);
}

void MediaElement::computeAverageLuminance() {
    ofPixels& pixels = image.getPixels();
    float totalLuminance = 0;

    for (int y = 0; y < pixels.getHeight(); y++) {
        for (int x = 0; x < pixels.getWidth(); x++) {
            ofColor color = pixels.getColor(x, y);
            float luminance = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
            totalLuminance += luminance;
        }
    }

    averageLuminance = totalLuminance / (pixels.getWidth() * pixels.getHeight());
}


ofColor MediaElement::getHeatmapColor(float value) {
    value = ofClamp(value, 0.0, 1.0);

    // Simple blue-green-red heatmap gradient
    if (value < 0.5f) {
        return ofColor(0, 2 * value * 255, 255); // Blue to Cyan
    }
    else {
        return ofColor(2 * (value - 0.5f) * 255, 255 - 2 * (value - 0.5f) * 255, 0); // Yellow to Red
    }
}

void MediaElement::assignLuminanceGroup() {
    computeAverageLuminance();

    if (averageLuminance < 85) {
        luminanceGroup = LOW;
    }
    else if (averageLuminance < 170) {
        luminanceGroup = MEDIUM;
    }
    else {
        luminanceGroup = HIGH;
    }
}

// -------------------------------------------------------------------------------------------------------------------------
// DRAWER METHODS 
// -------------------------------------------------------------------------------------------------------------------------

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

    const int numBins = (int)redHist.size(); // Assuming all histograms have same bin count
    const float sectionWidth = width / 3.0f; // Width allocated per color channel
    const float barSpacing = 1.0f;           // Spacing between bars in pixels

    ofFill();

    // Helper lambda to draw a single color histogram
    auto drawHistogram = [&](const std::vector<float>& hist, float startX, const ofColor& color) {
        ofSetColor(color);
        float barWidth = (sectionWidth - (numBins - 1) * barSpacing) / numBins;

        for (int i = 0; i < numBins; ++i) {
            float barHeight = height * hist[i];
            float barX = startX + i * (barWidth + barSpacing);
            ofDrawRectangle(barX, y, barWidth, -barHeight);
        }
    };

    // Draw Red histogram
    drawHistogram(redHist, x, ofColor::red);
    // Draw Green histogram
    drawHistogram(greenHist, x + sectionWidth, ofColor::green);
    // Draw Blue histogram
    drawHistogram(blueHist, x + 2 * sectionWidth, ofColor::blue);

    ofSetColor(ofColor::white); // Reset color
}



void MediaElement::drawEdgeMap(int x, int y, int width, int height) const {
    
    if (edgeHist.empty()) return;
	int gridCols = this->edgeGridCols;
	int gridRows = this->edgeGridRows;
    int totalCells = gridRows * gridCols;
    if (edgeHist.size() != totalCells) return;

    // Determine max value for normalization
    float maxVal = *std::max_element(edgeHist.begin(), edgeHist.end());
    if (maxVal <= 0.0f) return;

    // Cell size based on provided drawing area
    float cellWidth = static_cast<float>(width) / gridCols;
    float cellHeight = static_cast<float>(height) / gridRows;
    ofEnableAlphaBlending();

    for (int row = 0; row < gridRows; ++row) {
        for (int col = 0; col < gridCols; ++col) {
            int index = row * gridCols + col;
            float normalizedValue = edgeHist[index] / maxVal;
            ofFill();
            // Map normalized value to grayscale brightness
            ofSetColor(255 * normalizedValue, 255 * normalizedValue, 255 * normalizedValue, 150); // Alpha blend

            float cellX = x + col * cellWidth;
            float cellY = y + row * cellHeight;
            ofDrawRectangle(cellX, cellY, cellWidth, cellHeight);
        }
    }

    // draw grid border
    ofNoFill();
    ofSetColor(100); // light gray
    ofDrawRectangle(x, y, width, height);
    ofFill();

    // Reset color
    ofSetColor(255);
    ofDisableAlphaBlending();

}

void MediaElement::drawLuminanceMap(int x, int y) const {
    ofEnableAlphaBlending(); // Enable transparency

    ofSetColor(255, 255, 255, 120); // White with ~50% transparency
    luminanceMap.draw(x, y);

    ofSetColor(255); // Reset to opaque white
    ofDisableAlphaBlending();
}
