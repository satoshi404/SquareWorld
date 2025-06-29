#include "renderer.h"
#include <stdexcept>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glx.h>
#include <fstream>


static const char* defaultVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 fragColor;
out vec2 fragTexCoord;
void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    fragColor = color;
    fragTexCoord = texCoord;
}
)";

static const char* defaultFragmentShader = R"(
#version 330 core
in vec3 fragColor;
in vec2 fragTexCoord;
out vec4 outColor;
uniform sampler2D texture0;
uniform bool useTexture;
void main() {
    if (useTexture) {
        outColor = texture(texture0, fragTexCoord);
    } else {
        outColor = vec4(fragColor, 1.0);
    }
}
)";

void Renderer::compileShader(GLenum type, const char* source, GLuint& shader) {
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        throw std::runtime_error(std::string("Shader compilation failed: ") + infoLog);
    }
}

void Renderer::createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader, fragmentShader;
    compileShader(GL_VERTEX_SHADER, vertexSource ? vertexSource : defaultVertexShader, vertexShader);
    compileShader(GL_FRAGMENT_SHADER, fragmentSource ? fragmentSource : defaultFragmentShader, fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        throw std::runtime_error(std::string("Shader program linking failed: ") + infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Renderer::Renderer(const char* vertexShaderSource, const char* fragmentShaderSource) 
    : shaderProgram(0), camera(nullptr), selectedShape(nullptr), lastFrameTime(0.0f) {
    createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

Renderer::~Renderer() {
    for (Shape* shape : shapes) {
        delete shape;
    }
    delete camera;
    glDeleteProgram(shaderProgram);
}

void Renderer::init() {
    for (Shape* shape : shapes) {
        shape->init(shaderProgram);
    }
}

void Renderer::render() {
    glUseProgram(shaderProgram);
    if (camera) camera->setUniforms(shaderProgram);
    for (Shape* shape : shapes) {
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &shape->getModelMatrix()[0][0]);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), dynamic_cast<Mesh*>(shape) != nullptr);
        shape->render();
    }
}

