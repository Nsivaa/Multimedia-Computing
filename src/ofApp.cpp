#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    dir.listDir("images/of_logos/"); // Media directory
    dir.allowExt("jpg");
    dir.allowExt("mp4");


    // Iterate through the files and load them appropriately

    for (int i = 0; i < dir.size(); i++) {
        string filePath = dir.getPath(i);
        string extension = ofToLower(ofFilePath::getFileExt(filePath));

        if (extension == "jpg") {
            ofImage img;
            img.load(filePath);
            medias.push_back(MediaElement(img));
        }
        else if (extension == "mp4") {

            medias.push_back(MediaElement(filePath));
        }
    }
	// Set the first media as selected
	if (medias.size() > 0) {
		medias[0].isSelected = true;
	}
    ofBackground(ofColor::black);
}
//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {

    if (dir.size() < 1) {
        ofDrawBitmapString("No media found in the directory.", 20, 20);
        return;
    }
	int x_pos = margin, y_pos = margin;

	// draw all images one next to another with a small margin
    for (auto& media : medias) {
        if (media.isSelected) {
            media.drawImageWithContour(x_pos, y_pos);
        }
        else {
            media.drawImage(x_pos, y_pos);
        }

        x_pos += media.image.getWidth() + margin;
        if (x_pos > ofGetWidth() - media.image.getWidth()) {
            x_pos = margin;
            y_pos += media.image.getHeight() + margin;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
	case(OF_KEY_RIGHT):
        this->medias[currentMedia].isSelected = false;
		currentMedia++;
		if (currentMedia >= medias.size()) {
			currentMedia = 0;
		}
        this->medias[currentMedia].isSelected = true;
		break;
	case(OF_KEY_LEFT):
        this->medias[currentMedia].isSelected = false;
        currentMedia--;

        if (currentMedia < 0) {
            currentMedia = medias.size() - 1;
        }
        this->medias[currentMedia].isSelected = true;
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
