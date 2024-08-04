#include "game.h"


Game::Game()
    : window(WINDO_WIDTH, WINDOW_HEIGHT, "OpenGL Window"),
      player(new Camera(glm::vec3(0.0f, 128.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f), &chunkManager),
      seed(420),
      chunkManager(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH, VIEW_DISTANCE),
      lastUpdatePosition(0.0f, 128.0f, 0.0f),
      lastFrame(0.0),
      frameCount(0),
      fps(0.0f),
      lastFPSPrintTime(0.0)
{
    chunkManager.init(seed);
    initialize();
}

void Game::initialize() {
    InputListener::setPlayer(&player);
    InputListener::setWindow(&window);
    InputListener::setChunkManager(&chunkManager);
    glfwSetKeyCallback(window.getGLFWwindow(), InputListener::keyCallback);
    glfwSetCursorPosCallback(window.getGLFWwindow(), InputListener::cursorPositionCallback);
    glfwSetScrollCallback(window.getGLFWwindow(), InputListener::scrollCallback);
    glfwSetMouseButtonCallback(window.getGLFWwindow(), InputListener::mouseButtonCallback);
    chunkManager.updatePlayerPosition(player.getPosition());
    renderer.setCamera(player.getCamera());
    renderer.initShaders();
    renderer.loadTexture("../assets/textures/atlas.png");
    renderer.setSkyboxData(skyBox.GetVertices());
    gui.Init(window.getGLFWwindow()),
    glfwSetWindowUserPointer(window.getGLFWwindow(), &gui);
}

void Game::run() {
    lastFrame = glfwGetTime();
    lastFPSPrintTime = glfwGetTime();
    while (!window.shouldClose()) {
        double currentFrame = glfwGetTime();
        double deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        window.pollEvents();
        InputListener::update();
        player.update(deltaTime);
        glm::vec3 currentPlayerPos = player.getPosition();


        if (glm::distance(currentPlayerPos, lastUpdatePosition) > CHUNK_WIDTH / 2.0f) {
            chunkManager.updatePlayerPosition(currentPlayerPos);
            lastUpdatePosition = currentPlayerPos;
        }

        std::tuple<glm::ivec2, std::vector<float>, std::vector<unsigned int>, std::vector<float>, std::vector<unsigned int>> item;
        chunkManager.updateChunks();
        while (chunkManager.getRenderQueue().tryPop(item)) {
            const auto& [chunkPos, solidVertices, solidIndices, waterVertices, waterIndices] = item;
            if ((!solidVertices.empty() && !solidIndices.empty()) || (!waterVertices.empty() && !waterIndices.empty())) {
                renderer.addChunk(chunkPos, solidVertices, solidIndices, waterVertices, waterIndices);
            } else {
                renderer.removeChunk(chunkPos);
            }
        }

        renderer.processChunkUpdates();
        renderer.setLightDir(gui.getLightDirection());

        frameCount++;
        if (currentFrame - lastFPSPrintTime >= 1.0) {
            fps = frameCount / (currentFrame - lastFPSPrintTime);
            frameCount = 0;
            lastFPSPrintTime = currentFrame;
        }

        gui.newFrame();
        gui.displayInfo(fps, player.getPosition(), VIEW_DISTANCE, chunkManager.getLoadedChunksCount());
        renderer.draw();
        gui.render();
        gui.drawCrosshair();
        window.swapBuffers();
    }
}
