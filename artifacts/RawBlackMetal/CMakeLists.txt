cmake_minimum_required(VERSION 3.22)
project(RawBlackMetal VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(RawBlackMetal
    COMPANY_NAME "NihilAudio"
    PLUGIN_NAME "RawBlackMetal"
    PRODUCT_NAME "Raw Black Metal Distortion"
    VERSION 1.0.0
    BUNDLE_ID com.nihilaudio.rawblackmetal
    PLUGIN_MANUFACTURER_CODE Nhil
    PLUGIN_CODE Rbmx
    VST3_CATEGORIES Fx Distortion
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS VST3 Standalone
)

juce_generate_juce_header(RawBlackMetal)

target_sources(RawBlackMetal PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
)

target_compile_definitions(RawBlackMetal PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(RawBlackMetal PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

if(MSVC)
    target_compile_options(RawBlackMetal PRIVATE /O2 /Oi /Ot /AVX2 /FP:fast)
endif()
