cmake_minimum_required(VERSION 3.22)
project(Warlock VERSION 1.5.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.5
)
FetchContent_MakeAvailable(JUCE)
juce_add_plugin(Warlock
    COMPANY_NAME "NihilAudio"
    PLUGIN_NAME "Warlock"
    PRODUCT_NAME "Warlock Satanic Complete Suite"
    VERSION 1.5.0
    BUNDLE_ID com.nihilaudio.warlock.satanicsuite
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    FORMATS VST3
    COPY_PLUGIN_AFTER_BUILD FALSE
)
target_sources(Warlock PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/FactoryPresets.h
)
juce_generate_juce_header(Warlock)
target_compile_definitions(Warlock PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(Warlock PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_formats
    juce::juce_dsp
)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

if(MSVC)
    target_compile_options(Warlock PRIVATE
        $<$<CONFIG:Release>:/O2 /Oi /Ot /Oy /AVX2 /FP:fast /GL /Gw /Gy>
        $<$<CONFIG:RelWithDebInfo>:/O2 /Oi /Ot /AVX2 /FP:fast>
    )
    target_link_options(Warlock PRIVATE
        $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF /INCREMENTAL:NO>
    )
    target_compile_definitions(Warlock PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
    )
else()
    target_compile_options(Warlock PRIVATE
        $<$<CONFIG:Release>:-O3 -ffast-math -ftree-vectorize -mavx2 -mfma -flto>
    )
    target_link_options(Warlock PRIVATE
        $<$<CONFIG:Release>:-flto -Wl,-O1>
    )
endif()
