cmake_minimum_required(VERSION 3.22)
project(PPGU VERSION 4.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# JUCE 7.0.12 is checked out by the Windows workflow into this directory.
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    message(FATAL_ERROR "JUCE 7.0.12 checkout is missing. Run the configured workflow or provide JUCE/.")
endif()
add_subdirectory(JUCE EXCLUDE_FROM_ALL)

# Binary-data targets must be declared before every target that includes their
# generated headers. These are the canonical files present in REV.17.
juce_add_binary_data(PPGUData
    HEADER_NAME PPGUData.h
    NAMESPACE BinaryData
    SOURCES
        presets/factory_bank_300.json)

juce_add_binary_data(PPGUTestData
    HEADER_NAME PPGUTestData.h
    NAMESPACE PPGUTestData
    SOURCES
        tests/assets/ppgu_test_data.txt)

set(PPGU_JUCE_DEFINITIONS
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_FLAC=1
    JUCE_USE_OGGVORBIS=1
    JUCE_USE_MP3AUDIOFORMAT=1
    JUCE_VST3_CAN_REPLACE_VST2=0)

# The current production sampler lives under Source/Sampler. Keep the complete
# implementation in one library so the plugin and regression tests exercise
# exactly the same code.
set(PPGU_SAMPLER_SOURCES
    Source/Sampler/SfzParser.cpp
    Source/Sampler/SamplePool.cpp
    Source/Sampler/SampleStreamingService.cpp
    Source/Sampler/StreamingVoiceReader.cpp
    Source/Sampler/WavPageReader.cpp
    Source/Sampler/PageCacheSlots.cpp
    Source/Sampler/SampleMetadataIndex.cpp)

add_library(ppgu_sampler STATIC ${PPGU_SAMPLER_SOURCES})
set_target_properties(ppgu_sampler PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(ppgu_sampler PUBLIC Source Source/Sampler)
target_compile_definitions(ppgu_sampler PUBLIC ${PPGU_JUCE_DEFINITIONS})
target_link_libraries(ppgu_sampler PUBLIC
    juce::juce_audio_formats
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)

juce_add_plugin(PPGU
    COMPANY_NAME "L3XTOWN AUDIO LAB"
    BUNDLE_ID "com.l3xtown.audio.ppgu"
    PLUGIN_MANUFACTURER_CODE L3xa
    PLUGIN_CODE Ppgu
    FORMATS VST3
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PRODUCT_NAME "POP PUNK GUITAR ULTIMATE"
    VST3_CATEGORIES Instruments Synth)

target_sources(PPGU PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/UI/ThemeManager.cpp
    Source/UI/AuxiliaryPanelLayout.cpp
    Source/UI/PPGUMultiWindowWorkspace.cpp)
target_include_directories(PPGU PRIVATE Source Source/Sampler Source/Performance Source/UI)
target_compile_definitions(PPGU PUBLIC ${PPGU_JUCE_DEFINITIONS})
target_link_libraries(PPGU PRIVATE
    ppgu_sampler
    PPGUData
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_formats
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)
juce_generate_juce_header(PPGU)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
enable_testing()

function(ppgu_add_test target source)
    add_executable(${target})
    target_sources(${target} PRIVATE ${source})
    add_test(NAME ${target} COMMAND ${target})
endfunction()

# Pure C++ contract tests.
ppgu_add_test(ppgu_pattern_contracts tests/PatternSchedulerTests.cpp)
target_include_directories(ppgu_pattern_contracts PRIVATE Source/Performance)

ppgu_add_test(ppgu_chord_transition_contracts tests/ChordTransitionPolicyTests.cpp)
target_include_directories(ppgu_chord_transition_contracts PRIVATE Source/Performance)

ppgu_add_test(ppgu_sfz_contracts tests/SfzParserTests.cpp)
target_sources(ppgu_sfz_contracts PRIVATE Source/Sampler/SfzParser.cpp)
target_include_directories(ppgu_sfz_contracts PRIVATE Source/Sampler)

ppgu_add_test(ppgu_queue_contracts tests/BoundedSpscQueueTests.cpp)
target_include_directories(ppgu_queue_contracts PRIVATE Source/Sampler)

ppgu_add_test(ppgu_page_cache_contracts tests/PageCacheSlotsTests.cpp)
target_sources(ppgu_page_cache_contracts PRIVATE Source/Sampler/PageCacheSlots.cpp)
target_include_directories(ppgu_page_cache_contracts PRIVATE Source/Sampler)

ppgu_add_test(ppgu_metadata_contracts tests/SampleMetadataIndexTests.cpp)
target_include_directories(ppgu_metadata_contracts PRIVATE Source Source/Sampler)
target_link_libraries(ppgu_metadata_contracts PRIVATE ppgu_sampler)

ppgu_add_test(ppgu_panel_contracts tests/AuxiliaryPanelLayoutTests.cpp)
target_sources(ppgu_panel_contracts PRIVATE Source/UI/AuxiliaryPanelLayout.cpp)
target_include_directories(ppgu_panel_contracts PRIVATE Source/UI)

# JUCE-backed sampler and UI contract tests.
ppgu_add_test(ppgu_sample_pool_contracts tests/SamplePoolTests.cpp)
target_include_directories(ppgu_sample_pool_contracts PRIVATE Source Source/Sampler)
target_compile_definitions(ppgu_sample_pool_contracts PRIVATE ${PPGU_JUCE_DEFINITIONS})
target_link_libraries(ppgu_sample_pool_contracts PRIVATE ppgu_sampler)

ppgu_add_test(ppgu_streaming_contracts tests/SampleStreamingServiceTests.cpp)
target_include_directories(ppgu_streaming_contracts PRIVATE Source Source/Sampler)
target_compile_definitions(ppgu_streaming_contracts PRIVATE ${PPGU_JUCE_DEFINITIONS})
target_link_libraries(ppgu_streaming_contracts PRIVATE ppgu_sampler)

juce_add_console_app(ppgu_theme_contracts
    PRODUCT_NAME "PPGU Theme Contract Tests"
    NEEDS_WEB_BROWSER FALSE
    NEEDS_CURL FALSE)
target_sources(ppgu_theme_contracts PRIVATE
    tests/ThemeManagerTests.cpp
    Source/UI/ThemeManager.cpp)
add_test(NAME ppgu_theme_contracts COMMAND ppgu_theme_contracts)
set_tests_properties(ppgu_theme_contracts PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
target_include_directories(ppgu_theme_contracts PRIVATE Source Source/UI)
target_compile_definitions(ppgu_theme_contracts PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(ppgu_theme_contracts PRIVATE juce::juce_core juce::juce_graphics)
juce_generate_juce_header(ppgu_theme_contracts)

# End-to-end MIDI -> PatternScheduler -> SamplerEngine -> rendered audio test.
# It compiles the same processor/editor/UI/sampler sources used by the VST3.
juce_add_console_app(ppgu_processor_pattern_tests
    PRODUCT_NAME "PPGU Processor Pattern Tests"
    NEEDS_WEB_BROWSER FALSE
    NEEDS_CURL FALSE)
target_sources(ppgu_processor_pattern_tests PRIVATE
    tests/ProcessorPatternIntegrationTests.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/UI/ThemeManager.cpp
    Source/UI/AuxiliaryPanelLayout.cpp
    Source/UI/PPGUMultiWindowWorkspace.cpp)
target_include_directories(ppgu_processor_pattern_tests PRIVATE
    Source Source/Sampler Source/Performance Source/UI)
target_compile_definitions(ppgu_processor_pattern_tests PRIVATE ${PPGU_JUCE_DEFINITIONS})
target_link_libraries(ppgu_processor_pattern_tests PRIVATE
    ppgu_sampler
    PPGUData
    PPGUTestData
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_formats
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)
juce_generate_juce_header(ppgu_processor_pattern_tests)
add_test(NAME ppgu_processor_pattern_tests COMMAND ppgu_processor_pattern_tests)

add_test(NAME ppgu_sound_asset_registry_contract
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/TestSoundAssetRegistry.py)

# The orphaned root-level OverlayManager/SecureSkinImport test files are not
# registered: their corresponding production headers and implementations are
# absent from this REV.17 tree. They remain explicit implementation blockers,
# not silently reported as passing tests.
