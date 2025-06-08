#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class CameraHandler {
public:
    void setup(int width, int height);
    void update();
    bool isFrameNew() const;

    const ofxCvGrayscaleImage& getGrayImage() const;
    const ofxCvGrayscaleImage& getPrevGrayImage() const;
    const ofxCvGrayscaleImage& getDiffImage() const;

    int getWidth() const;
    int getHeight() const;

private:
    ofVideoGrabber cam;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImg;
    ofxCvGrayscaleImage prevGrayImg;
    ofxCvGrayscaleImage diffImg;

    int camWidth = 0;
    int camHeight = 0;
    bool frameNew = false;
};
