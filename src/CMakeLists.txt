set(SRC_FILES
        main.cpp
        models/game/Game.cpp
        models/scenes/StartUpScene.cpp
        views/StartUpSceneView.cpp)

# Resource script files
set(RES_FILES
    ${PROJECT_SOURCE_DIR}/icon.rc
    ${PROJECT_SOURCE_DIR}/version.rc)

# Set executables output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Create executable project source files
add_executable(PacMan ${SRC_FILES} ${RES_FILES})

# Find third party dependency
set(IME_DIR "${PROJECT_SOURCE_DIR}/extlibs/IME/lib/cmake/IME")
set(IME_BIN_DIR "${PROJECT_SOURCE_DIR}/extlibs/IME/bin")
find_package(IME 2.1.0 REQUIRED)

# Link third party dependency to executable
target_link_libraries (PacMan PRIVATE ime)

# Add <project>/ as include directory
include_directories(${PROJECT_SOURCE_DIR}/)

# Copy runtime dependencies to the executable output folder
add_custom_command(TARGET PacMan PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${IME_BIN_DIR}/${CMAKE_BUILD_TYPE}/" $<TARGET_FILE_DIR:PacMan>
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${IME_BIN_DIR}/Runtime/" $<TARGET_FILE_DIR:PacMan>
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" $<TARGET_FILE_DIR:PacMan>/assets
)