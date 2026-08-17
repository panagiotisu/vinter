#pragma once

#include <SDL3/SDL.h>
#include <vinter/app.hpp>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

class Editor : public vn::App {
public:
    explicit Editor(const vn::ProjectSettings& settings) : App(settings) {
    }

protected:
    void load() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        io.FontGlobalScale = 1.5f;
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(1.5f);

        float monitor_scale = SDL_GetWindowDisplayScale(get_window().get_native_handle());
        ImGui::GetIO().FontGlobalScale = monitor_scale;
        ImGui::GetStyle().ScaleAllSizes(monitor_scale);

        ImGui_ImplSDL3_InitForSDLRenderer(
            get_window().get_native_handle(), get_renderer().get_native_handle()
        );
        ImGui_ImplSDLRenderer3_Init(get_renderer().get_native_handle());
    }

    void update() override {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        draw_ui();

        ImGui::Render();
    }

    void render() override {
        ImGui_ImplSDLRenderer3_RenderDrawData(
            ImGui::GetDrawData(), get_renderer().get_native_handle()
        );
    }

    void unload() override {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void handle_debug_gui_events(const SDL_Event& event) override {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }

private:
    static void draw_ui() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("New Project");
                ImGui::MenuItem("Open Project");
                ImGui::MenuItem("Save");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
};
