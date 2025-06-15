#include "ofApp.h"
#include "MotionDetection.h"

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

    motionDetection.SetupMotionDetection();

	updateMediaMatrix(); // Initialize media matrix

    for (int i = 0; i < dir.size(); i++) {
        string filePath = dir.getPath(i);
        string extension = ofToLower(ofFilePath::getFileExt(filePath));
        MediaElement media;

        if (extension == "jpg") {
            ofImage img;
            img.load(filePath);
            media = MediaElement(img, standardImageSize.first, standardImageSize.second, filePath);
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

    motionDetection.UpdateMotionDetection(mediaMatrix, selectedRow, selectedCol, currentMedia, medias);
    // if a video is playing, update it

    if (currentVideoPlaying && !currentVideoPlaying->isPaused) {
        currentVideoPlaying->videoPlayer.nextFrame();
        currentVideoPlaying->videoPlayer.update();
    }
    updateMediaMatrix();
}

//--------------------------------------------------------------
void ofApp::draw() {
    motionDetection.DrawDebugCameras();
    if (medias.empty()) return;
    if (fullscreenMode) {
        drawSelectedMediaFullscreen();
        return;
    }

    std::string groupingInfo;

    if (groupByLuminance) {
        auto luminanceNames = getLuminanceGroupNames();
        groupingInfo = "Luminance grouping active: rows represent ";
        int count = 0;
        for (auto& pair : luminanceNames) {
            groupingInfo += pair.second;
            if (++count < luminanceNames.size()) groupingInfo += ", ";
        }
    }
    else if (groupByColor) {
        auto colorNames = getColorGroupNames();
        groupingInfo = "Color grouping active: rows represent ";
        int count = 0;
        for (auto& pair : colorNames) {
            groupingInfo += pair.second;
            if (++count < colorNames.size()) groupingInfo += ", ";
        }
    }
    else if (groupByTexture) {
        auto textureNames = getTextureGroupNames();
        groupingInfo = "Texture grouping active: rows represent ";
        int count = 0;
        for (auto& pair : textureNames) {
            groupingInfo += pair.second;
            if (++count < textureNames.size()) groupingInfo += ", ";
        }
    }
    else {
        groupingInfo = "No grouping active: all media in one row";
    }

    ofSetColor(255);
    ofDrawBitmapStringHighlight(groupingInfo, 10, 20);  // Draw at the top-left corner

    bool groupingActive = groupByLuminance || groupByColor || groupByTexture;

    // Build grouped matrix
    mediaMatrix.clear();
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
        if (!groupedMedias[row].empty()) {
            mediaMatrix.push_back(groupedMedias[row]);
        }
    }

    int rowHeight = standardImageSize.second + margin;
    int baseY = (ofGetHeight() - mediaMatrix.size() * rowHeight) / 2;

    int viewWidth = ofGetWidth();

    // Get currently selected media pointer
    MediaElement* current = &medias[currentMedia];

    // === Auto-scroll to selected media ===
    for (int row = 0; row < mediaMatrix.size(); ++row) {
        for (int col = 0; col < mediaMatrix[row].size(); ++col) {
            if (mediaMatrix[row][col] == current) {
                selectedRow = row;
                selectedCol = col;
                int selectedX = margin + col * (standardImageSize.first + margin);
                if (selectedX - scrollOffsetX < 0) {
                    scrollOffsetX = selectedX;
                }
                else if (selectedX - scrollOffsetX + standardImageSize.first > viewWidth) {
                    scrollOffsetX = selectedX - (viewWidth - standardImageSize.first - margin);
                }
                break;
            }
        }
    }

    // === Draw all rows ===
    for (int row = 0; row < mediaMatrix.size(); ++row) {
        int x_pos = margin - scrollOffsetX;
        int y_pos = baseY + row * rowHeight;

        // === Row label ===
        std::string rowLabel = "";
        if (groupByLuminance) rowLabel = getLuminanceGroupNames().at(static_cast<LuminanceGroup>(row));
        else if (groupByColor) rowLabel = getColorGroupNames().at(static_cast<ColorGroup>(row));
        else if (groupByTexture) rowLabel = getTextureGroupNames().at(static_cast<TextureGroup>(row));
        else rowLabel = "All";

        ofSetColor(255);
        ofDrawBitmapString(rowLabel + " group", 10, y_pos + standardImageSize.second / 2);

        for (int col = 0; col < mediaMatrix[row].size(); ++col) {
            MediaElement* media = mediaMatrix[row][col];
            int drawX = margin + col * (standardImageSize.first + margin) - scrollOffsetX;
            int drawY = y_pos;

            if (drawX + standardImageSize.first < 0 || drawX > ofGetWidth()) continue;

            std::string luminanceString = getLuminanceGroupNames().at(media->luminanceGroup) +
                " luminance (" + std::to_string(media->averageLuminance) + ")";
            std::string colorString = getColorGroupNames().at(media->colorGroup) + " dominant color ";

            if (media == current) {
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
                media->drawEdgeMap(drawX, drawY, standardImageSize.first, standardImageSize.second);
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
    // Draw the currently selected media info box
    if (showInfoWindow && current != nullptr) {
        drawMediaXMLInfo(*current, ofGetWidth(), ofGetHeight());
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

void ofApp::updateMediaMatrix() {
    mediaMatrix.clear();
    mediaMatrix.resize(3);

    if (groupByLuminance) {
        for (auto& m : medias) mediaMatrix[m.luminanceGroup].push_back(&m);
    }
    else if (groupByColor) {
        for (auto& m : medias) mediaMatrix[m.colorGroup].push_back(&m);
    }
    else if (groupByTexture) {
        for (auto& m : medias) mediaMatrix[m.textureGroup].push_back(&m);
    }
    else {
        mediaMatrix[0] = {};
        for (auto& m : medias) mediaMatrix[0].push_back(&m);
    }

    // Find new selection position
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < mediaMatrix[row].size(); ++col) {
            if (mediaMatrix[row][col] == &medias[currentMedia]) {
                selectedRow = row;
                selectedCol = col;
                return;
            }
        }
    }
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
        "'i'           : Toggle media metadata (XML) info window",
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

void ofApp::drawMediaXMLInfo(const MediaElement& media, int screenW, int screenH) {
    // Adjusted position and size
    int margin = 20;
    int boxWidth = 400;
    int boxHeight = 300;
    int x = screenW - boxWidth - margin;
    int y = margin;

    ofPushStyle();

    // Background
    ofSetColor(0, 255); // semi-transparent black
    ofDrawRectangle(x, y, boxWidth, boxHeight);

    // Border
    ofNoFill();
    ofSetColor(255);
    ofDrawRectangle(x, y, boxWidth, boxHeight);
    ofFill();

    // Title
    float textX = x + 10;
    float textY = y + 20;
    float lineHeight = 14;
    ofSetColor(255);
    ofDrawBitmapString("Media Metadata (XML)", textX, textY);
    textY += lineHeight * 2;

    // Serialize XML to string
    ofxXmlSettings xml;
    media.saveToXML(xml, 0);
    std::string xmlStr;
    xml.copyXmlToString(xmlStr);

    // Draw each line, clipped to boxWidth
    std::istringstream iss(xmlStr);
    std::string line;
    int maxLineChars = (boxWidth - 20) / 7; // rough char fit estimate

    while (std::getline(iss, line)) {
        if (textY > y + boxHeight - 10) break;
        if (line.size() > maxLineChars) line = line.substr(0, maxLineChars - 3) + "...";
        ofDrawBitmapString(line, textX, textY);
        textY += lineHeight;
    }

    ofPopStyle();
}





//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {

    case OF_KEY_RIGHT: {
        // Move to the next media in the current row
        if (selectedCol + 1 < mediaMatrix[selectedRow].size()) {
            selectedCol++;
        }
        else {
            selectedCol = 0;
        }
        MediaElement* selectedPtr = mediaMatrix[selectedRow][selectedCol];
        currentMedia = std::distance(
            medias.begin(),
            std::find_if(medias.begin(), medias.end(),
                [selectedPtr](MediaElement& m) { return &m == selectedPtr; })
        );

        break;
    }

    case OF_KEY_LEFT: {
        // Move to the previous media in the current row
        if (selectedCol > 0) {
            selectedCol--;
        }
        else {
            selectedCol = mediaMatrix[selectedRow].size() - 1;
        }
        MediaElement* selectedPtr = mediaMatrix[selectedRow][selectedCol];
        currentMedia = std::distance(
            medias.begin(),
            std::find_if(medias.begin(), medias.end(),
                [selectedPtr](MediaElement& m) { return &m == selectedPtr; })
        );

        break;
    }

    case OF_KEY_UP: {
        // Move to the previous row, wrapping around if necessary
        int originalRow = selectedRow;
        int rowCount = mediaMatrix.size();

        // Move up
        do {
            selectedRow = (selectedRow - 1 + rowCount) % rowCount;
        } while (mediaMatrix[selectedRow].empty() && selectedRow != originalRow);

        if (selectedCol >= mediaMatrix[selectedRow].size()) {
            selectedCol = mediaMatrix[selectedRow].size() - 1;
        }

        MediaElement* selectedPtr = mediaMatrix[selectedRow][selectedCol];
        currentMedia = std::distance(
            medias.begin(),
            std::find_if(medias.begin(), medias.end(),
                [selectedPtr](MediaElement& m) { return &m == selectedPtr; })
        );

        break;
    }

    case OF_KEY_DOWN: {
        // Move to the next row, wrapping around if necessary
        int originalRow = selectedRow;
        int rowCount = mediaMatrix.size();

        // Move down
        do {
            selectedRow = (selectedRow + 1) % rowCount;
        } while (mediaMatrix[selectedRow].empty() && selectedRow != originalRow);

        if (selectedCol >= mediaMatrix[selectedRow].size()) {
            selectedCol = mediaMatrix[selectedRow].size() - 1;
        }

        MediaElement* selectedPtr = mediaMatrix[selectedRow][selectedCol];
        currentMedia = std::distance(
            medias.begin(),
            std::find_if(medias.begin(), medias.end(),
                [selectedPtr](MediaElement& m) { return &m == selectedPtr; })
        );

        break;
    }

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
        showEdgeHist = !showEdgeHist; break;

    case('c'): // show dominant color contour
        showDominantColor = !showDominantColor; break;

    case('l'): // show luminance map
        showLuminanceMap = !showLuminanceMap; break;

    case('r'): // show RGB histogram
        showRGBHist = !showRGBHist; break;

    case('h'): // show/hide legend
        showLegend = !showLegend; break;

    case('i'): // show xml metadata
        showInfoWindow = !showInfoWindow; break;

    case '1': groupByLuminance = !groupByLuminance;
        groupByColor = groupByTexture = false;
        updateMediaMatrix(); break;

    case '2': groupByColor = !groupByColor;
        groupByLuminance = groupByTexture = false;
        updateMediaMatrix(); break;

    case '3': groupByTexture = !groupByTexture;
        groupByLuminance = groupByColor = false;
        updateMediaMatrix(); break;
    }
}