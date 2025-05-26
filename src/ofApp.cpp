#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    dir.listDir("images/of_logos/"); // Media directory
    dir.allowExt("jpg");
    dir.allowExt("mp4");

    ofSetVerticalSync(true);

    // Iterate through the files and load them appropriately

    for (int i = 0; i < dir.size(); i++) {
        string filePath = dir.getPath(i);
        string extension = ofToLower(ofFilePath::getFileExt(filePath));

        if (extension == "jpg") {
            ofImage img;
            img.load(filePath);
			auto img_media = MediaElement(img, standardImageSize.first, standardImageSize.second);
			img_media.computeNormalizedRGBHistogram();
			img_media.computeDominantColor();
			img_media.computeEdgeMap();
            medias.push_back(img_media);
        }
        else if (extension == "mp4") {
			auto vid_media = MediaElement(filePath);
            vid_media.computeNormalizedRGBHistogram();
            vid_media.computeEdgeMap();
			vid_media.computeDominantColor();
            medias.push_back(vid_media);
        }
    }

    ofBackground(ofColor::black);
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

    // draw all images one next to another with a small margin
    for (auto& media : medias) {
        // Check if the media is selected
        
        if (&media == &medias[currentMedia]) {
            media.drawImageWithContour(x_pos, y_pos);
        }
        else if (showDominantColor) {
			media.drawImageWithContour(x_pos, y_pos, media.dominantColor); // Draw with dominant color contour
        }
        else {
            media.drawImage(x_pos, y_pos);
		}
        if (showEdgeHist) {
            // media.drawNormalizedRGBHistogram(x_pos, y_pos + media.image.getHeight(), 100, 2000);
            media.drawEdgeMap(x_pos, y_pos, media.image.getWidth(), media.image.getHeight());
        }
        x_pos += media.image.getWidth() + margin;
        if (x_pos > ofGetWidth() - media.image.getWidth()) {
            x_pos = margin;
            y_pos += media.image.getHeight() + margin;
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

    if (selected.isVideo()) {
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

    case('h'): // show/hide edge histogram
        showEdgeHist = !showEdgeHist;
        break;

    case('c'): // show dominant color contour
        showDominantColor = !showDominantColor;
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
