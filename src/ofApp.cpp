#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	dir.listDir("images/of_logos/");
	dir.allowExt("jpg");
	dir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order

	//allocate the vector to have as many ofImages as files
	if (dir.size()) {
		images.assign(dir.size(), ofImage());
	}

	// you can now iterate through the files and load them into the ofImage vector
	for (int i = 0; i < (int)dir.size(); i++) {
		images[i].load(dir.getPath(i));
	}
	currentImageIndex = 0;

	ofBackground(ofColor::black);
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {

	if (dir.size() > 0) {
		ofSetColor(ofColor::white);
		int x_pos = margin, y_pos = margin;

		// draw all images one next to another with a small margin
		for (int i = 0; i < images.size(); i++) {
			images[i].draw(x_pos, y_pos);
			x_pos += images[i].getWidth() + margin;
			if (x_pos > ofGetWidth() - images[i].getWidth()) {
				x_pos = margin;
				y_pos += images[i].getHeight() + margin;
			}
		}
		
	}
	

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (dir.size() > 0) {
		currentImageIndex++;
		currentImageIndex %= dir.size();
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
	this->currentImageIndex = index;
}
