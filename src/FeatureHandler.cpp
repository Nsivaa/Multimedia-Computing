#pragma once
#include "FeatureHandler.h"
#include "ofxOpenCv.h"
#include <opencv2/opencv.hpp>
#include <algorithm>

void FeatureHandler::computeAllFeatures(MediaElement& element) {
    if (element.isVideo()) {
        generateThumbnail(element, 300, 300);
	}
    if (!element.image.isAllocated()) return;
    computeNormalizedRGBHistogram(element);
    computeEdgeMap(element);
    computeDominantColor(element);
    computeLuminanceMap(element);
    computeAverageLuminance(element);
    computeTextureDescriptor(element);
	assignLuminanceGroup(element);
    assignHueGroup(element);
}


void FeatureHandler::generateThumbnail(MediaElement& element, int width, int height) {
    if (!element.isVideo()) return;
    try {
        ofVideoPlayer tempVideo;
        tempVideo.load(element.videoPath);
        while (!tempVideo.isFrameNew()) {
            tempVideo.update();
        }
        element.image.setFromPixels(tempVideo.getPixels());
        element.image.resize(width, height);
    }
    catch (const std::exception& e) {
        std::cout << "Error generating thumbnail: " << e.what() << std::endl;
    }
}

void FeatureHandler::computeNormalizedRGBHistogram(MediaElement& element) {
    ofPixels pixels = element.image.getPixels();
    int numPixels = pixels.size() / 3;
    const int numBins = 256;
    element.redHist.assign(numBins, 0.0f);
    element.greenHist.assign(numBins, 0.0f);
    element.blueHist.assign(numBins, 0.0f);

    for (int y = 0; y < element.image.getHeight(); y++) {
        for (int x = 0; x < element.image.getWidth(); x++) {
            ofColor c = element.image.getColor(x, y);
            element.redHist[c.r]++;
            element.greenHist[c.g]++;
            element.blueHist[c.b]++;
        }
    }

    for (int i = 0; i < numBins; i++) {
        element.redHist[i] /= numPixels;
        element.greenHist[i] /= numPixels;
        element.blueHist[i] /= numPixels;
    }
}

void FeatureHandler::computeEdgeMap(MediaElement& element) {
    int gridX = element.edgeGridCols;
    int gridY = element.edgeGridRows;
    std::vector<float> histogram(gridX * gridY, 0.0f);
    ofImage img = element.image;

    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImg, edgeImg;
    colorImg.allocate(img.getWidth(), img.getHeight());
    grayImg.allocate(img.getWidth(), img.getHeight());
    edgeImg.allocate(img.getWidth(), img.getHeight());

    colorImg.setFromPixels(img.getPixels());
    grayImg = colorImg;

    cv::Mat grayMat = cv::cvarrToMat(grayImg.getCvImage());
    cv::Mat edges;
    cv::Canny(grayMat, edges, 50, 150);

    edgeImg.setFromPixels(edges.data, edges.cols, edges.rows);

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

    float maxCount = *std::max_element(histogram.begin(), histogram.end());
    if (maxCount > 0) {
        for (auto& h : histogram) {
            h /= maxCount;
        }
    }

    element.edgeHist = histogram;
}

void FeatureHandler::computeDominantColor(MediaElement& element) {
    if (!element.image.isAllocated()) return;
    ofPixels& pixels = element.image.getPixels();
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
        element.dominantColor = ofColor(r, g, b);
    }
}

void FeatureHandler::computeLuminanceMap(MediaElement& element) {
    ofPixels& pixels = element.image.getPixels();
    int w = pixels.getWidth();
    int h = pixels.getHeight();

    ofPixels heatmapPixels;
    heatmapPixels.allocate(w, h, OF_PIXELS_RGB);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            ofColor color = pixels.getColor(x, y);
            float luminance = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
            float normLum = ofClamp(luminance / 255.0f, 0.0f, 1.0f);
            ofColor heatColor = element.getHeatmapColor(normLum);
            heatmapPixels.setColor(x, y, heatColor);
        }
    }

    element.luminanceMap.setFromPixels(heatmapPixels);
}

void FeatureHandler::computeAverageLuminance(MediaElement& element) {
    ofPixels& pixels = element.image.getPixels();
    float totalLuminance = 0;
    for (int y = 0; y < pixels.getHeight(); y++) {
        for (int x = 0; x < pixels.getWidth(); x++) {
            ofColor color = pixels.getColor(x, y);
            float luminance = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
            totalLuminance += luminance;
        }
    }
    element.averageLuminance = totalLuminance / (pixels.getWidth() * pixels.getHeight());
}

void FeatureHandler::computeTextureDescriptor(MediaElement& element) {
    if (!element.image.isAllocated()) return;

    // Convert to grayscale
    ofxCvGrayscaleImage grayImg;
    grayImg.allocate(element.image.getWidth(), element.image.getHeight());
    ofxCvColorImage colorImg;
    colorImg.allocate(element.image.getWidth(), element.image.getHeight());
    colorImg.setFromPixels(element.image.getPixels());
    grayImg = colorImg;

    // Convert to OpenCV Mat
    cv::Mat grayMat = cv::cvarrToMat(grayImg.getCvImage());

    // Apply Laplacian
    cv::Mat laplacian;
    cv::Laplacian(grayMat, laplacian, CV_64F);

    // Compute variance of the Laplacian (texture strength)
    cv::Scalar mean, stddev;
    cv::meanStdDev(laplacian, mean, stddev);
    element.textureVariance = stddev.val[0] * stddev.val[0]; // variance = stddev^2
}


void FeatureHandler::assignLuminanceGroup(MediaElement& element) {
    computeAverageLuminance(element);
    if (element.averageLuminance < 85) {
        element.luminanceGroup = LOW;
    }
    else if (element.averageLuminance < 170) {
        element.luminanceGroup = LuminanceGroup::MEDIUM;
    }
    else {
        element.luminanceGroup = HIGH;
    }
}

void FeatureHandler::assignHueGroup(MediaElement& element) {
    float hue = element.dominantColor.getHueAngle(); // 0–360

    if (hue < 60 || hue >= 300) {
        element.colorGroup = RED; // Red-ish
    }
    else if (hue < 180) {
        element.colorGroup = GREEN; // Green-ish
    }
    else {
        element.colorGroup = BLUE; // Blue-ish
    }
   
}

void FeatureHandler::assignTextureGroup(MediaElement& element) {
    float v = element.textureVariance;
    if (v < 100.0f) {
        element.textureGroup = SMOOTH_TEXTURE;
    }
    else if (v < 500.0f) {
        element.textureGroup = MEDIUM_TEXTURE;
    }
    else {
        element.textureGroup = COARSE_TEXTURE;
    }
}

