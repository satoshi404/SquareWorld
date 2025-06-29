
#include "window.h"
#include "renderer.h"
#include <stdexcept>
#include <iostream>
#include <imgui/backends/imgui_impl_sdl2.h>

bool Window::g_ImGuiInitialized = false;
int Window::indice = 0;

Window::Window(unsigned int width, unsigned int height, SDL_GLContext sharedContext, WindowType type) {
    this->type = type;
    this->width = width;
    this->height = height;
    this->aspect = static_cast<float>(width) / height;

    static bool sdlInitialized = false;
    if (!sdlInitialized) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
        }
        sdlInitialized = true;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    std::string defaultTitle = std::string("Window ") + std::to_string(indice++);
    window = SDL_CreateWindow(defaultTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        if (!sdlInitialized) SDL_Quit();
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        SDL_DestroyWindow(window);
        if (!sdlInitialized) SDL_Quit();
        throw std::runtime_error("Failed to create GL context: " + std::string(SDL_GetError()));
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        if (!sdlInitialized) SDL_Quit();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    SDL_GL_SetSwapInterval(1);

    renderer = new Renderer(nullptr, nullptr);
    renderer->SetType(type);
    renderer->SetWindow(this);

    if (!g_ImGuiInitialized) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui_ImplSDL2_InitForOpenGL(window, glContext);
        ImGui_ImplOpenGL3_Init("#version 330");
        g_ImGuiInitialized = true;
    }
}

Window::~Window() {
    delete renderer;
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
}

void Window::SetTitle(const char* title) {
    SDL_SetWindowTitle(window, title);
    this->title = title;
}

void Window::Show() {
    SDL_ShowWindow(window);
}

void Window::LoadFromJSON(const nlohmann::json& json) {
    if (!json.is_object()) {
        throw std::runtime_error("Invalid window JSON: must be an object");
    }

    if (json.contains("title") && json["title"].is_string()) {
        SetTitle(json["title"].get<std::string>().c_str());
    }

    if (json.contains("width") && json["width"].is_number_unsigned() &&
        json.contains("height") && json["height"].is_number_unsigned()) {
        width = json["width"].get<unsigned int>();
        height = json["height"].get<unsigned int>();
        aspect = static_cast<float>(width) / height;
        SDL_SetWindowSize(window, width, height);
    }

    if (json.contains("shapes") && json["shapes"].is_array()) {
        renderer->loadFromJSON(json);
        renderer->init();
    }

    if (json.contains("camera") && json["camera"].is_object()) {
        nlohmann::json cameraJson = json["camera"];
        cameraJson["aspect"] = aspect;
        renderer->loadFromJSON({{"camera", cameraJson}, {"shapes", json["shapes"]}});
    }
}

void Window::Draw(int* state, std::vector<Renderer*>& allRenderers) {
    if (SDL_GL_MakeCurrent(window, glContext) < 0) {
        throw std::runtime_error("Failed to make GL context current: " + std::string(SDL_GetError()));
    }

    int newWidth, newHeight;
    SDL_GetWindowSize(window, &newWidth, &newHeight);
    if (newWidth != width || newHeight != height) {
        width = newWidth;
        height = newHeight;
        aspect = static_cast<float>(width) / height;
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
        renderer->updateCameraAspect(aspect);
    }

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (type == WINDOW_HIERARCHY) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        renderer->renderImGui(true, renderer->getFPS(), allRenderers);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    } else {
        renderer->render();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        renderer->renderImGui(type == WINDOW_DEBUG, renderer->getFPS(), allRenderers);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    SDL_GL_SwapWindow(window);
}
