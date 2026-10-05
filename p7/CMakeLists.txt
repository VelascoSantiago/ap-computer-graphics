get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
 
# SOIL2 viene como .c sueltos en la subcarpeta SOIL2/ -> se compilan junto al .cpp
file(GLOB SOIL2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/SOIL2/*.c")
 
foreach(SOURCE ${SOURCES})
    get_filename_component(SOURCE_NAME ${SOURCE} NAME_WE)
    add_executable(${TARGET_NAME}_${SOURCE_NAME} ${SOURCE} ${SOIL2_SOURCES})
    target_include_directories(${TARGET_NAME}_${SOURCE_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencias
        ${CMAKE_SOURCE_DIR}/dependencias/glm
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/SOIL2
    )
    target_link_libraries(${TARGET_NAME}_${SOURCE_NAME} PRIVATE OpenGL::GL glfw glad)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Model.h)
        find_package(assimp REQUIRED)
        target_link_libraries(${TARGET_NAME}_${SOURCE_NAME} PRIVATE assimp::assimp)
    endif()
endforeach()
 
# Shader/images/Models como symlinks: editás ahí y no hace falta re-copiar
foreach(RES Shader images Models)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${RES})
        file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/${RES}
             ${CMAKE_CURRENT_BINARY_DIR}/${RES} SYMBOLIC)
    endif()
endforeach()
