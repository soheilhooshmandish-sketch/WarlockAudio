cmake_minimum_required(VERSION 3.22)

project(WarlockAI VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.15
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(JUCE)

set(WARLOCK_AI_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND WARLOCK_AI_FORMATS AU)
endif()

juce_add_plugin(WarlockAI
    COMPANY_NAME "Black Forge Audio"
    COMPANY_WEBSITE "https://blackforge.audio"
    BUNDLE_ID com.blackforgeaudio.warlockai
    PLUGIN_MANUFACTURER_CODE BlFg
    PLUGIN_CODE WlAI
    PLUGIN_NAME "Warlock AI"
    PRODUCT_NAME "Warlock AI"
    VERSION ${PROJECT_VERSION}
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS ${WARLOCK_AI_FORMATS}
    VST3_CATEGORIES Fx Distortion
    AU_MAIN_TYPE kAudioUnitType_Effect
)

juce_generate_juce_header(WarlockAI)

target_sources(WarlockAI
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Core/WarlockParameters.cpp
        Source/Core/PresetManager.cpp
        Source/Engine/SignalChain.cpp
        Source/DSP/DistortionProcessor.cpp
        Source/DSP/NoiseGate.cpp
        Source/DSP/Compressor.cpp
        Source/DSP/Equalizer.cpp
        Source/DSP/DelayProcessor.cpp
        Source/DSP/ReverbProcessor.cpp
        Source/DSP/StereoProcessor.cpp
        Source/DSP/Limiter.cpp
        Source/DSP/AmpProcessor.cpp
        Source/DSP/CabinetProcessor.cpp
        Source/DSP/Harmonizer.cpp
        Source/AI/AudioFeatureExtractor.cpp
        Source/AI/ToneAnalyzer.cpp
        Source/AI/ToneHealth.cpp
        Source/AI/MixFit.cpp
        Source/AI/StyleDetector.cpp
        Source/AI/ToneDesigner.cpp
        Source/AI/RuleBasedToneModel.cpp
        Source/AI/AIEngine.cpp
        Source/UI/WarlockLookAndFeel.cpp
        Source/UI/WarlockKnob.cpp
        Source/UI/SpectrumComponent.cpp
        Source/UI/ForgeSection.cpp
        Source/UI/MeterBar.cpp
        Source/UI/AiDashboard.cpp
)

target_include_directories(WarlockAI
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_compile_definitions(WarlockAI
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
)

target_link_libraries(WarlockAI
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

if(MSVC)
    target_compile_options(WarlockAI PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
        $<$<CONFIG:Release>:/O2 /Oi /Ot /Oy /arch:AVX2 /fp:fast /GL /Gw /Gy>
    )
    target_link_options(WarlockAI PRIVATE
        $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF /INCREMENTAL:NO>
    )
else()
    target_compile_options(WarlockAI PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Release>:-O3 -ffast-math>
    )
endif()

juce_add_console_app(WarlockAIDspSmoke
    PRODUCT_NAME "WarlockAIDspSmoke"
)

juce_generate_juce_header(WarlockAIDspSmoke)

target_sources(WarlockAIDspSmoke
    PRIVATE
        Source/Tests/DspSmoke.cpp
        Source/Core/WarlockParameters.cpp
        Source/Engine/SignalChain.cpp
        Source/DSP/DistortionProcessor.cpp
        Source/DSP/NoiseGate.cpp
        Source/DSP/Compressor.cpp
        Source/DSP/Equalizer.cpp
        Source/DSP/DelayProcessor.cpp
        Source/DSP/ReverbProcessor.cpp
        Source/DSP/StereoProcessor.cpp
        Source/DSP/Limiter.cpp
        Source/DSP/AmpProcessor.cpp
        Source/DSP/CabinetProcessor.cpp
        Source/DSP/Harmonizer.cpp
)

target_include_directories(WarlockAIDspSmoke PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_compile_definitions(WarlockAIDspSmoke
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(WarlockAIDspSmoke
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_dsp
        juce::juce_recommended_config_flags
)

juce_add_console_app(WarlockAITests
    PRODUCT_NAME "WarlockAITests"
)

juce_generate_juce_header(WarlockAITests)

target_sources(WarlockAITests
    PRIVATE
        Source/Tests/AiTests.cpp
        Source/Core/WarlockParameters.cpp
        Source/DSP/DistortionProcessor.cpp
        Source/AI/AudioFeatureExtractor.cpp
        Source/AI/ToneAnalyzer.cpp
        Source/AI/ToneHealth.cpp
        Source/AI/MixFit.cpp
        Source/AI/StyleDetector.cpp
        Source/AI/ToneDesigner.cpp
        Source/AI/RuleBasedToneModel.cpp
        Source/AI/AIEngine.cpp
)

target_include_directories(WarlockAITests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_compile_definitions(WarlockAITests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(WarlockAITests
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_dsp
        juce::juce_recommended_config_flags
)
