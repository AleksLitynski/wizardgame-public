#pragma once

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <vector>
#include <optional>

struct WG_StringUtils {

    static std::string ltrim(std::string inStr) {
        std::string outStr = inStr;
        outStr.erase(outStr.begin(), std::find_if(outStr.begin(), outStr.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        return outStr;
    }

    static std::string rtrim(std::string inStr) {
        std::string outStr = inStr;
        outStr.erase(std::find_if(outStr.rbegin(), outStr.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), outStr.end());
        return outStr;
    }

    static std::string lowercase(std::string inStr) {
        std::string lcKey = inStr;
        std::transform(lcKey.begin(), lcKey.end(), lcKey.begin(), [](char c){ return std::tolower(c);});
        return lcKey;
    }
    static std::string uppercase(std::string inStr) {
        std::string lcKey = inStr;
        std::transform(lcKey.begin(), lcKey.end(), lcKey.begin(), [](char c){ return std::toupper(c);});
        return lcKey;
    }

    static std::string trim(std::string inStr) {
        return rtrim(ltrim(inStr));
    }

    static std::vector<std::string> split(std::string split, std::string inStr)
    {
        auto GetFirst = [](std::string split, std::string inStr) -> std::tuple<std::string, std::string> {
            int idx = inStr.find(split);
            int nextOffset = idx + split.length();
            return idx == std::string::npos
                ? std::make_tuple(inStr, "")
                : std::make_tuple(
                        inStr.substr(0, idx),
                        inStr.substr(nextOffset, inStr.length() - nextOffset)
                );
        };

        std::vector<std::string> matches;
        while (true)
        {
            auto [next, rest] = GetFirst(split, inStr);
            matches.push_back(next);
            if (rest == "") return matches;
            inStr = rest;
        }
    }
};
