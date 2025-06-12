

#include "MotionDetection.h"
#include "MediaElement.h"
#include "ofApp.h"

void MotionDetection::SetupMotionDetection() {
    ofSetFrameRate(60);
    cam.setup(camWidth, camHeight);
    colorImg.allocate(camWidth, camHeight);
    grayImg.allocate(camWidth, camHeight);
    prevGrayImg.allocate(camWidth, camHeight);
    diffImg.allocate(camWidth, camHeight);
}


void MotionDetection::UpdateMotionDetection(std::vector<std::vector<MediaElement*>>& mediaMatrix,
    int& selectedRow,
    int& selectedCol,
    int& currentMedia,
    std::vector<MediaElement>& medias) {
    cam.update();
    uint64_t now = ofGetElapsedTimeMillis();

    if (cam.isFrameNew()) {
        colorImg.setFromPixels(cam.getPixels());
        grayImg = colorImg;
        detectGesture(mediaMatrix, selectedRow, selectedCol, currentMedia, medias);
        prevGrayImg = grayImg;
    }

    // Set systemStatus based on cooldown and idle rotation logic
    if (movementCooldown > 0) {
        systemStatus = "Cooldown...";
        movementCooldown--;
    }
    else {
        if (now - lastInteractionTime > autoSwipeDelayMs) {
            if (now - lastAutoSwipeTime > autoSwipeIntervalMs) {
                movementCooldown = 30;
                lastAutoSwipeTime = now;
                systemStatus = "Auto Swipe";
                gestureStatus = "Idle movement";
                // Optional: auto-swipe logic here if desired
            }
            else {
                systemStatus = "Waiting...";
            }
        }
        else {
            systemStatus = "Waiting...";
        }
    }
}

void MotionDetection::selectCurrentMedia(MediaElement* selectedPtr, int& currentMedia, std::vector<MediaElement>& medias) {
    auto it = std::find_if(medias.begin(), medias.end(),
        [selectedPtr](MediaElement& m) { return &m == selectedPtr; });

    if (it != medias.end()) {
        currentMedia = std::distance(medias.begin(), it);
    }
}

void MotionDetection::navigateLeft(
    std::vector<std::vector<MediaElement*>>& mediaMatrix,
    int& selectedRow,
    int& selectedCol,
    int& currentMedia,
    std::vector<MediaElement>& medias)
{
    if (mediaMatrix.empty() || mediaMatrix[selectedRow].empty()) return;

    if (selectedCol > 0) {
        selectedCol--;
    }
    else {
        selectedCol = mediaMatrix[selectedRow].size() - 1;
    }

    MediaElement* selectedPtr = mediaMatrix[selectedRow][selectedCol];
    selectCurrentMedia(selectedPtr, currentMedia, medias);

    gestureStatus = "Swipe Left";
    movementCooldown = 60;
    lastInteractionTime = ofGetElapsedTimeMillis();
}

void MotionDetection::navigateRight(
    std::vector<std::vector<MediaElement*>>& mediaMatrix,
    int& selectedRow,
    int& selectedCol,
    int& currentMedia,
    std::vector<MediaElement>& medias)
{
    if (mediaMatrix.empty() || mediaMatrix[selectedRow].empty()) return;

    if (selectedCol + 1 < mediaMatrix[selectedRow].size()) {
        selectedCol++;
    }
    else {
        selectedCol = 0;
    }

    MediaElement* selectedPtr = mediaMatrix[selectedRow][selectedCol];
    selectCurrentMedia(selectedPtr, currentMedia, medias);

    gestureStatus = "Swipe Right";
    movementCooldown = 60;
    lastInteractionTime = ofGetElapsedTimeMillis();
}

void MotionDetection::detectGesture(
    std::vector<std::vector<MediaElement*>>& mediaMatrix,
    int& selectedRow,
    int& selectedCol,
    int& currentMedia,
    std::vector<MediaElement>& medias)
{
    diffImg.absDiff(grayImg, prevGrayImg);
    diffImg.threshold(30);

    int leftMovement = 0;
    int rightMovement = 0;
    int midX = camWidth / 2;
    unsigned char* pixels = diffImg.getPixels().getData();

    int yMin = camHeight * 0.3;
    int yMax = camHeight * 0.7;

    for (int y = yMin; y < yMax; y++) {
        for (int x = 0; x < camWidth; x++) {
            int index = y * camWidth + x;
            if (pixels[index] > 0) {
                if (x < midX) leftMovement++;
                else rightMovement++;
            }
        }
    }

    const int threshold = 1000;

    if (movementCooldown == 0) {
        if (leftMovement > threshold && rightMovement < threshold / 2) {
            navigateLeft(mediaMatrix, selectedRow, selectedCol, currentMedia, medias);
        }
        else if (rightMovement > threshold && leftMovement < threshold / 2) {
            navigateRight(mediaMatrix, selectedRow, selectedCol, currentMedia, medias);
        }
        else {
            gestureStatus = "None";
        }
    }
}

void MotionDetection::DrawDebugCameras() {
    // Debug camera views
    colorImg.draw(10, 10, 160, 120);
    diffImg.draw(180, 10, 160, 120);

    ofSetColor(255);
    ofDrawBitmapString("Gesture: " + gestureStatus, 10, 140);
    ofDrawBitmapString("Status: " + systemStatus, 10, 160);

    // Optional: visualize movement counts
    ofDrawBitmapString("Cooldown: " + ofToString(movementCooldown), 10, 180);
}
