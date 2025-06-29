#include "renderer.h"
#include <stdexcept>
#include <string>

// Default vertex shader
static const char* defaultVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec3 fragColor;
void main() {
    gl_Position = vec4(position, 1.0);
    fragColor = color;
}
)";

// Default fragment shader
static const char* defaultFragmentShader = R"(
#version 330 core
in vec3 fragColor;
out vec4 outColor;
void main() {
    outColor = vec4(fragColor, 1.0);
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
    : shaderProgram(0) {
    createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

Renderer::~Renderer() {
    for (Shape* shape : shapes) {
        delete shape;
    }
    glDeleteProgram(shaderProgram);
}

void Renderer::init() {
    for (Shape* shape : shapes) {
        shape->init(shaderProgram);
    }
}

void Renderer::render() {
    glUseProgram(shaderProgram);
    for (Shape* shape : shapes) {
        shape->render();
    }
}

void Renderer::loadFromJSON(const nlohmann::json& json) {
    for (const auto& shapeJson : json["shapes"]) {
        std::string type = shapeJson["type"].get<std::string>();
        if (type == "triangle") {
            std::vector<Vertex> vertices;
            for (const auto& v : shapeJson["vertices"]) {
                vertices.push_back({
                    v["x"].get<float>(), v["y"].get<float>(), 0.0f,
                    v["r"].get<float>(), v["g"].get<float>(), v["b"].get<float>()
                });
            }
            shapes.push_back(new Triangle(vertices));
        } else if (type == "circle") {
            Vertex center = {
                shapeJson["center"]["x"].get<float>(),
                shapeJson["center"]["y"].get<float>(),
                0.0f,
                0.0f, 0.0f, 0.0f
            };
            Vertex color = {
                0.0f, 0.0f, 0.0f,
                shapeJson["color"]["r"].get<float>(),
                shapeJson["color"]["g"].get<float>(),
                shapeJson["color"]["b"].get<float>()
            };
            shapes.push_back(new Circle(center, shapeJson["radius"].get<float>(),
                                       shapeJson["segments"].get<int>(), color));
        } else if (type == "cube") {
            Vertex pos = {
                shapeJson["position"]["x"].get<float>(),
                shapeJson["position"]["y"].get<float>(),
                shapeJson["position"]["z"].get<float>(),
                0.0f, 0.0f, 0.0f
            };
            Vertex color = {
                0.0f, 0.0f, 0.0f,
                shapeJson["color"]["r"].get<float>(),
                shapeJson["color"]["g"].get<float>(),
                shapeJson["color"]["b"].get<float>()
            };
            shapes.push_back(new Cube(pos, shapeJson["size"].get<float>(), color));
        }
    }
}