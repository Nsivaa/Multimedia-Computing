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
    if (medias.empty()) return;
    if (fullscreenMode) {
        drawSelectedMediaFullscreen();
        return;
    }
    bool groupingActive = groupByLuminance || groupByColor || groupByTexture;

    int rowHeight = standardImageSize.second + margin;
    int baseY = (ofGetHeight() - (groupingActive ? 3 * rowHeight : rowHeight)) / 2;

    // Group media if needed
    std::array<std::vector<MediaElement*>, 3> groupedMedias;

    if (groupByLuminance) {
        for (auto& media : medias) groupedMedias[media.luminanceGroup].push_back(&media);
    }
    else if (groupByColor) {
        for (auto& media : medias) groupedMedias[media.colorGroup].push_back(&media);
    }
    else if (groupByTexture) {
        for (auto& media : medias) groupedMedias[media.textureGroup].push_back(&media);
    }
    else {
        for (auto& media : medias) groupedMedias[0].push_back(&media);
    }

    for (int row = 0; row < 3; ++row) {
        if (groupedMedias[row].empty()) continue;

        int x_pos = margin - scrollOffsetX;
        int y_pos = baseY + row * rowHeight;

        // === Draw row label ===
        std::string rowLabel;
        if (groupByLuminance) {
            rowLabel = getLuminanceGroupNames().at(static_cast<LuminanceGroup>(row));
        }
        else if (groupByColor) {
            rowLabel = getColorGroupNames().at(static_cast<ColorGroup>(row));
        }
        else if (groupByTexture) {
            rowLabel = getTextureGroupNames().at(static_cast<TextureGroup>(row));
        }
        else {
            rowLabel = "All";
        }

        ofSetColor(255);
        ofDrawBitmapString(rowLabel + " group", 10, y_pos + standardImageSize.second / 2);

        // === Auto-scroll based on selected media ===
        int viewWidth = ofGetWidth();
        int selectedRow = 0;
        int selectedIndexInRow = 0;

        // Find the row and index of currentMedia in grouped layout
        for (int row = 0; row < 3; ++row) {
            for (int i = 0; i < groupedMedias[row].size(); ++i) {
                if (groupedMedias[row][i] == &medias[currentMedia]) {
                    selectedRow = row;
                    selectedIndexInRow = i;
                    break;
                }
            }
        }

        // Compute its X position in the row
        int selectedX = margin + selectedIndexInRow * (standardImageSize.first + margin);

        // Adjust scrollOffsetX to make it visible
        if (selectedX - scrollOffsetX < 0) {
            scrollOffsetX = selectedX;
        }
        else if (selectedX - scrollOffsetX + standardImageSize.first > viewWidth) {
            scrollOffsetX = selectedX - (viewWidth - standardImageSize.first - margin);
        }


        // === Draw each media in the row ===
        for (auto* media : groupedMedias[row]) {
            int drawX = x_pos;
            int drawY = y_pos;

            if (drawX + standardImageSize.first < 0 || drawX > ofGetWidth()) {
                x_pos += standardImageSize.first + margin;
                continue;
            }

            std::string luminanceString = getLuminanceGroupNames().at(media->luminanceGroup) +
                " luminance (" + std::to_string(media->averageLuminance) + ")";
            std::string colorString = getColorGroupNames().at(media->colorGroup) + " dominant color ";

            if (media == &medias[currentMedia]) {
                media->drawImageWithContour(drawX, drawY);
            }
            else if (showDominantColor) {
                media->drawImageWithContour(drawX, drawY, media->dominantColor);
            }
            else {
                media->drawImage(drawX, drawY);
            }

            if (showDominantColor) {
                ofDrawBitmapString(colorString, drawX + 5, drawY + media->image.getHeight() - 5);
            }

            if (media->isVideo()) {
                int iconX = drawX + media->image.getWidth() - iconSize - 5;
                int iconY = drawY + 5;
                ofSetColor(255);
                videoIcon.draw(iconX, iconY, iconSize, iconSize);
            }

            if (showEdgeHist) {
                media->drawEdgeMap(drawX, drawY, media->image.getWidth(), media->image.getHeight());
            }

            if (showLuminanceMap) {
                media->drawLuminanceMap(drawX, drawY);
                ofDrawBitmapString(luminanceString, drawX, drawY - 10);
            }

            if (showRGBHist) {
                int histW = 150;
                int histH = 400;
                int histX = drawX + 5;
                int histY = drawY + media->image.getHeight() - histH - 5;
                ofSetColor(255);
                media->drawNormalizedRGBHistogram(histX, histY + histH, histW, histH);
            }

            x_pos += media->image.getWidth() + margin;
        }
    }

    if (showLegend) {
        drawLegend();
    }
    else {
        std::string hint = "Press 'h' to open legend";
        int x = 20;
        int y = ofGetHeight() - 20;
        ofSetColor(255);
        ofDrawBitmapString(hint, x, y);
    }
}

void ofApp::drawSelectedMediaFullscreen() {

	// save the current screen size
	prevScreenSize.first = ofGetWidth();
	prevScreenSize.second = ofGetHeight();

    // Set fullscreen mode
    ofSetFullscreen(true);
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
        "'1'           : Group by luminance",
        "'2'           : Group by dominant color",
        "'3'           : Group by texture level",
        "'h'           : Toggle this legend"
    };

    int lineHeight = 20;
    int padding = 10;
    int legendHeight = lineHeight * lines.size() + padding * 2;
    int legendWidth = 420;
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


    case('f'):
        fullscreenMode = !fullscreenMode;
        ofSetFullscreen(fullscreenMode);

        if (!fullscreenMode) {
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

    case('1'): // group by luminance
        groupByLuminance = !groupByLuminance; // Toggle grouping 
        break;

    case('2'): // group by color
        groupByColor = !groupByColor; // Toggle grouping
        break;

    case('3'): // group by textures
        groupByTexture = !groupByTexture; // Toggle grouping
        break;
    }
}