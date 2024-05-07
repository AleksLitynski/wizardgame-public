#pragma once
#include "glm/gtx/string_cast.hpp"

using WG_Color = glm::vec<4, uint8_t>;
#define COLOR(NAME, RED, GREEN, BLUE) constexpr static WG_Color NAME { RED, GREEN, BLUE, 0 }

struct WG_Colors {
    COLOR(red, 255, 0, 0);
    COLOR(green, 0, 255, 0);
    COLOR(blue, 0, 0, 255);
    COLOR(white, 255, 255, 255);
    COLOR(black, 0, 0, 0);
    COLOR(purple, 128, 0, 128);
    COLOR(grey, 128, 128, 128);

    COLOR(alice_blue, 240, 248, 255);
    COLOR(coral, 255, 127, 80);
    COLOR(fire_brick, 178, 34, 34);
    COLOR(hot_pink, 255, 105, 180);
    COLOR(lemon_chiffon, 255, 250, 205);

	COLOR(silver, 192, 192, 192);
	COLOR(maroon, 128, 0, 0);
	COLOR(yellow, 255, 255, 0);
	COLOR(olive, 128, 128, 0);
	COLOR(lime, 0, 128, 0);
	COLOR(aqua, 0, 255, 255);
	COLOR(teal, 0, 128, 128);
	COLOR(navy, 0, 0, 128);
	COLOR(fuchsia, 255, 0, 255);

    COLOR(orange, 255, 165, 0);

    static std::optional<WG_Color> ToColor(std::string cn) {
        if (cn == "red") return WG_Colors::red;
        if (cn == "green") return WG_Colors::green;
        if (cn == "blue") return WG_Colors::blue;
        if (cn == "white") return WG_Colors::white;
        if (cn == "black") return WG_Colors::black;
        if (cn == "purple") return WG_Colors::purple;
        if (cn == "grey") return WG_Colors::grey;
        if (cn == "alice_blue") return WG_Colors::alice_blue;
        if (cn == "coral") return WG_Colors::coral;
        if (cn == "fire_brick") return WG_Colors::fire_brick;
        if (cn == "hot_pink") return WG_Colors::hot_pink;
        if (cn == "lemon_chiffon") return WG_Colors::lemon_chiffon;
        if (cn == "silver") return WG_Colors::silver;
        if (cn == "maroon") return WG_Colors::maroon;
        if (cn == "yellow") return WG_Colors::yellow;
        if (cn == "olive") return WG_Colors::olive;
        if (cn == "lime") return WG_Colors::lime;
        if (cn == "aqua") return WG_Colors::aqua;
        if (cn == "teal") return WG_Colors::teal;
        if (cn == "navy") return WG_Colors::navy;
        if (cn == "fuchsia") return WG_Colors::fuchsia;
        if (cn == "orange") return WG_Colors::orange;
        return std::nullopt;
    }
};