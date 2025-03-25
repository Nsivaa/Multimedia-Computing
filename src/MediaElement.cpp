#include "MediaElement.h"

bool MediaElement::isVideo() const {
    return !(this->videoPath.empty());
}

void MediaElement::generateThumbnail() {
    try {

        if (this->isVideo()) {
            ofVideoPlayer tempVideo;
            tempVideo.load(this->videoPath);
            tempVideo.update(); 
            ofSleepMillis(30); // Small delay to allow decoding. Garbage but otherwise won't work
            tempVideo.update();

            if (tempVideo.isLoaded() && tempVideo.getPixels().isAllocated()) {
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

