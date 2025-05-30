#pragma once

#include <map>
#include <string>

enum LuminanceGroup { LOW, MEDIUM, HIGH };

inline const std::map<LuminanceGroup, std::string>& getLuminanceGroupNames() {
    static const std::map<LuminanceGroup, std::string> names = {
        { LOW, "Low" },
        { MEDIUM, "Medium" },
        { HIGH, "High" }
    };
    return names;
}

enum FeatureType {
    COLOR, TEXTURE, LUMINANCE, RGBHISTOGRAM, EDGE
};

