#include "CameraHandler.h"

void CameraHandler::setup(int width, int height) {
    camWidth = width;
    camHeight = height;

    cam.setup(camWidth, camHeight);

    colorImg.allocate(camWidth, camHeight);
    grayImg.allocate(camWidth, camHeight);
    prevGrayImg.allocate(camWidth, camHeight);
    diffImg.allocate(camWidth, camHeight);
}

void CameraHandler::update() {
    cam.update();
    frameNew = cam.isFrameNew();

    if (frameNew) {
        colorImg.setFromPixels(cam.getPixels());
        grayImg = colorImg;
        diffImg.absDiff(prevGrayImg, grayImg);
        diffImg.threshold(40);
        prevGrayImg = grayImg;
    }
}

bool CameraHandler::isFrameNew() const {
    return frameNew;
}

const ofxCvGrayscaleImage& CameraHandler::getGrayImage() const {
    return grayImg;
}

const ofxCvGrayscaleImage& CameraHandler::getPrevGrayImage() const {
    return prevGrayImg;
}

const ofxCvGrayscaleImage& CameraHandler::getDiffImage() const {
    return diffImg;
}

int CameraHandler::getWidth() const {
    return camWidth;
}

int CameraHandler::getHeight() const {
    return camHeight;
}
