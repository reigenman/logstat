set(_target logstat)

add_library(${_target})

set(HEADERS
    ../include/logstat/thread_pool.hpp
    ../include/logstat/aggregator.hpp
    ../include/logstat/prop_set.hpp
    ../include/logstat/stats.hpp
    ../include/logstat/errors.hpp
)

set(SOURCES
    thread_pool.cpp
    aggregator.cpp
    jsonlib.hpp
)

target_sources(${_target} PRIVATE ${SOURCES} ${HEADERS})

target_include_directories(${_target} PUBLIC ../include)

target_compile_features(${_target} PUBLIC cxx_std_17)
