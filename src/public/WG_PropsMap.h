#pragma once

#include <map>
#include <string>
#include "WG_StringUtils.h"
#include "glm/vec3.hpp"
#include "WG_Colors.h"
#include "spdlog/spdlog.h"

struct WG_PropsMap {
    std::map<std::string, std::string> Props;

    WG_PropsMap(std::map<std::string, std::string> props) {
        for (auto prop : props) {
            Props[WG_StringUtils::trim(WG_StringUtils::lowercase(prop.first))] = prop.second;
        }
    }

    bool Contains(const std::string& key)
    {
        return Props.contains(key);
    }

    std::string ParseString(const std::string& key)
    {
        return WG_StringUtils::trim(Props[key]);
    }
    void WithString(const std::string& key, std::function<void(std::string)> onItem)
    {
        if (Contains(key)) onItem(ParseString(key));
    }

    bool ParseBool(const std::string& key)
    {
        auto inStr = Props[key];
        auto cleaned = WG_StringUtils::trim(WG_StringUtils::lowercase(inStr));
        return cleaned == "true" || cleaned == "t";
    }
    void WithBool(const std::string& key, std::function<void(bool)> onItem)
    {
        if (Contains(key)) onItem(ParseBool(key));
    }

    glm::vec2 ParseVec2(const std::string& key)
    {
        auto inStr = Props[key];
        std::vector<std::string> parts = WG_StringUtils::split(",", inStr);
        if (parts.size() == 1) return { ParsePassedFloat(parts[0]), 0 };
        if (parts.size() == 2) return { ParsePassedFloat(parts[0]), ParsePassedFloat(parts[1]) };
        return { 0, 0 };
    }
    void WithVec2(const std::string& key, std::function<void(glm::vec2)> onItem)
    {
        if (Contains(key)) onItem(ParseVec2(key));
    }

    glm::vec3 ParseVec3(const std::string& key)
    {
        auto inStr = Props[key];
        std::vector<std::string> parts = WG_StringUtils::split(",", inStr);
        if (parts.size() == 1) return { ParsePassedFloat(parts[0]), 0, 0 };
        if (parts.size() == 2) return { ParsePassedFloat(parts[0]), ParsePassedFloat(parts[1]), 0 };
        if (parts.size() == 3) return { ParsePassedFloat(parts[0]), ParsePassedFloat(parts[1]), ParsePassedFloat(parts[2]) };
        return { 0, 0, 0 };
    }
    void WithVec3(const std::string& key, std::function<void(glm::vec3)> onItem)
    {
        if (Contains(key)) onItem(ParseVec3(key));
    }

    glm::vec4 ParseVec4(const std::string& key)
    {
        auto inStr = Props[key];
        std::vector<std::string> parts = WG_StringUtils::split(",", inStr);
        if (parts.size() == 1) return { ParsePassedFloat(parts[0]), 0, 0, 0 };
        if (parts.size() == 2) return { ParsePassedFloat(parts[0]), ParsePassedFloat(parts[1]), 0, 0 };
        if (parts.size() == 3) return { ParsePassedFloat(parts[0]), ParsePassedFloat(parts[1]), ParsePassedFloat(parts[2]), 0 };
        if (parts.size() == 4) return { ParsePassedFloat(parts[0]), ParsePassedFloat(parts[1]), ParsePassedFloat(parts[2]), ParsePassedFloat(parts[3]) };
        return { 0, 0, 0, 0 };
    }
    void WithVec4(const std::string& key, std::function<void(glm::vec4)> onItem)
    {
        if (Contains(key)) onItem(ParseVec4(key));
    }

    float ParsePassedFloat(const std::string& floatStr)
    {
        if (floatStr.empty()) return 0;
        return std::stof(WG_StringUtils::trim(WG_StringUtils::lowercase(floatStr)));
    }

    float ParseFloat(const std::string& key)
    {
        auto inStr = Props[key];
        return ParsePassedFloat(inStr);
    }
    void WithFloat(const std::string& key, std::function<void(float)> onItem)
    {
        if (Contains(key)) onItem(ParseFloat(key));
    }

    // either a color string "blue" or "1, 2, 3" (rgb) or "1, 2, 3, 4" (rgba)
    WG_Color ParseColor(const std::string& key)
    {
        auto inStr = Props[key];
        auto cleaned = WG_StringUtils::trim(WG_StringUtils::lowercase(inStr));

        auto staticColor = WG_Colors::ToColor(cleaned);
        if (staticColor.has_value()) return staticColor.value();

        std::vector<std::string> parts = WG_StringUtils::split(",", cleaned);
        if (parts.size() == 1) return { ParsePassedFloat(parts[0]), 0, 0, 0 };
        if (parts.size() == 2) return { ParsePassedFloat(parts[0]), ParsePassedFloat(parts[1]), 0, 0 };
        if (parts.size() == 3) return { ParsePassedFloat(parts[0]), ParsePassedFloat(parts[1]), ParsePassedFloat(parts[2]), 0 };
        if (parts.size() == 4) return { ParsePassedFloat(parts[0]), ParsePassedFloat(parts[1]), ParsePassedFloat(parts[2]), ParsePassedFloat(parts[3]) };
        return { 0, 0, 0, 0 };
    }
    void WithColor(const std::string& key, std::function<void(WG_Color)> onItem)
    {
        if (Contains(key)) onItem(ParseColor(key));
    }
};
