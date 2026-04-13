#ifndef LIGHT_PROFILE_PROJECT_LAYOUTDATA_H
#define LIGHT_PROFILE_PROJECT_LAYOUTDATA_H

#pragma once

#include <string>
#include <vector>
#include "light/Layout.h"

class LayoutData {
public:
    bool loadFromFile(const std::string& path, std::string* error = nullptr);

    [[nodiscard]] LayoutView view() const {
        return {points_.data(), points_.size()};
    }

    [[nodiscard]] const std::string& name() const {
        return name_;
    }

    [[nodiscard]] const std::string& coordinateSpace() const {
        return coordinate_space_;
    }

    void clear() {
        name_.clear();
        coordinate_space_.clear();
        points_.clear();
    }

private:
    std::string name_;
    std::string coordinate_space_;
    std::vector<LedPoint> points_;
};

#endif //LIGHT_PROFILE_PROJECT_LAYOUTDATA_H