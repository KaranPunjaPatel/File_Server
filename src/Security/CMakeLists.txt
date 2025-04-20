# # Add the static library
# add_library(Security STATIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/asymmetricKey.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/sessionToken.cpp
# )

# # Include the headers
# target_include_directories(Security PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# # Set OpenSSL root directory before find_package
# # set(OPENSSL_ROOT_DIR "C:/sdk/openssl")
# # set(OPENSSL_USE_STATIC_LIBS TRUE)

# # Find OpenSSL
# # find_package(OpenSSL REQUIRED)

# include_directories("C:/sdk/openssl/include")
# link_directories("C:/sdk/openssl")

# # Link OpenSSL (Crypto)
# target_link_libraries(Security PRIVATE C:/sdk/openssl/libssl_static.lib 
#         C:/sdk/openssl/libcrypto_static.lib)

# Create the Network static library
file(GLOB SECURITY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/asymmetricKey.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sessionToken.cpp"
)

file(GLOB SECURITY_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/asymmetricKey.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/securityClass.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sessionToken.h"
)

# Add the static library
add_library(Security STATIC ${SECURITY_SOURCES} ${SECURITY_HEADERS})


# Include the headers
target_include_directories(Security PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

# Link OpenSSL
target_include_directories(Security PUBLIC "C:/sdk/openssl/include")
target_link_directories(Security PRIVATE "C:/sdk/openssl")

target_link_libraries(Security PUBLIC
    C:/sdk/openssl/libssl_static.lib 
    C:/sdk/openssl/libcrypto_static.lib
    crypt32
)
