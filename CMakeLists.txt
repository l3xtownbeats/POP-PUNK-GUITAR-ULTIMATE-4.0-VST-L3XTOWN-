cmake_minimum_required(VERSION 3.22)
project(PopPunkGuitarUltimate VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Pin JUCE 7 as required by v33 FINAL. FetchContent keeps the dependency reproducible.
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# JUCE owns the VST3 bundle install step; this avoids copying the shared-code
# target file instead of the actual .vst3 bundle.
if(WIN32)
    set(PPGU_FL_VST3_DIR "$ENV{CommonProgramFiles}/VST3" CACHE PATH
        "Destination for the post-build VST3 bundle")
endif()

juce_add_binary_data(PPGUPresetData SOURCES
    presets/factory_bank_300.json
    Resources/Artwork/APPROVED_STYLE_A.png
    Resources/Artwork/APPROVED_STYLE_B.png
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

target_sources(PopPunkGuitarUltimate PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Sampler/SampleLibraryImporter.cpp
    Source/Sampler/MappingEditorModel.cpp
    Source/Sampler/SampleLibraryImporter.h
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
    JUCE_USE_FLAC=1
    JUCE_USE_OGGVORBIS=1
    JUCE_USE_MP3AUDIOFORMAT=1
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


# Dependency-free SFZ parser regression tests.
include(CTest)
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
endif()



# End-to-end scheduler dispatch into the real streaming sampler using a generated WAV/SFZ fixture.
if(BUILD_TESTING)
    add_executable(ppgu_pattern_sampler_integration_tests
        Source/Sampler/SfzParser.cpp
        Source/Sampler/SamplePool.cpp
        Source/Sampler/SampleMetadataIndex.cpp
        Source/Sampler/WavPageReader.cpp
        Source/Sampler/PageCacheSlots.cpp
        Source/Sampler/SampleStreamingService.cpp
        Source/Sampler/StreamingVoiceReader.cpp
        tests/PatternSamplerIntegrationTests.cpp
    )
    target_include_directories(ppgu_pattern_sampler_integration_tests PRIVATE Source)
    target_link_libraries(ppgu_pattern_sampler_integration_tests PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_basics
        juce::juce_core
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_pattern_sampler_integration_tests COMMAND ppgu_pattern_sampler_integration_tests)
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


# M4 AutoChords/Fingering and M5 Style DNA regression suites.
if(BUILD_TESTING)
    add_executable(ppgu_auto_chords_tests tests/AutoChordsTests.cpp)
    target_include_directories(ppgu_auto_chords_tests PRIVATE Source/Performance)
    add_test(NAME ppgu_auto_chords_tests COMMAND ppgu_auto_chords_tests)

    add_executable(ppgu_style_engine_tests tests/StyleEngineTests.cpp)
    target_include_directories(ppgu_style_engine_tests PRIVATE Source/Performance)
    add_test(NAME ppgu_style_engine_tests COMMAND ppgu_style_engine_tests)
endif()


# Audio library import regression tests: WAV + AIFF conversion, manifest and SFZ mapping.
if(BUILD_TESTING)
    add_executable(ppgu_sample_library_importer_tests
        Source/Sampler/SampleLibraryImporter.cpp
        Source/Sampler/SfzParser.cpp
        tests/SampleLibraryImporterTests.cpp
    )
    target_include_directories(ppgu_sample_library_importer_tests PRIVATE Source/Sampler)
    target_compile_definitions(ppgu_sample_library_importer_tests PRIVATE
        JUCE_USE_FLAC=1
        JUCE_USE_OGGVORBIS=1
        JUCE_USE_MP3AUDIOFORMAT=1
    )
    target_link_libraries(ppgu_sample_library_importer_tests PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_basics
        juce::juce_core
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_sample_library_importer_tests COMMAND ppgu_sample_library_importer_tests)
endif()

# Editable key/velocity mapping and round-robin selection regression tests.
if(BUILD_TESTING)
    add_executable(ppgu_mapping_editor_model_tests
        Source/Sampler/MappingEditorModel.cpp
        tests/MappingEditorModelTests.cpp
    )
    target_include_directories(ppgu_mapping_editor_model_tests PRIVATE Source/Sampler)
    add_test(NAME ppgu_mapping_editor_model_tests COMMAND ppgu_mapping_editor_model_tests)
endif()


# Release gate: the two bundled factory sample profiles must load mapped
# samples and produce non-silent audio through the production streaming engine.
if(BUILD_TESTING)
    add_executable(ppgu_factory_soundbank_tests
        Source/Sampler/SfzParser.cpp
        Source/Sampler/SamplePool.cpp
        Source/Sampler/SampleMetadataIndex.cpp
        Source/Sampler/WavPageReader.cpp
        Source/Sampler/PageCacheSlots.cpp
        Source/Sampler/SampleStreamingService.cpp
        Source/Sampler/StreamingVoiceReader.cpp
        tests/FactorySoundbankTests.cpp
    )
    target_include_directories(ppgu_factory_soundbank_tests PRIVATE Source)
    target_link_libraries(ppgu_factory_soundbank_tests PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_basics
        juce::juce_core
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
    add_test(NAME ppgu_factory_soundbank_audio_smoke
        COMMAND ppgu_factory_soundbank_tests
            "${CMAKE_SOURCE_DIR}/Samples/FreePats-Direct-DI/EGuitarFSBS-direct bridge 20220911.sfz"
            "${CMAKE_SOURCE_DIR}/Samples/Karoryfer-Shinyguitar/Samples/acoustic.sfz")
endif()
