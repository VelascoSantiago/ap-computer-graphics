get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
foreach(SOURCE ${SOURCES})
    get_filename_component(SOURCE_NAME ${SOURCE} NAME_WE)
    add_executable(${TARGET_NAME}_${SOURCE_NAME} ${SOURCE})
    target_include_directories(${TARGET_NAME}_${SOURCE_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencias
        ${CMAKE_SOURCE_DIR}/dependencias/glm
    )
    target_link_libraries(${TARGET_NAME}_${SOURCE_NAME} PRIVATE OpenGL::GL glfw glad)
endforeach()
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})