#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    dir.listDir("images/of_logos/"); // Media directory
    dir.allowExt("jpg");
    dir.allowExt("mp4");

    if (!videoIcon.load("video_icon.png")) {
        ofLogError() << "Failed to load video_icon.png";
    }

    ofSetFrameRate(60);
    camera.setup(640, 480);
    ofSetVerticalSync(true);
    ofBackground(ofColor::black);
    //ofEnableDepthTest();


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

    camera.update();

    if (camera.isFrameNew()) {
        detectGesture();
    }

    if (movementCooldown > 0) {
        movementCooldown--;
    }
	// if a video is playing, update it
    if (currentVideoPlaying && !currentVideoPlaying->isPaused) {
        currentVideoPlaying->videoPlayer.nextFrame();
		currentVideoPlaying->videoPlayer.update();
	}

}

//--------------------------------------------------------------
void ofApp::draw() {
    if (dir.size() < 1) return;

    if (fullscreenMode) {
        drawSelectedMediaFullscreen();
        return;
    }

    ofBackground(0);
    ofEnableDepthTest();

    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
    drawCarousel();
    ofPopMatrix();

    ofDisableDepthTest();

    if (showLegend) {
        drawLegend();
    }
    else {
        std::string hint = "Press 'h' to open legend";
        ofSetColor(255);
        ofDrawBitmapString(hint, 20, ofGetHeight() - 20);
    }
}


void ofApp::drawCarousel() {
    float radius = 600;
    float y_offset = 0;  // vertical center for carousel

    int mediaW = 200;
    int mediaH = 200;
    int iconSize = 20;
    int histW = 150;
    int histH = 100;

    for (int i = 0; i < medias.size(); i++) {
        auto& media = medias[i];
        float angle = ofDegToRad(carouselAngle + i * angleStep);
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        ofPushMatrix();
        ofTranslate(x, y_offset, z);
        ofRotateYDeg(ofRadToDeg(-angle));

        // Base image
        media.drawImage(-mediaW / 2, -mediaH / 2, mediaW, mediaH);

        // Highlight selected
        if (&media == &medias[currentMedia]) {
            media.drawImageWithContour(-mediaW / 2, -mediaH / 2, mediaW, mediaH);
        }

        // Video icon
        if (media.isVideo()) {
            videoIcon.draw(mediaW / 2 - iconSize, -mediaH / 2 + 5, iconSize, iconSize);
        }

        // Overlays
        if (showDominantColor) {
            std::string colorStr = getColorGroupNames().at(media.colorGroup) + " dominant color";
            ofSetColor(255);
            ofDrawBitmapString(colorStr, -mediaW / 2 + 5, mediaH / 2 + 15);
        }

        if (showLuminanceMap) {
            ofDisableDepthTest();
            media.drawLuminanceMap(-mediaW / 2, -mediaH / 2, mediaW, mediaH);
            ofEnableDepthTest();
            std::string lumStr = getLuminanceGroupNames().at(media.luminanceGroup) +
                " (" + std::to_string(media.averageLuminance) + ")";
            ofSetColor(255);
            ofDrawBitmapString(lumStr, -mediaW / 2 + 5, -mediaH / 2 - 10);
        }

        if (showEdgeHist) {
            ofDisableDepthTest();
            media.drawEdgeMap(-mediaW / 2, -mediaH / 2, mediaW, mediaH);
			ofEnableDepthTest();
        }

        if (showRGBHist) {
            ofDisableDepthTest();
            media.drawNormalizedRGBHistogram(-histW / 2, mediaH / 2 + 20, histW, histH);
			ofEnableDepthTest();
        }

        ofPopMatrix();
    }
}


void ofApp::detectGesture() {
    const auto& diffImg = camera.getDiffImage();
    int camWidth = camera.getWidth();
    int camHeight = camera.getHeight();

    int leftSum = 0;
    int rightSum = 0;

    int bandWidth = camWidth / 4;

    for (int y = 0; y < camHeight; y++) {
        for (int x = 0; x < camWidth; x++) {
            if (diffImg.getPixels()[y * camWidth + x] > 200) {
                if (x < bandWidth) leftSum++;
                else if (x > camWidth - bandWidth) rightSum++;
            }
        }
    }

    int threshold = 5000;

    if (movementCooldown == 0) {
        if (leftSum > threshold) {
            carouselAngle += angleStep;
            movementCooldown = 20;
        }
        else if (rightSum > threshold) {
            carouselAngle -= angleStep;
            movementCooldown = 20;
        }
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
