cmake_minimum_required(VERSION 3.22)
project(BabyGirl VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional Universal Binary on macOS
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architectures for macOS" FORCE)
endif()

# Fetch JUCE automatically via CMake FetchContent
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# Define BabyGirl VST3 Plugin & Standalone Emulator Executable
juce_add_plugin(BabyGirl
    COMPANY_NAME "KeganaCummings"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    PLUGIN_MANUFACTURER_CODE Kcum
    PLUGIN_CODE BbyG
    FORMATS VST3 Standalone
    PRODUCT_NAME "BabyGirl"
    DESCRIPTION "BabyGirl.vst - Unified Analog-Modeled Synthesizer & Modular DSP Console"
)

# Source files
target_sources(BabyGirl PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/HushDsp.h
    Source/KyotoDsp.h
    Source/MasterDsp.h
    Source/ModulesDsp.h
    Source/DreamShare.h
    Source/DreamApi.h
    Source/Presets.h
    Source/CustomLookAndFeel.h
)

# Link JUCE Modules
target_link_libraries(BabyGirl PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_data_structures
    juce::juce_events
    juce::juce_core
)

# Compile definitions
target_compile_definitions(BabyGirl PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=1          # Required: DreamApi.h uses juce::URL networking for cloud preset sync
    JUCE_VST3_CAN_REPLACE_VST2=0
    BABYGIRL_VERSION="1.0.0"
)

# Link libcurl on Linux (needed by JUCE_USE_CURL)
if(UNIX AND NOT APPLE)
    find_package(CURL REQUIRED)
    target_link_libraries(BabyGirl PRIVATE CURL::libcurl)
endif()
