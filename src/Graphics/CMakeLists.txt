add_library(Graphics STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics.cpp
)

# === IMGUI ===
set(IMGUI_DIR "C:/sdk/imgui-master")
set(IMGUI_BACKENDS_DIR "${IMGUI_DIR}/backends")

# === GLAD ===
# add_library(glad STATIC ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c)
# target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/external/glad/include)


add_library(glad STATIC "C:/sdk/glad/include/glad.c")
target_include_directories(glad PUBLIC "C:/sdk/glad/include")


# === PREBUILT GLFW ===
set(GLFW_ROOT "C:/sdk/glfw")
set(GLFW_INCLUDE_DIR "${GLFW_ROOT}/include")
set(GLFW_LIBRARY_DIR "${GLFW_ROOT}/build/src")
set(GLFW_LIBRARY "${GLFW_LIBRARY_DIR}/glfw3.lib")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS_DIR}
    ${GLFW_INCLUDE_DIR}
)




target_include_directories(Graphics PUBLIC
    ${GLFW_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_directories(Graphics PUBLIC ${GLFW_LIBRARY_DIR})
target_link_libraries(Graphics PUBLIC imgui glad ${GLFW_LIBRARY} opengl32)