void Renderer::loadFromJSON(const nlohmann::json& json) {
    // Clear existing shapes
    for (Shape* shape : shapes) {
        delete shape;
    }
    shapes.clear();
    delete camera;
    camera = nullptr;

    // Load camera if present
    if (json.contains("camera") && json["camera"].is_object()) {
        camera = new Camera(json["camera"]);
    }

    // Load shapes if present
    if (!json.contains("shapes") || !json["shapes"].is_array()) {
        throw std::runtime_error("JSON missing 'shapes' array");
    }

    for (const auto& shapeJson : json["shapes"]) {
        if (!shapeJson.contains("type") || !shapeJson["type"].is_string()) {
            throw std::runtime_error("Shape missing 'type' field");
        }
        std::string type = shapeJson["type"].get<std::string>();

        if (type == "triangle") {
            if (!shapeJson.contains("vertices") || !shapeJson["vertices"].is_array()) {
                throw std::runtime_error("Triangle missing 'vertices' array");
            }
            std::vector<Vertex> vertices;
            for (const auto& v : shapeJson["vertices"]) {
                if (!v.contains("x") || !v.contains("y") || !v.contains("z") ||
                    !v.contains("r") || !v.contains("g") || !v.contains("b")) {
                    throw std::runtime_error("Triangle vertex missing required fields (x, y, z, r, g, b)");
                }
                vertices.push_back({
                    v["x"].get<float>(), v["y"].get<float>(), v["z"].get<float>(),
                    v["r"].get<float>(), v["g"].get<float>(), v["b"].get<float>(),
                    0.0f, 0.0f
                });
            }
            shapes.push_back(new Triangle(vertices));
        } else if (type == "circle") {
            if (!shapeJson.contains("center") || !shapeJson["center"].is_object() ||
                !shapeJson.contains("radius") || !shapeJson.contains("color") ||
                !shapeJson.contains("segments")) {
                throw std::runtime_error("Circle missing required fields (center, radius, color, segments)");
            }
            if (!shapeJson["center"].contains("x") || !shapeJson["center"].contains("y") ||
                !shapeJson["center"].contains("z") || !shapeJson["color"].contains("r") ||
                !shapeJson["color"].contains("g") || !shapeJson["color"].contains("b")) {
                throw std::runtime_error("Circle missing center (x, y, z) or color (r, g, b) fields");
            }
            Vertex center = {
                shapeJson["center"]["x"].get<float>(),
                shapeJson["center"]["y"].get<float>(),
                shapeJson["center"]["z"].get<float>(),
                0.0f, 0.0f, 0.0f, 0.0f, 0.0f
            };
            Vertex color = {
                0.0f, 0.0f, 0.0f,
                shapeJson["color"]["r"].get<float>(),
                shapeJson["color"]["g"].get<float>(),
                shapeJson["color"]["b"].get<float>(),
                0.0f, 0.0f
            };
            shapes.push_back(new Circle(center, shapeJson["radius"].get<float>(),
                                       shapeJson["segments"].get<int>(), color));
        } else if (type == "cube") {
            if (!shapeJson.contains("position") || !shapeJson["position"].is_object() ||
                !shapeJson.contains("size") || !shapeJson.contains("color")) {
                throw std::runtime_error("Cube missing required fields (position, size, color)");
            }
            if (!shapeJson["position"].contains("x") || !shapeJson["position"].contains("y") ||
                !shapeJson["position"].contains("z") || !shapeJson["color"].contains("r") ||
                !shapeJson["color"].contains("g") || !shapeJson["color"].contains("b")) {
                throw std::runtime_error("Cube missing position (x, y, z) or color (r, g, b) fields");
            }
            Vertex pos = {
                shapeJson["position"]["x"].get<float>(),
                shapeJson["position"]["y"].get<float>(),
                shapeJson["position"]["z"].get<float>(),
                0.0f, 0.0f, 0.0f, 0.0f, 0.0f
            };
            Vertex color = {
                0.0f, 0.0f, 0.0f,
                shapeJson["color"]["r"].get<float>(),
                shapeJson["color"]["g"].get<float>(),
                shapeJson["color"]["b"].get<float>(),
                0.0f, 0.0f
            };
            shapes.push_back(new Cube(pos, shapeJson["size"].get<float>(), color));
        } else if (type == "mesh") {
            if (!shapeJson.contains("obj_file") || !shapeJson.contains("texture_file") ||
                !shapeJson.contains("position") || !shapeJson["position"].is_object() ||
                !shapeJson.contains("scale")) {
                throw std::runtime_error("Mesh missing required fields (obj_file, texture_file, position, scale)");
            }
            if (!shapeJson["position"].contains("x") || !shapeJson["position"].contains("y") ||
                !shapeJson["position"].contains("z")) {
                throw std::runtime_error("Mesh missing position (x, y, z) fields");
            }
            glm::vec3 pos(
                shapeJson["position"]["x"].get<float>(),
                shapeJson["position"]["y"].get<float>(),
                shapeJson["position"]["z"].get<float>()
            );
            shapes.push_back(new Mesh(shapeJson["obj_file"].get<std::string>(),
                                     shapeJson["texture_file"].get<std::string>(),
                                     pos, shapeJson["scale"].get<float>()));
        } else {
            throw std::runtime_error("Unknown shape type: " + type);
        }
    }

    // Initialize newly loaded shapes
    init();
}

