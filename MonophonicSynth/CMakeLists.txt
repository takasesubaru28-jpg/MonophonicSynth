cmake_minimum_required(VERSION 3.15)
set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "Windows SDK version" FORCE)
project(MonophonicSynth VERSION 1.0.0)

# --- C++17譛牙柑蛹・(VSTGUI4縺ｫ蠢・・ ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- 1. 繝代せ險ｭ螳・---
# 隕ｪ繝・ぅ繝ｬ繧ｯ繝医Μ縺ｫ縺ゅｋ vst3sdk 繧貞盾辣ｧ
get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
set(VST3_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vst3sdk")

# --- 2. 繧ｿ繝ｼ繧ｲ繝・ヨ菴懈・ (DLL縺ｨ縺励※螳夂ｾｩ) ---
add_library(MonophonicSynth MODULE
    source/contour.h
    source/controller.cpp
    source/controller.h
    source/factory.cpp
    source/filter.h
    source/fuid.h
    source/noise.h
    source/oscillator.h
    source/output.h
    source/parameter.h
    source/processor.cpp
    source/processor.h
    "${VST3_SDK_ROOT}/public.sdk/source/main/dllmain.cpp"
)

# --- 3. 繧､繝ｳ繧ｯ繝ｫ繝ｼ繝峨ョ繧｣繝ｬ繧ｯ繝医Μ縺ｮ險ｭ螳・---
target_include_directories(MonophonicSynth PRIVATE 
    source
    "${VST3_SDK_ROOT}"
    "${VST3_SDK_ROOT}/vstgui4"
)

# --- 4. 繝励Μ繝励Ο繧ｻ繝・し縺ｮ螳夂ｾｩ ---
target_compile_definitions(MonophonicSynth PRIVATE 
    VSTGUI_LIVE_EDITING=1
    DEVELOPMENT=1
    WIN32=1
    _WINDOWS=1
)

# --- 5. 繝ｪ繝ｳ繧ｫ繝ｼ險ｭ螳・(SDK縺ｮ繝薙Ν繝画ｸ医∩.lib繧堤峩謗･繝ｪ繝ｳ繧ｯ) ---
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(VST3_LIB_DIR "${VST3_SDK_ROOT}/out/build/x64-Debug/lib")
else()
    set(VST3_LIB_DIR "${VST3_SDK_ROOT}/out/build/x64-Release/lib")
endif()

target_link_libraries(MonophonicSynth PRIVATE 
    "${VST3_LIB_DIR}/base.lib"
    "${VST3_LIB_DIR}/sdk.lib"
    "${VST3_LIB_DIR}/sdk_common.lib"
    "${VST3_LIB_DIR}/pluginterfaces.lib"
    "${VST3_LIB_DIR}/vstgui.lib"
    "${VST3_LIB_DIR}/vstgui_support.lib"
    "${VST3_LIB_DIR}/vstgui_uidescription.lib"
)

# --- 6. VST3繝励Λ繧ｰ繧､繝ｳ縺ｨ縺励※縺ｮ莉穂ｸ翫￡ ---
# 諡｡蠑ｵ蟄舌ｒ .vst3 縺ｫ
set_target_properties(MonophonicSynth PROPERTIES SUFFIX ".vst3")

# 萓句､門・逅・・譛牙柑蛹・
target_compile_options(MonophonicSynth PRIVATE /EHsc)

# Windows逕ｨ繧ｨ繧ｯ繧ｹ繝昴・繝磯未謨ｰ縺ｮ螳夂ｾｩ
if(WIN32)
    target_link_options(MonophonicSynth PRIVATE /EXPORT:GetPluginFactory /EXPORT:InitDll /EXPORT:ExitDll)
endif()