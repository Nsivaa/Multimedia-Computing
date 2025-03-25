#include "MediaElement.h"

bool MediaElement::isVideo() const {
    return !(this->videoPath.empty());
}

void MediaElement::generateThumbnail() {
	if (!this->isVideo()) {
		return;
	}

    try {

        ofVideoPlayer tempVideo;
        tempVideo.load(this->videoPath);
        while (!tempVideo.isFrameNew()) {
			tempVideo.update(); // Wait for the first frame to be loaded
        }
            this->image.setFromPixels(tempVideo.getPixels());
            this->image.resize(300, 300); // Resize to image size
            // cout << "Thumbnail successfully created for: " << videoPath << endl;
    }
    catch (const std::exception& e) {
        cout << "Error generating thumbnail: " << e.what() << endl;
    }
}

