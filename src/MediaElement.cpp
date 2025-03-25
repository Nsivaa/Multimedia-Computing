#include "MediaElement.h"

bool MediaElement::isVideo() const {
    return !(this->videoPath.empty());
}

void MediaElement::generateThumbnail() {
    try {
        if (this->isVideo()) {

            ofVideoPlayer tempVideo;
            tempVideo.load(this->videoPath);
            tempVideo.update(); // Update to make sure frame is loaded

            for (int i = 0; i < 10; i++) {
                tempVideo.update();
                ofSleepMillis(10); // Small delay to allow decoding
            }

            if (tempVideo.isLoaded() && tempVideo.getPixels().isAllocated()) {
                this->image.allocate(tempVideo.getWidth(), tempVideo.getHeight(), OF_IMAGE_COLOR);
                this->image.setFromPixels(tempVideo.getPixels());
                this->image.resize(300, 300); // Resize to image size

                // cout << "Thumbnail successfully created for: " << videoPath << endl;
            }
            else {
                cout << "Failed to load first frame from video: " << videoPath << endl;
            }
        }
    }
	catch (const std::exception& e) {
		cout << "Error generating thumbnail: " << e.what() << endl;
	}
}

