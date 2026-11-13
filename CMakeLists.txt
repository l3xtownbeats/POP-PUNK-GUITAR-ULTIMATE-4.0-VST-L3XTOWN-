cmake_minimum_required(VERSION 3.22)
project(PopPunkGuitarUltimate VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define BUILD_TESTING before any test target is conditionally declared.
# CTest's default option is ON; placing this here prevents early test blocks
# from being silently skipped because BUILD_TESTING was still undefined.
include(CTest)

# Pin JUCE 7 as required by v33 FINAL. FetchContent keeps the dependency reproducible.
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.14-security
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# JUCE owns the VST3 bundle install step; this avoids copying the shared-code
# target file instead of the actual .vst3 bundle.
if(WIN32)
    set(PPGU_FL_VST3_DIR "$ENV{CommonProgramFiles}/VST3" CACHE PATH
        "Destination for the post-build VST3 bundle")
endif()

juce_add_binary_data(PPGUPresetData
    HEADER_NAME "PPGUData.h"
    SOURCES presets/factory_bank_300.json
)

juce_add_plugin(PopPunkGuitarUltimate
    COMPANY_NAME "L3XTOWN"
    BUNDLE_ID "com.l3xtown.pop-punk-guitar-ultimate"
    PLUGIN_MANUFACTURER_CODE L3XT
    PLUGIN_CODE PPGU
    PRODUCT_NAME "POP PUNK GUITAR ULTIMATE"
    DESCRIPTION "Pop-punk guitar sampler instrument"
    FORMATS VST3
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "${PPGU_FL_VST3_DIR}"
)

# Runtime themes are shipped beside the VST3 bundle, matching the editor's
# sibling Skins/ discovery contract. This is visual data only; it is not linked
# into the audio binary or accessed from the audio callback.
if(TARGET PopPunkGuitarUltimate_VST3)
    set(PPGU_RUNTIME_SKINS_DIR "$<TARGET_FILE_DIR:PopPunkGuitarUltimate_VST3>/../../../Skins")
    add_custom_command(TARGET PopPunkGuitarUltimate_VST3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PPGU_RUNTIME_SKINS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/SKIN"
            "${PPGU_RUNTIME_SKINS_DIR}"
        COMMENT "Packaging PPGU visual themes beside the VST3 bundle"
        VERBATIM
    )
endif()

target_sources(PopPunkGuitarUltimate PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/UI/ThemeManager.cpp
    Source/UI/ThemeManager.h
    Source/UI/AuxiliaryPanelLayout.cpp
    Source/UI/AuxiliaryPanelLayout.h
    Source/UI/PPGUMultiWindowWorkspace.cpp
    Source/UI/PPGUMultiWindowWorkspace.h
    Source/Sampler/SfzParser.cpp
    Source/Sampler/SfzParser.h
    Source/Sampler/SamplePool.cpp
    Source/Sampler/SamplePool.h
    Source/Sampler/SampleMetadataIndex.cpp
    Source/Sampler/SampleMetadataIndex.h
    Source/Sampler/WavPageReader.cpp
    Source/Sampler/WavPageReader.h
    Source/Sampler/PageCacheSlots.cpp
    Source/Sampler/PageCacheSlots.h
    Source/Sampler/SampleStreamingService.cpp
    Source/Sampler/SampleStreamingService.h
    Source/Sampler/StreamingVoiceReader.cpp
    Source/Sampler/StreamingVoiceReader.h
)

target_compile_definitions(PopPunkGuitarUltimate PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(PopPunkGuitarUltimate PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    PPGUPresetData
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Generate the compatibility header after all JUCE modules are linked.
juce_generate_juce_header(PopPunkGuitarUltimate)

if(MSVC)
    target_compile_options(PopPunkGuitarUltimate PRIVATE /W4 /permissive- /Zc:__cplusplus)
endif()

# Keep installation separate from compilation: the CI job validates and archives
# the generated JUCE VST3 bundle without writing to a machine-wide plugin folder.


# Pure geometry contract for independent auxiliary editor panels.
if(BUILD_TESTING)
    add_executable(ppgu_auxiliary_panel_layout_tests
        Source/UI/AuxiliaryPanelLayout.cpp
        tests/AuxiliaryPanelLayoutTests.cpp
    )
    target_include_directories(ppgu_auxiliary_panel_layout_tests PRIVATE Source/UI)
    target_link_libraries(ppgu_auxiliary_panel_layout_tests PRIVATE
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_auxiliary_panel_layout_contract COMMAND ppgu_auxiliary_panel_layout_tests)
endif()

# Dependency-free SFZ parser regression tests.
if(BUILD_TESTING)
    add_executable(ppgu_sfz_parser_tests
        Source/Sampler/SfzParser.cpp
        tests/SfzParserTests.cpp
    )
    target_include_directories(ppgu_sfz_parser_tests PRIVATE Source/Sampler)
    add_test(NAME ppgu_sfz_parser_tests COMMAND ppgu_sfz_parser_tests)

    add_executable(ppgu_pattern_scheduler_tests
        tests/PatternSchedulerTests.cpp
    )
    target_include_directories(ppgu_pattern_scheduler_tests PRIVATE Source/Performance)
    add_test(NAME ppgu_pattern_scheduler_tests COMMAND ppgu_pattern_scheduler_tests)

    add_executable(ppgu_chord_transition_tests
        tests/ChordTransitionPolicyTests.cpp
    )
    target_include_directories(ppgu_chord_transition_tests PRIVATE Source/Performance)
    add_test(NAME ppgu_chord_transition_tests COMMAND ppgu_chord_transition_tests)
endif()


# JUCE-backed SamplePool integration test with a generated, license-neutral WAV fixture.
if(BUILD_TESTING)
    add_executable(ppgu_sample_pool_tests
        Source/Sampler/SfzParser.cpp
        Source/Sampler/SamplePool.cpp
        Source/Sampler/SampleMetadataIndex.cpp
        Source/Sampler/WavPageReader.cpp
        Source/Sampler/PageCacheSlots.cpp
        Source/Sampler/SampleStreamingService.cpp
        Source/Sampler/StreamingVoiceReader.cpp
        tests/SamplePoolTests.cpp
    )
    target_include_directories(ppgu_sample_pool_tests PRIVATE Source/Sampler)
    target_link_libraries(ppgu_sample_pool_tests PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_basics
        juce::juce_core
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_sample_pool_tests COMMAND ppgu_sample_pool_tests)
endif()


# Fixed-slot page cache concurrency/state tests (no JUCE dependency).
if(BUILD_TESTING)
    add_executable(ppgu_page_cache_slots_tests
        Source/Sampler/PageCacheSlots.cpp
        tests/PageCacheSlotsTests.cpp
    )
    target_include_directories(ppgu_page_cache_slots_tests PRIVATE Source/Sampler)
    target_link_libraries(ppgu_page_cache_slots_tests PRIVATE
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_page_cache_slots_tests COMMAND ppgu_page_cache_slots_tests)
endif()

# Metadata-only sample indexing test. This scans WAV headers and must not
# decode or retain full audio buffers during a library inventory operation.
if(BUILD_TESTING)
    add_executable(ppgu_bounded_spsc_queue_tests
        tests/BoundedSpscQueueTests.cpp
    )
    target_include_directories(ppgu_bounded_spsc_queue_tests PRIVATE Source/Sampler)
    target_link_libraries(ppgu_bounded_spsc_queue_tests PRIVATE
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_bounded_spsc_queue_tests COMMAND ppgu_bounded_spsc_queue_tests)
endif()

if(BUILD_TESTING)
    add_executable(ppgu_sample_streaming_service_tests
        Source/Sampler/SampleMetadataIndex.cpp
        Source/Sampler/WavPageReader.cpp
        Source/Sampler/PageCacheSlots.cpp
        Source/Sampler/SampleStreamingService.cpp
        Source/Sampler/StreamingVoiceReader.cpp
        tests/SampleStreamingServiceTests.cpp
    )
    target_include_directories(ppgu_sample_streaming_service_tests PRIVATE Source/Sampler)
    target_link_libraries(ppgu_sample_streaming_service_tests PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_basics
        juce::juce_core
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_sample_streaming_service_tests COMMAND ppgu_sample_streaming_service_tests)
endif()

if(BUILD_TESTING)
    add_executable(ppgu_sample_metadata_index_tests
        Source/Sampler/SampleMetadataIndex.cpp
        Source/Sampler/WavPageReader.cpp
        tests/SampleMetadataIndexTests.cpp
    )
    target_include_directories(ppgu_sample_metadata_index_tests PRIVATE Source/Sampler)
    target_link_libraries(ppgu_sample_metadata_index_tests PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_basics
        juce::juce_core
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_sample_metadata_index_tests COMMAND ppgu_sample_metadata_index_tests)
endif()

# JUCE-backed runtime contract tests for every registry theme and safe fallback.
if(BUILD_TESTING)
    juce_add_console_app(ppgu_theme_manager_tests
        PRODUCT_NAME "PPGU Theme Manager Tests"
    )
    target_sources(ppgu_theme_manager_tests PRIVATE
        Source/UI/ThemeManager.cpp
        tests/ThemeManagerTests.cpp
    )
    target_include_directories(ppgu_theme_manager_tests PRIVATE Source/UI)
    target_link_libraries(ppgu_theme_manager_tests PRIVATE
        juce::juce_core
        juce::juce_graphics
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    juce_generate_juce_header(ppgu_theme_manager_tests)
    add_test(NAME ppgu_theme_manager_contract COMMAND ppgu_theme_manager_tests)
    set_tests_properties(ppgu_theme_manager_contract PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Data-contract validation for the 50 interchangeable UI skins.
# Python is optional for developers, but the validator is registered whenever
# a standard interpreter is available (including the Windows CI runner).
if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(
            NAME ppgu_skin_manifest_contract
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/ValidateSkinManifests.py
        )
        add_test(
            NAME ppgu_sound_asset_registry_contract
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/TestSoundAssetRegistry.py
        )
        add_test(
            NAME ppgu_skin_batch_generator_contract
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/TestSkinBatchGenerator.py
        )
        set_tests_properties(ppgu_sound_asset_registry_contract PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        set_tests_properties(ppgu_skin_batch_generator_contract PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endif()


# Regression for generated JUCE BinaryData.h in the processor integration test.
# Conflict-check: JUCE 7.0.12 accepts NAMESPACE (not HEADER_NAMESPACE); linking
# this target exports its generated include directory and JUCE_TARGET_HAS_BINARY_DATA
# to this test target. The fixture is intentionally tiny and license-neutral.
if(BUILD_TESTING)
    juce_add_binary_data(PPGUTestData
        HEADER_NAME "PPGUTestData.h"
        NAMESPACE PPGUTestData
        SOURCES tests/assets/ppgu_test_data.txt
    )
endif()

# Real AudioProcessor integration regression for the PATTERN MIDI path.
# Conflict-check: links the production processor target rather than duplicating
# a mock scheduler; fixture WAV/SFZ files are generated in the system temp dir.
if(BUILD_TESTING)
    juce_add_console_app(ppgu_processor_pattern_tests
        PRODUCT_NAME "PPGU Processor Pattern Tests"
    )
    target_sources(ppgu_processor_pattern_tests PRIVATE
        tests/ProcessorPatternIntegrationTests.cpp
    )
    target_include_directories(ppgu_processor_pattern_tests PRIVATE Source)
    target_link_libraries(ppgu_processor_pattern_tests PRIVATE
        PopPunkGuitarUltimate
        PPGUTestData
        juce::juce_audio_formats
        juce::juce_audio_utils
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    juce_generate_juce_header(ppgu_processor_pattern_tests)
    add_test(NAME ppgu_processor_pattern_integration COMMAND ppgu_processor_pattern_tests)
endif()
