cmake_minimum_required(VERSION 3.22)
project(DistPedal VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE 8.0.12 — includes Visual Studio 2026 (VS 18) Projucer exporter.
include(FetchContent)
FetchContent_Declare(juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.12
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(juce)

set(DISTPEDAL_FORMATS VST3 Standalone)
if(DEFINED ENV{VST2_SDK} AND EXISTS "$ENV{VST2_SDK}/pluginterfaces/vst2.x/aeffect.h")
    message(STATUS "VST2 SDK found — also building VST2 .dll")
    juce_set_vst2_sdk_path("$ENV{VST2_SDK}")
    list(APPEND DISTPEDAL_FORMATS VST)
endif()

juce_add_plugin(DistPedal
    COMPANY_NAME "The Warlock"
    COMPANY_WEBSITE "https://thewarlock.local"
    BUNDLE_ID com.thewarlock.distpedal
    PLUGIN_MANUFACTURER_CODE WrLk
    PLUGIN_CODE DstP
    FORMATS ${DISTPEDAL_FORMATS}
    PRODUCT_NAME "DistPedal"
    DESCRIPTION "Black-metal distortion — port of The Warlock Web Audio DSP"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES Distortion Fx
    VST2_CATEGORY kPlugCategEffect)

juce_generate_juce_header(DistPedal)

target_sources(DistPedal PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/DistDSP.cpp
    Source/DistDSP.h
    Source/CabModel.cpp
    Source/CabModel.h)

target_compile_definitions(DistPedal PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(DistPedal PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)
