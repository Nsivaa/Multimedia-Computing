#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "MediaElement.h"

class MotionDetection {

public:
	MotionDetection() {};
	void SetupMotionDetection();
	void UpdateMotionDetection(
		std::vector<std::vector<MediaElement*>>& mediaMatrix,
		int& selectedRow,
		int& selectedCol,
		int& currentMedia,
		std::vector<MediaElement>& medias);
	void DrawDebugCameras();

	void navigateLeft(std::vector<std::vector<MediaElement*>>& mediaMatrix, int& selectedRow, int& selectedCol, int& currentMedia, std::vector<MediaElement>& medias);
	void navigateRight(std::vector<std::vector<MediaElement*>>& mediaMatrix, int& selectedRow, int& selectedCol, int& currentMedia, std::vector<MediaElement>& medias);
	void detectGesture(std::vector<std::vector<MediaElement*>>& mediaMatrix, int& selectedRow, int& selectedCol, int& currentMedia, std::vector<MediaElement>& medias);
	void selectCurrentMedia(MediaElement* selectedPtr, int& currentMedia, std::vector<MediaElement>& medias);
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	void mouseMoved(int x, int y);

	ofVideoGrabber cam;
	ofxCvColorImage colorImg;
	ofxCvGrayscaleImage grayImg;
	ofxCvGrayscaleImage prevGrayImg;
	ofxCvGrayscaleImage diffImg;
	ofSoundPlayer swipeSound;

	//ofEasyCam easyCam; // allows mouse control and camera movement 3D


	std::string gestureStatus = "None";
	std::string systemStatus = "Waiting...";

	int autoSwipeDelayMs = 20000; // 20 seconds of inactivity before auto swipe
	int autoSwipeIntervalMs = 3000; // every 3 seconds after delay, auto swipe once
	uint64_t lastAutoSwipeTime = 0;
	uint64_t lastInteractionTime = 0;


	int camWidth = 640;
	int camHeight = 480;

	int movementCooldown = 0;
};