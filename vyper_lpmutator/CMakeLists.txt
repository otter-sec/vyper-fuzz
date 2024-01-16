cmake_minimum_required(VERSION 3.28)
project(vyper_lpmutator)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    set(PROTOBUF_LIBRARIES protobufd)
ELSE()
    set(PROTOBUF_LIBRARIES protobuf)
ENDIF()

# Seems like protobuf dropped these dependecies? (good news if true)
# list(APPEND PROTOBUF_LIBRARIES
    # absl_bad_any_cast_impl
    # absl_bad_optional_access
    # absl_bad_variant_access
    # absl_base
    # absl_city
    # absl_civil_time
    # absl_cord
    # absl_cord_internal
    # absl_cordz_functions
    # absl_cordz_handle
    # absl_cordz_info
    # absl_cordz_sample_token
    # absl_crc_cord_state
    # absl_crc_cpu_detect
    # absl_crc_internal
    # absl_crc32c
    # absl_debugging_internal
    # absl_demangle_internal
    # absl_die_if_null
    # absl_examine_stack
    # absl_exponential_biased
    # absl_failure_signal_handler
    # absl_flags
    # absl_flags_commandlineflag
    # absl_flags_commandlineflag_internal
    # absl_flags_config
    # absl_flags_internal
    # absl_flags_marshalling
    # absl_flags_parse
    # absl_flags_private_handle_accessor
    # absl_flags_program_name
    # absl_flags_reflection
    # absl_flags_usage
    # absl_flags_usage_internal
    # absl_graphcycles_internal
    # absl_hash
    # absl_hashtablez_sampler
    # absl_int128
    # absl_leak_check
    # absl_log_entry
    # absl_log_flags
    # absl_log_globals
    # absl_log_initialize
    # absl_log_internal_check_op
    # absl_log_internal_conditions
    # absl_log_internal_format
    # absl_log_internal_globals
    # absl_log_internal_log_sink_set
    # absl_log_internal_message
    # absl_log_internal_nullguard
    # absl_log_internal_proto
    # absl_log_severity
    # absl_log_sink
    # absl_low_level_hash
    # absl_malloc_internal
    # absl_periodic_sampler
    # absl_random_distributions
    # absl_random_internal_distribution_test_util
    # absl_random_internal_platform
    # absl_random_internal_pool_urbg
    # absl_random_internal_randen
    # absl_random_internal_randen_hwaes
    # absl_random_internal_randen_hwaes_impl
    # absl_random_internal_randen_slow
    # absl_random_internal_seed_material
    # absl_random_seed_gen_exception
    # absl_random_seed_sequences
    # absl_raw_hash_set
    # absl_raw_logging_internal
    # absl_scoped_set_env
    # absl_spinlock_wait
    # absl_stacktrace
    # absl_status
    # absl_statusor
    # absl_str_format_internal
    # absl_strerror
    # absl_strings
    # absl_strings_internal
    # absl_symbolize
    # absl_synchronization
    # absl_throw_delegate
    # absl_time
    # absl_time_zone
    # utf8_validity
# )

set(CMAKE_LINK_GROUP_USING_cross_refs_SUPPORTED TRUE)
set(CMAKE_LINK_GROUP_USING_cross_refs
    "LINKER:--start-group"
    "LINKER:--end-group"
)

set(Protobuf_LIBRARIES "$<LINK_GROUP:cross_refs,${PROTOBUF_LIBRARIES}>")

add_subdirectory(libprotobuf-mutator)
add_subdirectory(fmt)
add_subdirectory(pybind11)
add_subdirectory(eigen)

# Mutator library
add_library(${PROJECT_NAME}
    src/generate.cc
    src/proto/vyper.pb.cc
    libprotobuf-mutator/src/binary_format.cc
    libprotobuf-mutator/src/mutator.cc
    libprotobuf-mutator/src/text_format.cc
    libprotobuf-mutator/src/utf8_fix.cc
)

target_include_directories(${PROJECT_NAME}
    PUBLIC fmt/include libprotobuf-mutator ${Protobuf_INCLUDE_DIR}
)

# Python wrapper
pybind11_add_module(${PROJECT_NAME}_wrapper src/pywrap.cc)

target_compile_definitions(${PROJECT_NAME}_wrapper PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
target_include_directories(${PROJECT_NAME}_wrapper PRIVATE libprotobuf-mutator ${PYBIND11_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME}_wrapper
    PUBLIC ${Protobuf_LIBPATH}
)
target_link_libraries(${PROJECT_NAME}_wrapper PRIVATE
  ${PROJECT_NAME}
  ${Protobuf_LIBRARIES}
  fmt
  Eigen3::Eigen
)

# Test program
add_executable(${PROJECT_NAME}_test
    src/test.cc
)

target_include_directories(${PROJECT_NAME}_test
    PUBLIC libprotobuf-mutator ${Protobuf_INCLUDE_DIR}
)

target_link_directories(${PROJECT_NAME}_test
    PUBLIC ${Protobuf_LIBPATH}
)

target_link_libraries(${PROJECT_NAME}_test
    ${PROJECT_NAME}
    ${Protobuf_LIBRARIES}
    fmt
)