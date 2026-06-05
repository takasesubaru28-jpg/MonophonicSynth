cmake_minimum_required(VERSION 3.15)
set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "Windows SDK version" FORCE)
project(Mini_Moog VERSION 1.0.0)

# --- C++17有効化 (VSTGUI4に必須) ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- 1. パス設定 ---
# 親ディレクトリにある vst3sdk を参照
get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
set(VST3_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vst3sdk")

# --- 2. ターゲット作成 (DLLとして定義) ---
add_library(Mini_Moog MODULE
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

# --- 3. インクルードディレクトリの設定 ---
target_include_directories(Mini_Moog PRIVATE 
    source
    "${VST3_SDK_ROOT}"
    "${VST3_SDK_ROOT}/vstgui4"
)

# --- 4. プリプロセッサの定義 ---
target_compile_definitions(Mini_Moog PRIVATE 
    VSTGUI_LIVE_EDITING=1
    DEVELOPMENT=1
    WIN32=1
    _WINDOWS=1
)

# --- 5. リンカー設定 (SDKのビルド済み.libを直接リンク) ---
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(VST3_LIB_DIR "${VST3_SDK_ROOT}/out/build/x64-Debug/lib")
else()
    set(VST3_LIB_DIR "${VST3_SDK_ROOT}/out/build/x64-Release/lib")
endif()

target_link_libraries(Mini_Moog PRIVATE 
    "${VST3_LIB_DIR}/base.lib"
    "${VST3_LIB_DIR}/sdk.lib"
    "${VST3_LIB_DIR}/sdk_common.lib"
    "${VST3_LIB_DIR}/pluginterfaces.lib"
    "${VST3_LIB_DIR}/vstgui.lib"
    "${VST3_LIB_DIR}/vstgui_support.lib"
    "${VST3_LIB_DIR}/vstgui_uidescription.lib"
)

# --- 6. VST3プラグインとしての仕上げ ---
# 拡張子を .vst3 に
set_target_properties(Mini_Moog PROPERTIES SUFFIX ".vst3")

# 例外処理の有効化
target_compile_options(Mini_Moog PRIVATE /EHsc)

# Windows用エクスポート関数の定義
if(WIN32)
    target_link_options(Mini_Moog PRIVATE /EXPORT:GetPluginFactory /EXPORT:InitDll /EXPORT:ExitDll)
endif()