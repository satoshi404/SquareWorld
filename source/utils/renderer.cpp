
#include "renderer.h"
#include "window.h"
#include <stdexcept>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <imgui.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <algorithm>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>

static const char* _defaultVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 fragPos;
out vec3 normal;
void main() {
    fragPos = vec3(model * vec4(aPos, 1.0));
    normal = aPos; // Simplified, assumes vertex normals are same as positions
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

static const char* _defaultFragmentShader = R"(
#version 330 core
in vec3 fragPos;
in vec3 normal;
out vec4 FragColor;
uniform vec4 color;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform vec4 lightColor;
uniform float lightCutoff;
uniform float lightIntensity;
void main() {
    vec3 norm = normalize(normal);
    vec3 lightDirNorm = normalize(lightDir);
    float theta = dot(-lightDirNorm, normalize(fragPos - lightPos));
    float cutoff = cos(radians(lightCutoff));
    float lightEffect = lightIntensity * max(theta > cutoff ? theta : 0.0, 0.0);
    FragColor = color * lightColor * lightEffect;
}
)";

static const char* defaultVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

static const char* defaultFragmentShader = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 color;
void main() {
    FragColor = color;
}
)";

Renderer::Renderer(const char* vertexShaderSource, const char* fragmentShaderSource) {
    shaderProgram = 0;
    camera = nullptr;
    selectedShape = nullptr;
    selectedSpotlight = nullptr;
    lastFrameTime = std::chrono::high_resolution_clock::now();
    window = nullptr;
    type = WINDOW_MAIN;
    createShaderProgram(
        vertexShaderSource ? vertexShaderSource : defaultVertexShader,
        fragmentShaderSource ? fragmentShaderSource : defaultFragmentShader
    );
}

Renderer::~Renderer() {
    for (Shape* shape : shapes) {
        delete shape;
    }
    shapes.clear();
    for (Spotlight* light : spotlights) {
        delete light;
    }
    spotlights.clear();
    glDeleteProgram(shaderProgram);
    delete camera;
}

void Renderer::compileShader(GLenum type, const char* source, GLuint& shader) {
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
    }
}

