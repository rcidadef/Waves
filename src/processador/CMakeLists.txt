set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/Index.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/PostingList.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/PreProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/valstringuniao.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/ConfigFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/auxFunctions.cc
)

add_library( query_processor ${SRC})
target_include_directories(query_processor PUBLIC
    ${CMAKE_SOURCE_DIR}/include/indexer
    ${CMAKE_SOURCE_DIR}/include/processador
    ${CMAKE_SOURCE_DIR}/include/utils)

add_executable(queryProcessor ${CMAKE_CURRENT_SOURCE_DIR}/run/queryProcessor.cpp)
target_link_libraries(queryProcessor query_processor)

add_executable(gerarIndice ${CMAKE_CURRENT_SOURCE_DIR}/run/geraR_Indice.cc)
target_link_libraries(gerarIndice query_processor)

include ( ${CMAKE_SOURCE_DIR}/cmake/install.cmake )
