#include "FeatureHandler.h"

void FeatureHandler::extractFeature(MediaElement& element, FeatureType feature) {
    switch (feature) {
    case COLOR: element.computeDominantColor(); break;
    case TEXTURE: element.computeEdgeMap(); break;
    case LUMINANCE: element.computeLuminanceMap(); break;
    case RGBHISTOGRAM: element.computeNormalizedRGBHistogram(); break;
    case EDGE: element.computeEdgeMap(); break;
    default: break;
    }
}

void FeatureHandler::groupByFeature(std::vector<MediaElement>& elements, FeatureType feature) {
       
};
void FeatureHandler::sortByFeature(std::vector<MediaElement>& elements, FeatureType feature) {
    
};
int FeatureHandler::compareFeatures(const MediaElement& element1, const MediaElement& element2, FeatureType feature) {

};
