
#include "scenes/WG_SampleScenes.h"

#include <memory>
#include "spdlog/spdlog.h"
#include "WG_PropsMap.h"


void sampleParsing(std::shared_ptr<WG_Engine> engine)
{
    WG_PropsMap pm({
        {"str_1", "hello"}, {"str_2", ""},
        {"bool_1", "true"}, {"bool_2", "false"}, {"bool_3", "t"}, {"bool_4", "f"}, {"bool_5", ""},
        {"float_1", ""}, {"float_2", "1"}, {"float_3", "-1"}, {"float_4", "1.3"},
        {"vec2_1", ""}, {"vec2_2", "1"}, {"vec2_3", "1, 2"},
        {"vec3_1", ""}, {"vec3_2", "1"}, {"vec3_3", "1, 2"}, {"vec3_4", "1, 2, 3"},
        {"vec4_1", ""}, {"vec4_2", "1"}, {"vec4_3", "1, 2"}, {"vec4_4", "1, 2, 3"}, {"vec4_5", "1, 2, 3, 4"},
        {"color_1", ""}, {"color_2", "red"}, {"color_3", "blue"}, {"color_4", "1, 1, 1"}, {"color_5", "1, 1, 1, 1"},
    });

    spdlog::info("str_1: {}, str_2: {}", pm.ParseString("str_1"), pm.ParseString("str_2"));
    spdlog::info("bool_1: {}, bool_2: {}, bool_3: {}, bool_4: {}, bool_5: {}", pm.ParseBool("bool_1"), pm.ParseBool("bool_2"), pm.ParseBool("bool_3"), pm.ParseBool("bool_4"), pm.ParseBool("bool_5"));
    spdlog::info("float_1: {}, float_2: {}, float_3: {}, float_4: {}", pm.ParseFloat("float_1"), pm.ParseFloat("float_2"), pm.ParseFloat("float_3"), pm.ParseFloat("float_4"));
    spdlog::info("vec2_1: {}, vec2_2: {}, vec2_3: {}", glm::to_string(pm.ParseVec2("vec2_1")), glm::to_string(pm.ParseVec2("vec2_2")), glm::to_string(pm.ParseVec2("vec2_3")));
    spdlog::info("vec3_1: {}, vec3_2: {}, vec3_3: {}, vec3_4: {}", glm::to_string(pm.ParseVec3("vec3_1")), glm::to_string(pm.ParseVec3("vec3_2")), glm::to_string(pm.ParseVec3("vec3_3")), glm::to_string(pm.ParseVec3("vec3_4")));
    spdlog::info("vec4_1: {}, vec4_2: {}, vec4_3: {}, vec4_4: {}, vec4_5: {}", glm::to_string(pm.ParseVec4("vec4_1")), glm::to_string(pm.ParseVec4("vec4_2")), glm::to_string(pm.ParseVec4("vec4_3")), glm::to_string(pm.ParseVec4("vec4_4")), glm::to_string(pm.ParseVec4("vec4_5")));
    spdlog::info("color_1: {}, color_2: {}, color_3: {}, color_4: {}, color_5: {}", glm::to_string(pm.ParseColor("color_1")), glm::to_string(pm.ParseColor("color_2")), glm::to_string(pm.ParseColor("color_3")), glm::to_string(pm.ParseColor("color_4")), glm::to_string(pm.ParseColor("color_5")));
}