void Renderer::createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader, fragmentShader;
    compileShader(GL_VERTEX_SHADER, vertexSource, vertexShader);
    compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        throw std::runtime_error("Shader program linking failed: " + std::string(infoLog));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Renderer::init() {
    if (!camera) {
        camera = new Camera();
        camera->setAspect(1.0f);
    }

    for (Shape* shape : shapes) {
        try {
            shape->init();
        } catch (const std::exception& e) {
            std::cerr << "Shape init failed: " << e.what() << std::endl;
        }
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}


void Renderer::render() {
    // Clear color and depth buffers
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set viewport
    if (window) {
        int width, height;
        SDL_GetWindowSize(window->GetWindow(), &width, &height);
        glViewport(0, 0, width, height);
        updateCameraAspect(static_cast<float>(width) / height);
    }

    glUseProgram(shaderProgram);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    for (Shape* shape : shapes) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, shape->getPosition());
        model = glm::rotate(model, glm::radians(shape->getRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(shape->getRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(shape->getRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, shape->getScale());
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, &shape->getColor()[0]);
        shape->draw(shaderProgram);
    }
}

void Renderer::loadFromJSON(const nlohmann::json& json) {
    if (json.contains("shapes") && json["shapes"].is_array()) {
        for (const auto& shapeJson : json["shapes"]) {
            if (shapeJson.contains("type") && shapeJson["type"].is_string()) {
                std::string shapeType = shapeJson["type"].get<std::string>();
                Shape* shape = nullptr;
                if (shapeType == "Mesh" && shapeJson.contains("objPath") && shapeJson["objPath"].is_string()) {
                    shape = new Mesh(shapeJson["objPath"].get<std::string>());
                } else {
                    shape = Shape::createShape(shapeType);
                }
                if (shape) {
                    try {
                        if (shapeJson.contains("position") && shapeJson["position"].is_array()) {
                            auto pos = shapeJson["position"].get<std::vector<float>>();
                            if (pos.size() == 3) shape->setPosition({pos[0], pos[1], pos[2]});
                        }
                        if (shapeJson.contains("scale") && shapeJson["scale"].is_array()) {
                            auto scl = shapeJson["scale"].get<std::vector<float>>();
                            if (scl.size() == 3) shape->setScale({scl[0], scl[1], scl[2]});
                        }
                        if (shapeJson.contains("rotation") && shapeJson["rotation"].is_array()) {
                            auto rot = shapeJson["rotation"].get<std::vector<float>>();
                            if (rot.size() == 3) shape->setRotation({rot[0], rot[1], rot[2]});
                        }
                        if (shapeJson.contains("color") && shapeJson["color"].is_array()) {
                            auto col = shapeJson["color"].get<std::vector<float>>();
                            if (col.size() == 4) shape->setColor({col[0], col[1], col[2], col[3]});
                        }
                        shape->init();
                        shapes.push_back(shape);
                    } catch (const std::exception& e) {
                        std::cerr << "Failed to initialize shape: " << e.what() << std::endl;
                        delete shape;
                    }
                }
            }
        }
    }
    if (json.contains("spotlights") && json["spotlights"].is_array()) {
        for (const auto& lightJson : json["spotlights"]) {
            if (lightJson.contains("name") && lightJson["name"].is_string()) {
                Spotlight* light = new Spotlight(lightJson["name"].get<std::string>());
                if (lightJson.contains("position") && lightJson["position"].is_array()) {
                    auto pos = lightJson["position"].get<std::vector<float>>();
                    if (pos.size() == 3) light->setPosition({pos[0], pos[1], pos[2]});
                }
                if (lightJson.contains("direction") && lightJson["direction"].is_array()) {
                    auto dir = lightJson["direction"].get<std::vector<float>>();
                    if (dir.size() == 3) light->setDirection({dir[0], dir[1], dir[2]});
                }
                if (lightJson.contains("color") && lightJson["color"].is_array()) {
                    auto col = lightJson["color"].get<std::vector<float>>();
                    if (col.size() == 4) light->setColor({col[0], col[1], col[2], col[3]});
                }
                if (lightJson.contains("cutoff") && lightJson["cutoff"].is_number_float()) {
                    light->setCutoff(lightJson["cutoff"].get<float>());
                }
                if (lightJson.contains("intensity") && lightJson["intensity"].is_number_float()) {
                    light->setIntensity(lightJson["intensity"].get<float>());
                }
                spotlights.push_back(light);
            }
        }
    }
    if (json.contains("camera") && json["camera"].is_object()) {
        if (!camera) camera = new Camera();
        auto camJson = json["camera"];
        if (camJson.contains("aspect") && camJson["aspect"].is_number_float()) {
            camera->setAspect(camJson["aspect"].get<float>());
        }
        if (camJson.contains("position") && camJson["position"].is_array()) {
            auto pos = camJson["position"].get<std::vector<float>>();
            if (pos.size() == 3) camera->setPosition({pos[0], pos[1], pos[2]});
        }
        if (camJson.contains("rotation") && camJson["rotation"].is_array()) {
            auto rot = camJson["rotation"].get<std::vector<float>>();
            if (rot.size() == 3) camera->setRotation({rot[0], rot[1], rot[2]});
        }
        if (camJson.contains("fov") && camJson["fov"].is_number_float()) {
            camera->setFov(camJson["fov"].get<float>());
        }
    }
}

void Renderer::setupImGui() {
    // Empty for now
}

void Renderer::renderImGui(bool isDebugWindow, float fps, std::vector<Renderer*>& allRenderers) {
    if (window) {
        if (SDL_GL_MakeCurrent(window->GetWindow(), window->GetGLContext()) < 0) {
            std::cerr << "Failed to make GL context current: " << SDL_GetError() << std::endl;
            return;
        }
    } else {
        std::cerr << "Renderer has no associated window" << std::endl;
        return;
    }

    ImGui::Begin(isDebugWindow ? "Debug Window" : "Renderer Controls");
    ImGui::Text("FPS: %.1f", fps);

    // Shape selection combo
    static int currentShape = 0;
    const char* shapeTypes[] = {"Cube", "Circle", "Triangle", "Mesh"};
    ImGui::Combo("Shape Type", &currentShape, shapeTypes, IM_ARRAYSIZE(shapeTypes));

    // Add shape button
    static char objPath[256] = "";
    if (currentShape == 3) { // Mesh selected
        ImGui::InputText("OBJ Path", objPath, IM_ARRAYSIZE(objPath));
    }
    if (ImGui::Button("Add Shape")) {
        std::string shapeType = shapeTypes[currentShape];
        std::string path = (currentShape == 3) ? objPath : "";
        std::cout << "Adding " << shapeType << (path.empty() ? "" : " with path " + path) << std::endl;

        std::thread([this, shapeType, path]() {
            Shape* newShape = (shapeType == "Mesh") ? new Mesh(path) : Shape::createShape(shapeType);
            if (newShape) {
                std::lock_guard<std::mutex> lock(shapeMutex);
                pendingShapes.push(newShape);
            } else {
                std::cerr << "Failed to create shape: " << shapeType << std::endl;
            }
        }).detach();
    }

    // Add spotlight button
    static char lightName[256] = "Spotlight";
    ImGui::InputText("Spotlight Name", lightName, IM_ARRAYSIZE(lightName));
    if (ImGui::Button("Add Spotlight")) {
        spotlights.push_back(new Spotlight(lightName));
    }

    // Delete selected shape
    if (selectedShape && ImGui::Button("Delete Selected Shape")) {
        auto it = std::find(shapes.begin(), shapes.end(), selectedShape);
        if (it != shapes.end()) {
            delete *it;
            shapes.erase(it);
            selectedShape = nullptr;
        }
    }

    // Delete selected spotlight
    if (selectedSpotlight && ImGui::Button("Delete Selected Spotlight")) {
        auto it = std::find(spotlights.begin(), spotlights.end(), selectedSpotlight);
        if (it != spotlights.end()) {
            delete *it;
            spotlights.erase(it);
            selectedSpotlight = nullptr;
        }
    }

    // Reset scene
    if (ImGui::Button("Reset Scene")) {
        for (Shape* shape : shapes) {
            delete shape;
        }
        shapes.clear();
        for (Spotlight* light : spotlights) {
            delete light;
        }
        spotlights.clear();
        selectedShape = nullptr;
        selectedSpotlight = nullptr;
    }

    // Process pending shapes
    {
        std::lock_guard<std::mutex> lock(shapeMutex);
        while (!pendingShapes.empty()) {
            Shape* newShape = pendingShapes.front();
            pendingShapes.pop();
            try {
                newShape->init();
                shapes.push_back(newShape);
            } catch (const std::exception& e) {
                std::cerr << "Failed to initialize shape: " << e.what() << std::endl;
                delete newShape;
            }
        }
    }

    // Shape list
    if (ImGui::CollapsingHeader("Shapes")) {
        for (size_t i = 0; i < shapes.size(); ++i) {
            ImGui::PushID(i);
            if (ImGui::Selectable(shapes[i]->getType().c_str(), selectedShape == shapes[i])) {
                selectedShape = shapes[i];
                selectedSpotlight = nullptr;
            }
            ImGui::PopID();
        }
    }

    // Spotlight list
    if (ImGui::CollapsingHeader("Spotlights")) {
        for (size_t i = 0; i < spotlights.size(); ++i) {
            ImGui::PushID(i + shapes.size());
            if (ImGui::Selectable(spotlights[i]->getName().c_str(), selectedSpotlight == spotlights[i])) {
                selectedSpotlight = spotlights[i];
                selectedShape = nullptr;
            }
            ImGui::PopID();
        }
    }

    // Shape properties panel
    if (selectedShape) {
        ImGui::Begin("Shape Properties");
        ImGui::Text("Selected Shape: %s", selectedShape->getType().c_str());
        if (selectedShape->getType() == "Mesh") {
            ImGui::Text("OBJ Path: %s", dynamic_cast<Mesh*>(selectedShape)->getObjPath().c_str());
        }

        glm::vec3 pos = selectedShape->getPosition();
        if (ImGui::DragFloat3("Position", &pos[0], 0.1f)) {
            selectedShape->setPosition(pos);
        }

        glm::vec3 scl = selectedShape->getScale();
        if (ImGui::DragFloat3("Scale", &scl[0], 0.1f, 0.1f)) {
            selectedShape->setScale(scl);
        }

        glm::vec3 rot = selectedShape->getRotation();
        if (ImGui::DragFloat3("Rotation", &rot[0], 1.0f)) {
            selectedShape->setRotation(rot);
        }

        glm::vec4 col = selectedShape->getColor();
        if (ImGui::ColorEdit4("Color", &col[0])) {
            selectedShape->setColor(col);
        }

        ImGui::End();
    }

    // Spotlight properties panel
    if (selectedSpotlight) {
        ImGui::Begin("Spotlight Properties");
        ImGui::Text("Selected Spotlight: %s", selectedSpotlight->getName().c_str());

        glm::vec3 pos = selectedSpotlight->getPosition();
        if (ImGui::DragFloat3("Position", &pos[0], 0.1f)) {
            selectedSpotlight->setPosition(pos);
        }

        glm::vec3 dir = selectedSpotlight->getDirection();
        if (ImGui::DragFloat3("Direction", &dir[0], 0.1f)) {
            selectedSpotlight->setDirection(dir);
        }

        glm::vec4 col = selectedSpotlight->getColor();
        if (ImGui::ColorEdit4("Color", &col[0])) {
            selectedSpotlight->setColor(col);
        }

        float cutoff = selectedSpotlight->getCutoff();
        if (ImGui::SliderFloat("Cutoff Angle", &cutoff, 0.0f, 90.0f)) {
            selectedSpotlight->setCutoff(cutoff);
        }

        float intensity = selectedSpotlight->getIntensity();
        if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f)) {
            selectedSpotlight->setIntensity(intensity);
        }

        ImGui::End();
    }

    // Camera properties panel
    if (camera) {
        ImGui::Begin("Camera Properties");
        ImGui::Text("Camera Settings");

        glm::vec3 camPos = camera->getPosition();
        if (ImGui::DragFloat3("Camera Position", &camPos[0], 0.1f)) {
            camera->setPosition(camPos);
        }

        glm::vec3 camRot = camera->getRotation();
        if (ImGui::DragFloat3("Camera Rotation", &camRot[0], 1.0f)) {
            camera->setRotation(camRot);
        }

        float camFov = camera->getFov();
        if (ImGui::SliderFloat("Field of View", &camFov, 10.0f, 120.0f)) {
            camera->setFov(camFov);
        }

        ImGui::End();
    }

    ImGui::End();
}

float Renderer::getFPS() const {
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;
    return deltaTime > 0 ? 1.0f / deltaTime : 0.0f;
}

void Renderer::updateCameraAspect(float aspect) {
    if (camera) {
        camera->setAspect(aspect);
    }
}
