file(GLOB SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(Server ${SERVER_SOURCES})
target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
  target_link_options(Server PRIVATE "/NODEFAULTLIB:MSVCRT")
endif()

target_link_libraries(Server PRIVATE Network Security Graphics)
