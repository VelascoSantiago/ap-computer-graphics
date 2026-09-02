get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB SOURCES "*.cpp")

add_executable(${TARGET_NAME} ${SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencias
    ${CMAKE_SOURCE_DIR}/dependencias/glm
)

target_link_libraries(${TARGET_NAME} PRIVATE OpenGL::GL glfw glad)

# Copia los shaders (.vs/.frag) junto al ejecutable
