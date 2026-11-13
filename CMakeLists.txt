cmake_minimum_required(VERSION 3.22)
project(PPGU VERSION 4.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE EXCLUDE_FROM_ALL)

# §16.1 BINARY DATA CONTRACT — binary targets declared BEFORE dependents.
juce_add_binary_data(PPGUData
    HEADER_NAME PPGUData.h
    NAMESPACE BinaryData
    SOURCES
        Resources/Embedded/FactoryCatalog.json
        Resources/Embedded/FactorySfz.sfz)

juce_add_binary_data(PPGUTestData
    HEADER_NAME PPGUTestData.h
    NAMESPACE PPGUTestData
    SOURCES
        Resources/TestData/TestProbe.wav)

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
    VST3_CATEGORIES Instruments Synth)

target_sources(PPGU PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Engine/SfzParser.cpp
    Source/Engine/PatternScheduler.cpp
    Source/Engine/SamplePool.cpp
    Source/Engine/SampleStreamingService.cpp
    Source/Engine/SamplerEngine.cpp
    Source/Engine/FxChain.cpp
    Source/Engine/ChordEngine.cpp
    Source/Engine/ImportPipeline.cpp
    Source/Engine/PresetRuntime.cpp
    Source/UI/ThemeManager.cpp
    Source/UI/AuxiliaryPanelLayout.cpp
    Source/UI/MultiWindowWorkspace.cpp
    Source/UI/Panels.cpp)

target_link_libraries(PPGU PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_formats
    PPGUData
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)

target_compile_definitions(PPGU PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_FLAC=1
    JUCE_USE_OGGVORBIS=1
    JUCE_USE_MP3AUDIOFORMAT=1
    JUCE_VST3_CAN_REPLACE_VST2=0)

add_executable(ppgu_engine_contracts
    tests/EngineContractTests.cpp
    Source/Engine/SfzParser.cpp
    Source/Engine/PatternScheduler.cpp
    Source/Engine/ChordEngine.cpp)
target_include_directories(ppgu_engine_contracts PRIVATE Source)
target_compile_definitions(ppgu_engine_contracts PRIVATE PPGU_CONTRACT_TEST=1)

add_executable(ppgu_theme_contracts
    tests/ThemeManagerTests.cpp
    Source/UI/ThemeManager.cpp)
target_include_directories(ppgu_theme_contracts PRIVATE Source)
target_compile_definitions(ppgu_theme_contracts PRIVATE PPGU_CONTRACT_TEST=1)

add_executable(ppgu_panel_contracts
    tests/AuxiliaryPanelTests.cpp
    Source/UI/AuxiliaryPanelLayout.cpp
    Source/UI/ThemeManager.cpp)
target_include_directories(ppgu_panel_contracts PRIVATE Source)
target_compile_definitions(ppgu_panel_contracts PRIVATE PPGU_CONTRACT_TEST=1)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
enable_testing()
add_test(NAME ppgu_engine_contracts COMMAND ppgu_engine_contracts)
add_test(NAME ppgu_theme_contracts COMMAND ppgu_theme_contracts)
add_test(NAME ppgu_panel_contracts COMMAND ppgu_panel_contracts)
add_test(NAME ppgu_sound_asset_registry_contract
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/TestSoundAssetRegistry.py)
# ---- PPGU contract targets (O1/O2/S1 stage) ----
add_library(ppgu_overlay_contracts STATIC
    Source/UI/OverlayManager.h
    Source/UI/OverlayManager.cpp)
target_include_directories(ppgu_overlay_contracts PUBLIC Source)
target_compile_features(ppgu_overlay_contracts PUBLIC cxx_std_17)

add_library(ppgu_skin_import_contracts STATIC
    Source/Security/SecureSkinImport.h
    Source/Security/SecureSkinImport.cpp)
target_include_directories(ppgu_skin_import_contracts PUBLIC Source)
target_compile_features(ppgu_skin_import_contracts PUBLIC cxx_std_17)
target_link_libraries(ppgu_skin_import_contracts PUBLIC ppgu_overlay_contracts)

add_executable(ppgu_overlay_contract_tests Tests/OverlayManagerTests.cpp)
target_compile_definitions(ppgu_overlay_contract_tests PRIVATE PPGU_STANDALONE_TEST)
target_link_libraries(ppgu_overlay_contract_tests PRIVATE ppgu_overlay_contracts)
add_test(NAME ppgu_overlay_contract_tests COMMAND ppgu_overlay_contract_tests)

add_executable(ppgu_skin_import_contract_tests Tests/SecureSkinImportTests.cpp)
target_compile_definitions(ppgu_skin_import_contract_tests PRIVATE PPGU_STANDALONE_TEST)
target_link_libraries(ppgu_skin_import_contract_tests PRIVATE ppgu_skin_import_contracts)
add_test(NAME ppgu_skin_import_contract_tests COMMAND ppgu_skin_import_contract_tests)
