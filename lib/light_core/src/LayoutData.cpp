#include "light/LayoutData.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

bool getString(const json& obj, const char* key, std::string& out, std::string* error) {
    auto it = obj.find(key);
    if (it == obj.end()) {
        if (error) {
            *error = std::string("Missing required string field: ") + key;
        }
        return false;
    }
    if (!it->is_string()) {
        if (error) {
            *error = std::string("Field is not a string: ") + key;
        }
        return false;
    }
    out = it->get<std::string>();
    return true;
}

bool getNumber(const json& obj, const char* key, float& out, std::string* error) {
    auto it = obj.find(key);
    if (it == obj.end()) {
        if (error) {
            *error = std::string("Missing required numeric field: ") + key;
        }
        return false;
    }
    if (!it->is_number()) {
        if (error) {
            *error = std::string("Field is not numeric: ") + key;
        }
        return false;
    }
    out = it->get<float>();
    return true;
}

bool getUnsigned(const json& obj, const char* key, uint16_t& out, std::string* error) {
    auto it = obj.find(key);
    if (it == obj.end()) {
        if (error) {
            *error = std::string("Missing required integer field: ") + key;
        }
        return false;
    }
    if (!it->is_number_integer()) {
        if (error) {
            *error = std::string("Field is not an integer: ") + key;
        }
        return false;
    }

    const auto value = it->get<int64_t>();
    if (value < 0) {
        if (error) {
            *error = std::string("Field must be non-negative: ") + key;
        }
        return false;
    }

    out = static_cast<uint32_t>(value);
    return true;
}

} // namespace

bool LayoutData::loadFromFile(const std::string& path, std::string* error) {
    clear();

    std::ifstream file(path);
    if (!file.is_open()) {
        if (error) {
            *error = "Failed to open layout file: " + path;
        }
        return false;
    }

    json root;
    try {
        file >> root;
    } catch (const std::exception& e) {
        if (error) {
            *error = std::string("Failed to parse JSON: ") + e.what();
        }
        return false;
    }

    if (!root.is_object()) {
        if (error) {
            *error = "Layout JSON root must be an object";
        }
        return false;
    }

    if (!getString(root, "name", name_, error)) {
        return false;
    }

    if (!getString(root, "coordinate_space", coordinate_space_, error)) {
        return false;
    }

    if (coordinate_space_ != "normalized_2d") {
        if (error) {
            *error = "Unsupported coordinate_space: " + coordinate_space_;
        }
        return false;
    }

    auto ledsIt = root.find("leds");
    if (ledsIt == root.end()) {
        if (error) {
            *error = "Missing required array field: leds";
        }
        return false;
    }

    if (!ledsIt->is_array()) {
        if (error) {
            *error = "Field 'leds' must be an array";
        }
        return false;
    }

    const json& leds = *ledsIt;
    points_.reserve(leds.size());

    for (size_t i = 0; i < leds.size(); ++i) {
        const json& item = leds[i];

        if (!item.is_object()) {
            if (error) {
                *error = "Each entry in 'leds' must be an object";
            }
            clear();
            return false;
        }

        LedPoint point{};
        if (!getUnsigned(item, "index", point.index, error)) {
            if (error) {
                *error += " at leds[" + std::to_string(i) + "]";
            }
            clear();
            return false;
        }

        if (!getNumber(item, "u", point.u, error)) {
            if (error) {
                *error += " at leds[" + std::to_string(i) + "]";
            }
            clear();
            return false;
        }

        if (!getNumber(item, "v", point.v, error)) {
            if (error) {
                *error += " at leds[" + std::to_string(i) + "]";
            }
            clear();
            return false;
        }

        if (!getNumber(item, "led_radius", point.ledRadius, error)) {
            if (error) {
                *error += " at leds[" + std::to_string(i) + "]";
            }
            clear();
            return false;
        }

        points_.push_back(point);
    }

    std::sort(points_.begin(), points_.end(), [](const LedPoint& a, const LedPoint& b) {
        return a.index < b.index;
    });

    for (size_t i = 0; i < points_.size(); ++i) {
        if (points_[i].index != static_cast<uint32_t>(i)) {
            if (error) {
                *error = "LED indices must be contiguous starting at 0";
            }
            clear();
            return false;
        }
    }

    return true;
}