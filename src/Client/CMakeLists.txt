file(GLOB CLIENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(Client ${CLIENT_SOURCES})
target_include_directories(Client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Client PRIVATE Network Graphics)
