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

enum FeatureType { RGBHISTOGRAM, COLOR, LUMINANCE, EDGE, TEXTURE };

enum ColorGroup { RED, GREEN, BLUE };

inline const std::map<ColorGroup, std::string>& getColorGroupNames() {
    static const std::map<ColorGroup, std::string> names = {
        { RED, "Red" },
        { GREEN, "Green" },
        { BLUE, "Blue" }
    };
    return names;
}

enum TextureGroup { SMOOTH_TEXTURE, MEDIUM_TEXTURE, COARSE_TEXTURE };

inline const std::map<TextureGroup, std::string>& getTextureGroupNames() {
    static const std::map<TextureGroup, std::string> names = {
        { SMOOTH_TEXTURE, "Smooth" },
        { MEDIUM_TEXTURE, "Medium" },
        { COARSE_TEXTURE, "Coarse" }
    };
    return names;
}
