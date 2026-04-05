#pragma once
#include <string>
#include "light/LightProfile.h"

namespace light {

class LightProfileLoader {
public:
    static bool loadFromJsonText(const std::string& jsonText, LightProfile& outProfile, std::string& outError);
    static bool loadFromFile(const std::string& path, LightProfile& outProfile, std::string& outError);
};

}
