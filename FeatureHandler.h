#pragma once
#include "utils.h"
#include "MediaElement.h"

class FeatureHandler
{
	public:
		FeatureHandler() {};
		void extractFeature(MediaElement& element, FeatureType feature);
		void groupByFeature(std::vector<MediaElement>& elements, FeatureType feature);
		void sortByFeature(std::vector<MediaElement>& elements, FeatureType feature);
		int compareFeatures(const MediaElement& element1, const MediaElement& element2, FeatureType feature);

};

