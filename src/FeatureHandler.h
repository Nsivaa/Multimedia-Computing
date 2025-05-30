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

		float histogramDistance(const MediaElement& el1, const MediaElement& el2) const {
			// Use Euclidean distance between concatenated RGB histograms
			float dist = 0.0f;
			for (size_t i = 0; i < el1.redHist.size(); i++) {
				float dr = el1.redHist[i] - el2.redHist[i];
				float dg = el1.greenHist[i] - el2.greenHist[i];
				float db = el1.blueHist[i] - el2.blueHist[i];
				dist += dr * dr + dg * dg + db * db;
			}
			return sqrt(dist);
		}

		// Feature extraction methods
		void computeNormalizedRGBHistogram(MediaElement& element);
		void computeEdgeMap(MediaElement& element);
		void computeDominantColor(MediaElement& element);
		void computeLuminanceMap(MediaElement& element);
		void computeAverageLuminance(MediaElement& element);
		void computeTextureDescriptor(MediaElement& element);
		void assignLuminanceGroup(MediaElement& element); // Assigns the luminance group based on the average luminance value
		void assignHueGroup(MediaElement& element); // Assigns the hue group based on the dominant color's hue value
		void assignTextureGroup(MediaElement& element);
};

