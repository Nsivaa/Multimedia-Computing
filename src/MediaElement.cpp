#include "MediaElement.h"



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


// -------------------------------------------------------------------------------------------------------------------------
// DRAWER METHODS 
// -------------------------------------------------------------------------------------------------------------------------

void MediaElement::drawImageWithContour(int x, int y, ofColor contourColor, int thickness) const {
    // Draw the image itself
    this->image.draw(x, y);

    // Set the color for the contour
    ofSetColor(contourColor);
    ofNoFill(); // No fill for the contourójust an outline

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


void MediaElement::saveToXML(ofxXmlSettings& xml, int index) const {
    std::string tag = "media";
    xml.addTag(tag);
    xml.pushTag(tag, index);

    xml.addValue("filePath", filePath);
    xml.addValue("videoPath", videoPath);
    xml.addValue("isVideoFlag", isVideoFlag);
    xml.addValue("isPaused", isPaused);

    xml.addValue("averageLuminance", averageLuminance);
    xml.addValue("rhythmScore", rhythmMetric); // Updated from rhythmScore
    xml.addValue("textureVariance", textureVariance);

    xml.addValue("luminanceGroup", int(luminanceGroup));
    xml.addValue("colorGroup", int(colorGroup));
    xml.addValue("textureGroup", int(textureGroup));
    xml.addValue("rhythmGroup", int(rhythmGroup));

    xml.addValue("dominantColorR", dominantColor.r);
    xml.addValue("dominantColorG", dominantColor.g);
    xml.addValue("dominantColorB", dominantColor.b);


    xml.popTag(); // media
}

void MediaElement::loadFromXML(ofxXmlSettings& xml, int index) {
    std::string tag = "media";
    if (!xml.tagExists(tag)) return;

    xml.pushTag(tag, index);

    filePath = xml.getValue("filePath", "");
    videoPath = xml.getValue("videoPath", "");
    isVideoFlag = xml.getValue("isVideoFlag", false);
    isPaused = xml.getValue("isPaused", false);

    averageLuminance = xml.getValue("averageLuminance", 0.0f);
    rhythmMetric = xml.getValue("rhythmScore", 0.0f);
    textureVariance = xml.getValue("textureVariance", 0.0f);

    luminanceGroup = static_cast<LuminanceGroup>(xml.getValue("luminanceGroup", 0));
    colorGroup = static_cast<ColorGroup>(xml.getValue("colorGroup", 0));
    textureGroup = static_cast<TextureGroup>(xml.getValue("textureGroup", 0));
    rhythmGroup = static_cast<RhythmGroup>(xml.getValue("rhythmGroup", 0));

    dominantColor.r = xml.getValue("dominantColorR", 0);
    dominantColor.g = xml.getValue("dominantColorG", 0);
    dominantColor.b = xml.getValue("dominantColorB", 0);

    xml.popTag(); // media
}
