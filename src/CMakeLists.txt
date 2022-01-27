set(SRC_FILES
        main.cpp
        Common/ObjectReferenceKeeper.cpp
        AI/ActorStateFSM.cpp
        PathFinders/GhostGridMover.cpp
        PathFinders/PacManGridMover.cpp
        AI/ghost/GhostState.cpp
        AI/ghost/GIdleState.cpp
        AI/ghost/ScatterState.cpp
        AI/ghost/ChaseState.cpp
        AI/ghost/FrightenedState.cpp
        AI/ghost/EatenState.cpp
        GameObjects/Fruit.cpp
        GameObjects/Ghost.cpp
        GameObjects/Pellet.cpp
        GameObjects/PacMan.cpp
        GameObjects/Wall.cpp
        GameObjects/Sensor.cpp
        Game.cpp
        Scenes/StartUpScene.cpp
        Scenes/LoadingScene.cpp
        Scenes/MainMenuScene.cpp
        Scenes/PauseMenuScene.cpp
        Scenes/GameplayScene.cpp
        Scenes/GameOverScene.cpp
        Scenes/CollisionResponseRegisterer.cpp
        Scoreboard/Score.cpp
        Scoreboard/Scoreboard.cpp
        Grid2D/Grid.cpp
        Views/GameplaySceneView.cpp
        Views/StartUpSceneView.cpp
        Views/LoadingSceneView.cpp
        Views/MainMenuSceneView.cpp
        Views/PauseMenuSceneView.cpp
        Views/GameOverSceneView.cpp
        Animations/GhostAnimations.cpp
        Animations/GridAnimation.cpp
        Animations/PelletAnimations.cpp
        Animations/PacManAnimations.cpp
        Utils/ObjectCreator.cpp
        Utils/Utils.cpp)

# Resource script files
set(RES_FILES
    ${PROJECT_SOURCE_DIR}/icon.rc
    ${PROJECT_SOURCE_DIR}/version.rc)

# Set executables output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Create executable project source files
add_executable(PacMan ${SRC_FILES} ${RES_FILES})

# Find and link IME
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/extlibs")
find_package(ime 3.0.0 REQUIRED)
target_link_libraries (PacMan PRIVATE ime)

# Add <project>/ as include directory
include_directories(${PROJECT_SOURCE_DIR}/src)

# Copy runtime dependencies to the executable output folder
set(IME_BIN_DIR "${PROJECT_SOURCE_DIR}/extlibs/IME-${ime_VERSION}/bin")

add_custom_command(TARGET PacMan PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${IME_BIN_DIR}/${CMAKE_BUILD_TYPE}/" $<TARGET_FILE_DIR:PacMan>
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${IME_BIN_DIR}/Runtime/" $<TARGET_FILE_DIR:PacMan>
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res" $<TARGET_FILE_DIR:PacMan>/res
)