add_executable(aggregator main.cpp app.hpp app.cpp)

target_compile_features(aggregator PRIVATE cxx_std_17)

target_link_libraries(aggregator PRIVATE logstat)