void Renderer::setupImGui() {
    ImGui::NewFrame();
}
void Renderer::renderImGui(bool isDebugWindow, float fps, std::vector<Renderer*>& allRenderers) {
    if (isDebugWindow) {
        ImGui::Begin("Debug Info");
        ImGui::Text("FPS: %.1f", fps);
        if (selectedShape) {
            ImGui::Text("Selected Shape: %s", selectedShape->getType().c_str());
            nlohmann::json json;
            selectedShape->updateJSON(json);
            if (selectedShape->getType() == "triangle") {
                for (size_t i = 0; i < json["vertices"].size(); ++i) {
                    float pos[3] = {json["vertices"][i]["x"].get<float>(),
                                    json["vertices"][i]["y"].get<float>(),
                                    json["vertices"][i]["z"].get<float>()};
                    float col[3] = {json["vertices"][i]["r"].get<float>(),
                                    json["vertices"][i]["g"].get<float>(),
                                    json["vertices"][i]["b"].get<float>()};
                    if (ImGui::InputFloat3(("Position##" + std::to_string(i)).c_str(), pos)) {
                        json["vertices"][i]["x"] = pos[0];
                        json["vertices"][i]["y"] = pos[1];
                        json["vertices"][i]["z"] = pos[2];
                    }
                    if (ImGui::InputFloat3(("Color##" + std::to_string(i)).c_str(), col)) {
                        json["vertices"][i]["r"] = col[0];
                        json["vertices"][i]["g"] = col[1];
                        json["vertices"][i]["b"] = col[2];
                    }
                }
                if (ImGui::Button("Apply")) {
                    try {
                        shapes.clear();
                        shapes.push_back(new Triangle({
                            {json["vertices"][0]["x"].get<float>(), json["vertices"][0]["y"].get<float>(), json["vertices"][0]["z"].get<float>(),
                             json["vertices"][0]["r"].get<float>(), json["vertices"][0]["g"].get<float>(), json["vertices"][0]["b"].get<float>(), 0.0f, 0.0f},
                            {json["vertices"][1]["x"].get<float>(), json["vertices"][1]["y"].get<float>(), json["vertices"][1]["z"].get<float>(),
                             json["vertices"][1]["r"].get<float>(), json["vertices"][1]["g"].get<float>(), json["vertices"][1]["b"].get<float>(), 0.0f, 0.0f},
                            {json["vertices"][2]["x"].get<float>(), json["vertices"][2]["y"].get<float>(), json["vertices"][2]["z"].get<float>(),
                             json["vertices"][2]["r"].get<float>(), json["vertices"][2]["g"].get<float>(), json["vertices"][2]["b"].get<float>(), 0.0f, 0.0f}
                        }));
                        init();
                        ImGui::Text("Triangle updated successfully!");
                    } catch (const std::exception& e) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", e.what());
                    }
                }
            } else if (selectedShape->getType() == "circle") {
                float center[3] = {json["center"]["x"].get<float>(),
                                   json["center"]["y"].get<float>(),
                                   json["center"]["z"].get<float>()};
                float radius = json["radius"].get<float>();
                float color[3] = {json["color"]["r"].get<float>(),
                                  json["color"]["g"].get<float>(),
                                  json["color"]["b"].get<float>()};
                int segments = json["segments"].get<int>();
                if (ImGui::InputFloat3("Center", center)) {
                    json["center"]["x"] = center[0];
                    json["center"]["y"] = center[1];
                    json["center"]["z"] = center[2];
                }
                if (ImGui::InputFloat("Radius", &radius, 0.1f, 1.0f, "%.2f")) json["radius"] = radius;
                if (ImGui::InputFloat3("Color", color)) {
                    json["color"]["r"] = color[0];
                    json["color"]["g"] = color[1];
                    json["color"]["b"] = color[2];
                }
                if (ImGui::InputInt("Segments", &segments, 1, 100)) json["segments"] = segments;
                if (ImGui::Button("Apply")) {
                    try {
                        if (radius <= 0.0f) throw std::runtime_error("Radius must be positive");
                        if (segments < 3) throw std::runtime_error("Segments must be at least 3");
                        shapes.clear();
                        shapes.push_back(new Circle(
                            {center[0], center[1], center[2], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                            radius, segments, {0.0f, 0.0f, 0.0f, color[0], color[1], color[2], 0.0f, 0.0f}
                        ));
                        init();
                        ImGui::Text("Circle updated successfully!");
                    } catch (const std::exception& e) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", e.what());
                    }
                }
            } else if (selectedShape->getType() == "cube") {
                float pos[3] = {json["position"]["x"].get<float>(),
                                json["position"]["y"].get<float>(),
                                json["position"]["z"].get<float>()};
                float size = json["size"].get<float>();
                float color[3] = {json["color"]["r"].get<float>(),
                                  json["color"]["g"].get<float>(),
                                  json["color"]["b"].get<float>()};
                if (ImGui::InputFloat3("Position", pos)) {
                    json["position"]["x"] = pos[0];
                    json["position"]["y"] = pos[1];
                    json["position"]["z"] = pos[2];
                }
                if (ImGui::InputFloat("Size", &size, 0.1f, 1.0f, "%.2f")) json["size"] = size;
                if (ImGui::InputFloat3("Color", color)) {
                    json["color"]["r"] = color[0];
                    json["color"]["g"] = color[1];
                    json["color"]["b"] = color[2];
                }
                if (ImGui::Button("Apply")) {
                    try {
                        if (size <= 0.0f) throw std::runtime_error("Size must be positive");
                        shapes.clear();
                        shapes.push_back(new Cube(
                            {pos[0], pos[1], pos[2], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                            size, {0.0f, 0.0f, 0.0f, color[0], color[1], color[2], 0.0f, 0.0f}
                        ));
                        init();
                        ImGui::Text("Cube updated successfully!");
                    } catch (const std::exception& e) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", e.what());
                    }
                }
            } else if (selectedShape->getType() == "mesh") {
                float pos[3] = {json["position"]["x"].get<float>(),
                                json["position"]["y"].get<float>(),
                                json["position"]["z"].get<float>()};
                float scale = json["scale"].get<float>();
                if (ImGui::InputFloat3("Position", pos)) {
                    json["position"]["x"] = pos[0];
                    json["position"]["y"] = pos[1];
                    json["position"]["z"] = pos[2];
                }
                if (ImGui::InputFloat("Scale", &scale, 0.1f, 1.0f, "%.2f")) json["scale"] = scale;
                if (ImGui::Button("Apply")) {
                    try {
                        if (scale <= 0.0f) throw std::runtime_error("Scale must be positive");
                        shapes.clear();
                        shapes.push_back(new Mesh(
                            json["obj_file"].get<std::string>(),
                            json["texture_file"].get<std::string>(),
                            glm::vec3(pos[0], pos[1], pos[2]), scale
                        ));
                        init();
                        ImGui::Text("Mesh updated successfully!");
                    } catch (const std::exception& e) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", e.what());
                    }
                }
            }
        }
        if (ImGui::Button("Save Scene")) {
            nlohmann::json json;
            if (camera) camera->updateJSON(json["camera"]);
            for (Shape* shape : shapes) {
                nlohmann::json shapeJson;
                shape->updateJSON(shapeJson);
                json["shapes"].push_back(shapeJson);
            }
            std::ofstream file("scene.json");
            if (file.is_open()) {
                file << json.dump(2);
                file.close();
                ImGui::Text("Scene saved successfully!");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to save scene.json");
            }
        }
        ImGui::End();
    } else {
        ImGui::Begin("Add Element");
        static int shapeType = 0;
        static bool added = false;
        static bool error = false;
        static std::string errorMsg;
        ImGui::Text("Select a shape to add:");
        ImGui::Combo("Shape Type", &shapeType, "Triangle\0Circle\0Cube\0Mesh\0");
        static nlohmann::json newShape;
        if (shapeType == 0) { // Triangle
            static float vertices[9] = {0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f};
            static float colors[9] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            ImGui::Text("Triangle Vertices");
            ImGui::InputFloat3("Vertex 1", &vertices[0], "%.2f");
            ImGui::InputFloat3("Vertex 2", &vertices[3], "%.2f");
            ImGui::InputFloat3("Vertex 3", &vertices[6], "%.2f");
            ImGui::Text("Vertex Colors");
            ImGui::InputFloat3("Color 1", &colors[0], "%.2f");
            ImGui::InputFloat3("Color 2", &colors[3], "%.2f");
            ImGui::InputFloat3("Color 3", &colors[6], "%.2f");
            if (ImGui::Button("Add Triangle")) {
                try {
                    newShape = {
                        {"type", "triangle"},
                        {"vertices", {
                            {{"x", vertices[0]}, {"y", vertices[1]}, {"z", vertices[2]}, {"r", colors[0]}, {"g", colors[1]}, {"b", colors[2]}},
                            {{"x", vertices[3]}, {"y", vertices[4]}, {"z", vertices[5]}, {"r", colors[3]}, {"g", colors[4]}, {"b", colors[5]}},
                            {{"x", vertices[6]}, {"y", vertices[7]}, {"z", vertices[8]}, {"r", colors[6]}, {"g", colors[7]}, {"b", colors[8]}}
                        }}
                    };
                    shapes.push_back(new Triangle({
                        {vertices[0], vertices[1], vertices[2], colors[0], colors[1], colors[2], 0.0f, 0.0f},
                        {vertices[3], vertices[4], vertices[5], colors[3], colors[4], colors[5], 0.0f, 0.0f},
                        {vertices[6], vertices[7], vertices[8], colors[6], colors[7], colors[8], 0.0f, 0.0f}
                    }));
                    shapes.back()->init(shaderProgram);
                    added = true;
                    error = false;
                } catch (const std::exception& e) {
                    error = true;
                    errorMsg = e.what();
                }
            }
            if (ImGui::Button("Reset")) {
                vertices[0] = 0.0f; vertices[1] = 0.5f; vertices[2] = 0.0f;
                vertices[3] = -0.5f; vertices[4] = -0.5f; vertices[5] = 0.0f;
                vertices[6] = 0.5f; vertices[7] = -0.5f; vertices[8] = 0.0f;
                colors[0] = 1.0f; colors[1] = 0.0f; colors[2] = 0.0f;
                colors[3] = 0.0f; colors[4] = 1.0f; colors[5] = 0.0f;
                colors[6] = 0.0f; colors[7] = 0.0f; colors[8] = 1.0f;
                error = false;
                added = false;
            }
            if (added) ImGui::Text("Triangle added successfully!");
            if (error) ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", errorMsg.c_str());
        } else if (shapeType == 1) { // Circle
            static float center[3] = {0.0f, 0.0f, 0.0f};
            static float radius = 0.3f;
            static float color[3] = {1.0f, 1.0f, 0.0f};
            static int segments = 32;
            ImGui::Text("Circle Properties");
            ImGui::InputFloat3("Center", center, "%.2f");
            ImGui::InputFloat("Radius", &radius, 0.1f, 1.0f, "%.2f");
            ImGui::InputFloat3("Color", color, "%.2f");
            ImGui::InputInt("Segments", &segments, 1, 100);
            if (ImGui::Button("Add Circle")) {
                try {
                    if (radius <= 0.0f) throw std::runtime_error("Radius must be positive");
                    if (segments < 3) throw std::runtime_error("Segments must be at least 3");
                    newShape = {
                        {"type", "circle"},
                        {"center", {{"x", center[0]}, {"y", center[1]}, {"z", center[2]}}},
                        {"radius", radius},
                        {"color", {{"r", color[0]}, {"g", color[1]}, {"b", color[2]}}},
                        {"segments", segments}
                    };
                    shapes.push_back(new Circle(
                        {center[0], center[1], center[2], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                        radius, segments, {0.0f, 0.0f, 0.0f, color[0], color[1], color[2], 0.0f, 0.0f}
                    ));
                    shapes.back()->init(shaderProgram);
                    added = true;
                    error = false;
                } catch (const std::exception& e) {
                    error = true;
                    errorMsg = e.what();
                }
            }
            if (ImGui::Button("Reset")) {
                std::fill(center, center + 3, 0.0f);
                radius = 0.3f;
                color[0] = color[1] = 1.0f; color[2] = 0.0f;
                segments = 32;
                error = false;
                added = false;
            }
            if (added) ImGui::Text("Circle added successfully!");
            if (error) ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", errorMsg.c_str());
        } else if (shapeType == 2) { // Cube
            static float pos[3] = {0.0f, 0.0f, 0.0f};
            static float size = 0.5f;
            static float color[3] = {0.5f, 0.5f, 1.0f};
            ImGui::Text("Cube Properties");
            ImGui::InputFloat3("Position", pos, "%.2f");
            ImGui::InputFloat("Size", &size, 0.1f, 1.0f, "%.2f");
            ImGui::InputFloat3("Color", color, "%.2f");
            if (ImGui::Button("Add Cube")) {
                try {
                    if (size <= 0.0f) throw std::runtime_error("Size must be positive");
                    newShape = {
                        {"type", "cube"},
                        {"position", {{"x", pos[0]}, {"y", pos[1]}, {"z", pos[2]}}},
                        {"size", size},
                        {"color", {{"r", color[0]}, {"g", color[1]}, {"b", color[2]}}}
                    };
                    shapes.push_back(new Cube(
                        {pos[0], pos[1], pos[2], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                        size, {0.0f, 0.0f, 0.0f, color[0], color[1], color[2], 0.0f, 0.0f}
                    ));
                    shapes.back()->init(shaderProgram);
                    added = true;
                    error = false;
                } catch (const std::exception& e) {
                    error = true;
                    errorMsg = e.what();
                }
            }
            if (ImGui::Button("Reset")) {
                std::fill(pos, pos + 3, 0.0f);
                size = 0.5f;
                color[0] = color[1] = 0.5f; color[2] = 1.0f;
                error = false;
                added = false;
            }
            if (added) ImGui::Text("Cube added successfully!");
            if (error) ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", errorMsg.c_str());
        } else if (shapeType == 3) { // Mesh
            static char objFile[256] = "cube.obj";
            static char textureFile[256] = "texture.png";
            static float pos[3] = {0.0f, 0.0f, 0.0f};
            static float scale = 0.5f;
            ImGui::Text("Mesh Properties");
            ImGui::InputText("OBJ File", objFile, 256);
            ImGui::InputText("Texture File", textureFile, 256);
            ImGui::InputFloat3("Position", pos, "%.2f");
            ImGui::InputFloat("Scale", &scale, 0.1f, 1.0f, "%.2f");
            if (ImGui::Button("Add Mesh")) {
                try {
                    if (scale <= 0.0f) throw std::runtime_error("Scale must be positive");
                    if (std::string(objFile).empty()) throw std::runtime_error("OBJ file path cannot be empty");
                    if (std::string(textureFile).empty()) throw std::runtime_error("Texture file path cannot be empty");
                    newShape = {
                        {"type", "mesh"},
                        {"obj_file", objFile},
                        {"texture_file", textureFile},
                        {"position", {{"x", pos[0]}, {"y", pos[1]}, {"z", pos[2]}}},
                        {"scale", scale}
                    };
                    shapes.push_back(new Mesh(objFile, textureFile, glm::vec3(pos[0], pos[1], pos[2]), scale));
                    shapes.back()->init(shaderProgram);
                    added = true;
                    error = false;
                } catch (const std::exception& e) {
                    error = true;
                    errorMsg = e.what();
                }
            }
            if (ImGui::Button("Reset")) {
                strcpy(objFile, "cube.obj");
                strcpy(textureFile, "texture.png");
                std::fill(pos, pos + 3, 0.0f);
                scale = 0.5f;
                error = false;
                added = false;
            }
            if (added) ImGui::Text("Mesh added successfully!");
            if (error) ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", errorMsg.c_str());
        }
        ImGui::End();
        // Hierarchy window
        ImGui::Begin("Hierarchy");
        for (size_t i = 0; i < allRenderers.size(); ++i) {
            std::string windowName = "Window " + std::to_string(i);
            if (ImGui::TreeNode(windowName.c_str())) {
                for (size_t j = 0; j < allRenderers[i]->getShapes().size(); ++j) {
                    std::string shapeName = allRenderers[i]->getShapes()[j]->getType() + " " + std::to_string(j);
                    if (ImGui::Selectable(shapeName.c_str(), allRenderers[i]->getSelectedShape() == allRenderers[i]->getShapes()[j])) {
                        for (Renderer* r : allRenderers) r->setSelectedShape(nullptr);
                        allRenderers[i]->setSelectedShape(allRenderers[i]->getShapes()[j]);
                    }
                    if (ImGui::BeginPopupContextItem(("Context##" + shapeName).c_str())) {
                        if (ImGui::MenuItem("Delete")) {
                            delete allRenderers[i]->getShapes()[j];
                            allRenderers[i]->getShapes().erase(allRenderers[i]->getShapes().begin() + j);
                            if (allRenderers[i]->getSelectedShape() == allRenderers[i]->getShapes()[j]) {
                                allRenderers[i]->setSelectedShape(nullptr);
                            }
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

float Renderer::getFPS() const {
    using namespace std::chrono;
    static auto start = high_resolution_clock::now();
    auto currentTime = high_resolution_clock::now();
    float deltaTime = duration_cast<duration<float>>(currentTime - start).count();
    start = currentTime;
    return deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;
}