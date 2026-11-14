cmake_minimum_required(VERSION 3.22)
project(PPGU VERSION 4.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# JUCE is pinned to the product's declared framework version. Fetching it here
# makes a clean checkout/build reproducible; the repository does not vendor JUCE.
include(FetchContent)
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
    GIT_SHALLOW TRUE
    GIT_SUBMODULES_RECURSE TRUE
)
FetchContent_MakeAvailable(JUCE)

# The runtime preset browser reads this embedded catalog by its generated
# BinaryData symbol. Do not reference absent legacy Resources/ files.
juce_add_binary_data(PPGUData
    HEADER_NAME PPGUData.h
    NAMESPACE BinaryData
    SOURCES
        presets/factory_bank_300.json
)

juce_add_plugin(PPGU
    COMPANY_NAME "L3XTOWN AUDIO LAB"
    PLUGIN_MANUFACTURER_CODE L3xa
    PLUGIN_CODE Ppgu
    FORMATS VST3
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PRODUCT_NAME "POP PUNK GUITAR ULTIMATE"
    VST3_CATEGORIES Instrument Synth
)
juce_generate_juce_header(PPGU)

# Canonical sample engine. SamplerEngine is implemented in SamplePool.cpp;
# transport/pattern and chord transition components are header-only.
add_library(ppgu_sampler_core STATIC
    Source/Sampler/SfzParser.cpp
    Source/Sampler/SamplePool.cpp
    Source/Sampler/SampleStreamingService.cpp
    Source/Sampler/SampleMetadataIndex.cpp
    Source/Sampler/PageCacheSlots.cpp
    Source/Sampler/StreamingVoiceReader.cpp
    Source/Sampler/WavPageReader.cpp
)
target_include_directories(ppgu_sampler_core PUBLIC
    Source
    Source/Sampler
    Source/Performance
)
target_link_libraries(ppgu_sampler_core PUBLIC
    juce::juce_audio_formats
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)
target_compile_definitions(ppgu_sampler_core PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_FLAC=1
    JUCE_USE_OGGVORBIS=1
    JUCE_USE_MP3AUDIOFORMAT=1
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_sources(PPGU PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/UI/ThemeManager.cpp
    Source/UI/AuxiliaryPanelLayout.cpp
    Source/UI/AuxiliaryPanelWindowManager.cpp
    Source/UI/AutoChordsPanel.cpp
    Source/UI/EditorOverlayPanel.cpp
    Source/UI/FXEditPanel.cpp
    Source/UI/ImportGuitarPanel.cpp
    Source/UI/PerformanceDisplay.cpp
    Source/UI/SettingsPanel.cpp
)
target_include_directories(PPGU PRIVATE
    Source
    Source/Sampler
    Source/Performance
    Source/UI
)
target_link_libraries(PPGU PRIVATE
    ppgu_sampler_core
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_formats
    PPGUData
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)

# Small deterministic contract tests are separate executables because each
# source owns its own main(). This also keeps failures attributable.
function(ppgu_add_header_test target source)
    add_executable(${target} ${source})
    target_include_directories(${target} PRIVATE Source/Performance Source/Sampler)
    target_compile_features(${target} PRIVATE cxx_std_20)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

enable_testing()
ppgu_add_header_test(ppgu_pattern_scheduler_tests tests/PatternSchedulerTests.cpp)
ppgu_add_header_test(ppgu_auto_chord_engine_tests tests/AutoChordEngineTests.cpp)
ppgu_add_header_test(ppgu_chord_transition_policy_tests tests/ChordTransitionPolicyTests.cpp)
ppgu_add_header_test(ppgu_circle_of_fifths_tests tests/CircleOfFifthsTests.cpp)
ppgu_add_header_test(ppgu_queue_tests tests/BoundedSpscQueueTests.cpp)

add_executable(ppgu_sfz_parser_tests
    tests/SfzParserTests.cpp
    Source/Sampler/SfzParser.cpp
)
target_include_directories(ppgu_sfz_parser_tests PRIVATE Source/Sampler)
add_test(NAME ppgu_sfz_parser_tests COMMAND ppgu_sfz_parser_tests)

add_executable(ppgu_page_cache_tests
    tests/PageCacheSlotsTests.cpp
    Source/Sampler/PageCacheSlots.cpp
)
target_include_directories(ppgu_page_cache_tests PRIVATE Source/Sampler)
add_test(NAME ppgu_page_cache_tests COMMAND ppgu_page_cache_tests)

foreach(test_name IN ITEMS SampleMetadataIndex SamplePool SampleStreamingService)
    string(TOLOWER "${test_name}" test_suffix)
    add_executable(ppgu_${test_suffix}_tests tests/${test_name}Tests.cpp)
    target_link_libraries(ppgu_${test_suffix}_tests PRIVATE
        ppgu_sampler_core
        juce::juce_audio_formats
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_${test_suffix}_tests COMMAND ppgu_${test_suffix}_tests)
endforeach()

add_executable(ppgu_theme_contracts
    tests/ThemeManagerTests.cpp
    Source/UI/ThemeManager.cpp
)
target_include_directories(ppgu_theme_contracts PRIVATE Source/UI)
target_link_libraries(ppgu_theme_contracts PRIVATE
    juce::juce_gui_basics
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)
add_test(NAME ppgu_theme_contracts COMMAND ppgu_theme_contracts)

add_executable(ppgu_panel_contracts
    tests/AuxiliaryPanelLayoutTests.cpp
    Source/UI/AuxiliaryPanelLayout.cpp
)
target_include_directories(ppgu_panel_contracts PRIVATE Source/UI)
target_link_libraries(ppgu_panel_contracts PRIVATE
    juce::juce_gui_basics
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)
add_test(NAME ppgu_panel_contracts COMMAND ppgu_panel_contracts)

# Standalone security/overlay utilities currently live at repository root.
add_executable(ppgu_overlay_contract_tests
    OverlayManagerTests.cpp
    OverlayManager.cpp
)
target_include_directories(ppgu_overlay_contract_tests PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_definitions(ppgu_overlay_contract_tests PRIVATE PPGU_STANDALONE_TEST)
add_test(NAME ppgu_overlay_contract_tests COMMAND ppgu_overlay_contract_tests)

add_executable(ppgu_skin_import_contract_tests
    SecureSkinImportTests.cpp
    SecureSkinImport.cpp
)
target_include_directories(ppgu_skin_import_contract_tests PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_definitions(ppgu_skin_import_contract_tests PRIVATE PPGU_STANDALONE_TEST)
add_test(NAME ppgu_skin_import_contract_tests COMMAND ppgu_skin_import_contract_tests)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_test(
    NAME ppgu_sound_asset_registry_contract
    COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/tools/validate_sound_asset_registry.py
        --registry ${CMAKE_SOURCE_DIR}/assets/sound_hunter/registry.json
        --root ${CMAKE_SOURCE_DIR}
)
