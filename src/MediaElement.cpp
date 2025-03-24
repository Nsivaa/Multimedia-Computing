#include "MediaElement.h"

bool MediaElement::hasVideo() const {
    return this->videoPath.empty();
}

void MediaElement::generateThumbnail() {
    if (this->hasVideo()) {

        // opens the video pointed by the pointer and saves its first frame as an ofImage in the image field of the object
        try {
            ofVideoPlayer tempVideo;
            tempVideo.load(*this->videoPath);
            tempVideo.setFrame(0); // Move to the first frame
            tempVideo.update();
            if (tempVideo.isFrameNew()) {
                this->image.setFromPixels(tempVideo.getPixels());
                // this->image.resize(160, 90); // Resize to a standard thumbnail size
            }
        }
        catch (std::exception const& ex) { // catch every exception
            cout << "Error opening the video" << endl;
        }
    }
    else cout << "Not a video" << endl;
}

