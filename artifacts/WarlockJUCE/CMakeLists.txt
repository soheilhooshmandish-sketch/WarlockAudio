cmake_minimum_required(VERSION 3.22)
project(Warlock VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(Warlock
    COMPANY_NAME "NihilAudio"
    PLUGIN_NAME "Warlock"
    PRODUCT_NAME "Warlock Chainsaw Distortion"
    VERSION 1.1.0
    BUNDLE_ID com.nihilaudio.warlock
    PLUGIN_MANUFACTURER_CODE Nhil
    PLUGIN_CODE Wrck
    VST3_CATEGORIES Fx Distortion
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS VST3
)

juce_generate_juce_header(Warlock)

# Compiled INTO the DLL. The editor never needs these files on disk.
juce_add_binary_data(WarlockData HEADER_NAME BinaryData.h NAMESPACE BinaryData SOURCES
    Resources/CabinetIR.wav
    Resources/Cab_Center.wav
    Resources/Cab_Edge.wav
    Resources/legal.txt
    Resources/chassis.jpg
    Resources/knob.jpg
    Resources/stomp.jpg
    Resources/logo.png
)

target_sources(Warlock PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
)

target_compile_definitions(Warlock PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Warlock PRIVATE
    WarlockData
    juce::juce_audio_utils
    juce::juce_audio_formats
    juce::juce_dsp
)

if(MSVC)
    target_compile_options(Warlock PRIVATE /O2 /Oi /Ot /AVX2 /FP:fast)
endif()

# Host-visible extras only. Skins and IRs are already inside the DLL (BinaryData).
# TARGET_FILE_DIR = .../Warlock.vst3/Contents/x86_64-win
if(TARGET Warlock_VST3)
    add_custom_command(TARGET Warlock_VST3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:Warlock_VST3>/../Resources/Snapshots"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources/legal.txt"
            "$<TARGET_FILE_DIR:Warlock_VST3>/../Resources/legal.txt"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources/moduleinfo.example.json"
            "$<TARGET_FILE_DIR:Warlock_VST3>/../Resources/moduleinfo.json"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Snapshots"
            "$<TARGET_FILE_DIR:Warlock_VST3>/../Resources/Snapshots"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources/windows/desktop.ini"
            "$<TARGET_FILE_DIR:Warlock_VST3>/../../desktop.ini"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources/windows/Plugin.ico"
            "$<TARGET_FILE_DIR:Warlock_VST3>/../../Plugin.ico"
        COMMENT "Packaging VST3 Snapshots, legal, and Explorer icon (skins live in BinaryData)"
    )
endif()
