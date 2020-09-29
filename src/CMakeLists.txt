add_library(prefixinmem ${CMAKE_SOURCE_DIR}/libs/internal/prefixinmem/src/prefixinmem.cc)
target_include_directories(prefixinmem PUBLIC ${CMAKE_SOURCE_DIR}/libs/internal/prefixinmem/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/indexer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/processador)
