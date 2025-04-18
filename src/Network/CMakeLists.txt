# Create the Network static library

add_definitions(-D_WIN32_WINNT=0x0A00)


file(GLOB NETWORK_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/network.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkClient.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkConnection.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkContext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkDeque.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkServer.cpp"
)

file(GLOB NETWORK_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/network.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkClass.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkClient.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkConnection.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkContext.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkDeque.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/networkServer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/packet.h"
)

# Add the static library
add_library(Network STATIC ${NETWORK_SOURCES} ${NETWORK_HEADERS})

target_include_directories(Network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "C:/sdk/boost_1_88_0")

# include_directories("C:/sdk/openssl/include")
# link_directories("C:/sdk/openssl")

# Boost settings
set(BOOST_ROOT "C:/sdk/boost_1_88_0")
# set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
# set(Boost_USE_STATIC_RUNTIME OFF) 

# find_package(Boost 1.88.0 REQUIRED COMPONENTS filesystem)
find_package(Boost 1.88.0 REQUIRED)

# include_directories("C:/sdk/boost_1_88_0")


if(Boost_FOUND)
    target_include_directories(Network PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(Network PUBLIC ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "Boost not found!")
endif()

# Link Security
target_link_libraries(Network PUBLIC Security)