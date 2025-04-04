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
    
    currentMedia = 0;

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
        for (int i = 0; i < medias.size(); i++) {
            medias[i].image.draw(x_pos, y_pos);
            x_pos += medias[i].image.getWidth() + margin;
            if (x_pos > ofGetWidth() - medias[i].image.getWidth()) {
                x_pos = margin;
                y_pos += medias[i].image.getHeight() + margin;
            }
        }
	}
	

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

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
