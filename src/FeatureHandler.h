#pragma once
#include "utils.h"
#include "MediaElement.h"

class FeatureHandler
{
	public:
		FeatureHandler() {};
		void computeAllFeatures(MediaElement& element);
		void computeFeature(MediaElement& element, FeatureType feature);
		void groupByFeature(std::vector<MediaElement>& elements, FeatureType feature);
		void sortByFeature(std::vector<MediaElement>& elements, FeatureType feature);
		int compareFeatures(const MediaElement& element1, const MediaElement& element2, FeatureType feature);
		void generateThumbnail(MediaElement& element, int width = 300, int height = 300);

		// Feature extraction methods
		void computeNormalizedRGBHistogram(MediaElement& element);
		void computeEdgeMap(MediaElement& element);
		void computeDominantColor(MediaElement& element);
		void computeLuminanceMap(MediaElement& element);
		void computeAverageLuminance(MediaElement& element);

		void assignLuminanceGroup(MediaElement& element); // Assigns the luminance group based on the average luminance value

};

