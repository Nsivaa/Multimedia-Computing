#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    dir.listDir("images/of_logos/"); // Media directory
    dir.allowExt("jpg");
    dir.allowExt("mp4");

    if (!videoIcon.load("video_icon.png")) {
        ofLogError() << "Failed to load video_icon.png";
    }

    ofSetVerticalSync(true);
    ofBackground(ofColor::black);

    FeatureHandler featureHandler; // Create a handler instance

    for (int i = 0; i < dir.size(); i++) {
        string filePath = dir.getPath(i);
        string extension = ofToLower(ofFilePath::getFileExt(filePath));
        MediaElement media;

        if (extension == "jpg") {
            ofImage img;
            img.load(filePath);
            media = MediaElement(img, standardImageSize.first, standardImageSize.second);
        }
        else if (extension == "mp4") {
            media = MediaElement(filePath);
        }

        else {
            continue; // Skip unsupported formats
        }

        // Extract all relevant features using the handler
        featureHandler.computeAllFeatures(media);

        medias.push_back(media); // Store processed media
    }
}


//--------------------------------------------------------------
void ofApp::update() {
	// if a video is playing, update it
    if (currentVideoPlaying && !currentVideoPlaying->isPaused) {
        currentVideoPlaying->videoPlayer.nextFrame();
		currentVideoPlaying->videoPlayer.update();
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

    if (dir.size() < 1) return;
    if (fullscreenMode){
        drawSelectedMediaFullscreen();
        return;
    }
    
    int x_pos = margin, y_pos = margin;

    for (auto& media : medias) {
        int drawX = x_pos;
        int drawY = y_pos - scrollOffsetY;
        // Auto-scroll to make selected media visible
        int selectedY = margin;
        int selectedRow = currentMedia / (ofGetWidth() / (standardImageSize.first + margin));
        selectedY += selectedRow * (standardImageSize.second + margin + 20);

        int viewHeight = ofGetHeight();

        // Ensure selected media is visible vertically
        if (selectedY - scrollOffsetY < 0) {
            scrollOffsetY = selectedY; // Scroll up
        }
        else if (selectedY - scrollOffsetY + standardImageSize.second > viewHeight) {
            scrollOffsetY = selectedY - (viewHeight - standardImageSize.second - margin);
		} // Scroll down

        // Draw image with optional dominant color contour
        if (&media == &medias[currentMedia]) {
            media.drawImageWithContour(drawX, drawY);
        }
        else if (showDominantColor) {
            media.drawImageWithContour(drawX, drawY, media.dominantColor);
        }
        else {
            media.drawImage(drawX, drawY);
        }
        
		if (media.isVideo()) { // Draw video icon if this is a video
            int iconX = drawX + media.image.getWidth() - iconSize - 5; // top-right corner
            int iconY = drawY + 5;
            ofSetColor(255); // Full white, fully opaque
            videoIcon.draw(iconX, iconY, iconSize, iconSize);
        }

        std::string colorString = getLuminanceGroupNames().at(media.luminanceGroup) +
            " luminance (" + std::to_string(media.averageLuminance) + ")";
        ofDrawBitmapString(colorString, drawX, drawY - 10);

        // Draw overlays using the same y base
        if (showEdgeHist) {
            media.drawEdgeMap(drawX, drawY, media.image.getWidth(), media.image.getHeight());
        }

        if (showLuminanceMap) {
            media.drawLuminanceMap(drawX, drawY);
        }

        if (showRGBHist) {
            int histW = 150;
            int histH = 400;
            int histX = drawX + 5; // 5 px padding from left edge of image
            int histY = drawY + media.image.getHeight() - histH - 5; // 5 px padding from bottom edge of image
            ofSetColor(255); // full opacity
            media.drawNormalizedRGBHistogram(histX, histY + histH, histW, histH);
        }

        // Move to next column or row
        x_pos += media.image.getWidth() + margin;
        if (x_pos > ofGetWidth() - media.image.getWidth()) {
            x_pos = margin;
            y_pos += media.image.getHeight() + margin + 20; // 100 to leave space for histogram
        }
    }

    if (showLegend) {
        drawLegend();
    }
    else {
        std::string hint = "Press 'h' to open legend";
        int x = 20;
        int y = ofGetHeight() - 20; // 20 px from bottom
        ofSetColor(255);            // White text
        ofDrawBitmapString(hint, x, y);
    }

}

void ofApp::drawSelectedMediaFullscreen() {

	// save the current screen size
	prevScreenSize.first = ofGetWidth();
	prevScreenSize.second = ofGetHeight();

    // Set fullscreen mode
    ofSetFullscreen(true);
    fullscreenMode = true;
    // Make sure we have media to show
    if (medias.empty()) return;

    // Get current screen size
    int screenW = ofGetWidth();
    int screenH = ofGetHeight();

    // Get the currently selected media
    MediaElement& selected = medias[currentMedia];

    // Make sure the image is allocated
    if (!selected.image.isAllocated()) return;

    int imgW = selected.image.getWidth();
    int imgH = selected.image.getHeight();

    // Calculate scale to fit the screen (keep aspect ratio)
    float scale = std::min((float)screenW / imgW, (float)screenH / imgH);
    int drawW = imgW * scale;
    int drawH = imgH * scale;

    // Center the image
    int x = (screenW - drawW) / 2;
    int y = (screenH - drawH) / 2;

    if (selected.isVideo()) { // playing the video
        if (selected.videoPlayer.isLoaded()) {
            selected.videoPlayer.update();
            selected.videoPlayer.draw(x, y, drawW, drawH);
        }
        else {
            selected.image.draw(x, y, drawW, drawH);
        }
        return;
    }
    // Draw the image
    selected.image.draw(x, y, drawW, drawH);
}

void ofApp::drawLegend() {
    std::vector<std::string> lines = {
        "Legend (press 'h' to hide):",
        "-> / <-       : Next / Previous media",
        "'f'           : Toggle fullscreen",
        "Spacebar      : Play/Pause video (only while in fullscreen mode)",
        "'e'           : Toggle edge histogram",
        "'c'           : Toggle dominant color contour",
        "'l'           : Toggle luminance map",
        "'r'           : Toggle RGB histogram",
        "'h'           : Toggle this legend"
    };

    int lineHeight = 20;
    int padding = 10;
    int legendHeight = lineHeight * lines.size() + padding * 2;
    int legendWidth = 400;
    int x = 20;
    int y = ofGetHeight() - legendHeight - 20;  // 20 px above bottom of screen

    // Draw semi-transparent dark background
    ofSetColor(0, 0, 0, 180);
    ofDrawRectangle(x, y, legendWidth, legendHeight);

    // Draw white text
    ofSetColor(255);
    int textY = y + padding + lineHeight;
    for (const auto& line : lines) {
        ofDrawBitmapString(line, x + padding, textY);
        textY += lineHeight;
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {

        case(OF_KEY_RIGHT):

            currentMedia++;
            if (currentMedia >= medias.size()) {
                currentMedia = 0;
            }
            if (currentVideoPlaying && currentVideoPlaying != &medias[currentMedia]) {
                currentVideoPlaying->videoPlayer.stop(); // Reset
                currentVideoPlaying = nullptr;
            }
            break;

        case(OF_KEY_LEFT):
            currentMedia--;

            if (currentMedia < 0) {
                currentMedia = medias.size() - 1;
            }
            if (currentVideoPlaying && currentVideoPlaying != &medias[currentMedia]) {
                currentVideoPlaying->videoPlayer.stop(); // Reset
                currentVideoPlaying = nullptr;
            }
            break;

        case(OF_KEY_UP):
		    // Select media up one row
            currentMedia -= (ofGetWidth() / (standardImageSize.first + margin));
            if (currentMedia < 0) {
                currentMedia = 0;
            }
            if (currentVideoPlaying && currentVideoPlaying != &medias[currentMedia]) {
                currentVideoPlaying->videoPlayer.stop(); // Reset
                currentVideoPlaying = nullptr;
            }
		    break;

	    case(OF_KEY_DOWN):
            // Select media down one row
            currentMedia += (ofGetWidth() / (standardImageSize.first + margin));
            if (currentMedia >= medias.size()) {
                currentMedia = medias.size() - 1;
		    }
            if (currentVideoPlaying && currentVideoPlaying != &medias[currentMedia]) {
                currentVideoPlaying->videoPlayer.stop(); // Reset
                currentVideoPlaying = nullptr;
            }
            break;

        case('f'): //toggles full screen 
            if (!fullscreenMode) {
                drawSelectedMediaFullscreen();
                fullscreenMode = true;
            }
            else {
                fullscreenMode = false;
                ofSetFullscreen(false);
                ofSetWindowShape(prevScreenSize.first, prevScreenSize.second);
            }
            break;

        case(' '): // Play/pause the video
            if (medias[currentMedia].isVideo()) {
                MediaElement& media = medias[currentMedia];

                if (!media.videoPlayer.isLoaded()) {
                    media.videoPlayer.setPixelFormat(OF_PIXELS_RGB);
                    media.videoPlayer.load(media.videoPath);
                    media.videoPlayer.setLoopState(OF_LOOP_NORMAL);
                    media.videoPlayer.play();
                    media.isPaused = false;
                }
                else {
                    if (media.isPaused) {
                        media.videoPlayer.setPaused(false);
                        media.isPaused = false;
                    }
                    else {
                        media.videoPlayer.setPaused(true);
                        media.isPaused = true;
                    }
                }

                currentVideoPlaying = &media;
            }
            break;

        case('e'): // show/hide edge histogram
            showEdgeHist = !showEdgeHist;
            break;

        case('c'): // show dominant color contour
            showDominantColor = !showDominantColor;
            break;

        case('l'): // show luminance map
            showLuminanceMap = !showLuminanceMap;
            break;

        case('r'): // show RGB histogram
            showRGBHist = !showRGBHist;
            break;

	    case('h'): // show/hide legend
            showLegend = !showLegend;
            break;
        }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::setMargin(int margin) {
	this->margin = margin;
}

void ofApp::setImageIndex(int index) {
}
