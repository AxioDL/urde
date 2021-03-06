add_library(imgui
        ../extern/imgui/imgui.cpp
        ../extern/imgui/imgui_demo.cpp
        ../extern/imgui/imgui_draw.cpp
        ../extern/imgui/imgui_tables.cpp
        ../extern/imgui/imgui_widgets.cpp
        ../extern/imgui/misc/cpp/imgui_stdlib.cpp
        ImGuiEngine.cpp
        ImGuiEngine.hpp
        NotoMono.cpp
        MetaforceIcon.cpp
        )
target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/extern/imgui ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(imgui PUBLIC IMGUI_USER_CONFIG="imconfig_user.h")
if (CMAKE_COMPILER_IS_GNUCXX)
    # currently explicitly ignored for clang in imgui code, but not gcc (yet)
    target_compile_options(imgui PRIVATE -Wno-deprecated-enum-enum-conversion)
endif()
target_link_libraries(imgui PRIVATE boo hecl-light RetroDataSpec)

bintoc_compress(NotoMono.cpp NotoMono-Regular.ttf NOTO_MONO_FONT)
bintoc(MetaforceIcon.cpp ../Runtime/platforms/freedesktop/256x256/apps/metaforce.png METAFORCE_ICON)

add_shader(ImGuiShader)
target_link_libraries(shader_ImGuiShader PRIVATE hecl-light)
