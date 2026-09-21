get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
 
# Assimp se instala a nivel sistema (sudo apt install libassimp-dev),
# a diferencia de SOIL2 que viene como código fuente dentro de esta carpeta.
find_package(assimp REQUIRED)
 
# Los .cpp de esta carpeta -> uno o más targets (igual que el template genérico)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
 
# SOIL2 viene como .c sueltos en la subcarpeta SOIL2/ -> se compilan junto al .cpp,
# no como librería aparte
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
    target_link_libraries(${TARGET_NAME}_${SOURCE_NAME} PRIVATE
        OpenGL::GL glfw glad assimp::assimp
    )
endforeach()
 
# Se copian junto al ejecutable: los shaders y también los modelos 3D (.obj + texturas)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Models DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
