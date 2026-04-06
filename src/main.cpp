#include "glad.h"

#include "GLFW/glfw3.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "TimesNewRoman.h"

#include "node.h"
#include "algorithms.h"
#include "main.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

guiNode::guiNode(int n, ImVec2 pos) : n(n), pos(pos) {};

static void glfw_error_callback(int error, const char* description){
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    
    if (!glfwInit()) {
        return 1;
    }
    
    // Decide GL and GLSL versions
#if defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "Pathfinding App", nullptr, nullptr);
    
    if (window == nullptr) {
        return 1;
    }
    
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialise GLAD";
        return 1;
    }

    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    //Style
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    //Initialise backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //Load fonts
    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;

    io.Fonts->AddFontFromMemoryTTF(timesNewRoman, timesNewRomanSize, 16.0f, &fontConfig);

    //State
    const ImVec2 graphSize = ImVec2(800.0f, 400.0f);
    const float nodeRadius = 20.0f;

    const ImU32 colourBlue = IM_COL32(21, 45, 84, 255);
    const ImU32 colourBlack = IM_COL32(0, 0, 0, 255);
    const ImU32 colourAqua = IM_COL32(21, 45, 50, 255);
    const ImU32 colourGreen = IM_COL32(17, 102, 58, 255);

    std::vector<guiNode> guiNodes = {};
    std::vector<node> nodes = {};
    std::vector<hnode> hnodes = {};
    std::vector<std::vector<bool>> edgeMatrix = {};

    int dragNode = -1;
    int selectedNode = -1;
    std::pair<int, int> selectedEdge = { -1, -1 };

    float positions[4] = { 20.0f, 100.0f, 150.0f, 300.0f };
    for (int i = 0; i < 4; i++) {
        guiNodes.push_back(guiNode(i, ImVec2(positions[i], positions[i])));
        nodes.push_back(node(i));
        
        for (std::vector<bool>& vect : edgeMatrix) {
            vect.push_back(true);
        }

        edgeMatrix.push_back(std::vector<bool>(nodes.size(), true));
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        //New ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Make ImGui window match size of parent window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);

        ImGui::Begin("Hello, world!", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );

        ImVec2 origin = ImGui::GetCursorScreenPos();

        //------------------------------------------------------------------------------

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        float windowWidth = ImGui::GetWindowWidth();

        ImVec2 graphStart = ImVec2(ImVec2(origin.x + (windowWidth / 2) - (graphSize.x / 2), origin.y));
        ImVec2 graphEnd = ImVec2(origin.x + (windowWidth / 2) + (graphSize.x / 2), origin.y + graphSize.y);

        drawList->AddRect(graphStart, graphEnd, colourBlue, 0.0f, 0, 5.0f);

        ImVec2 mousePos = ImGui::GetMousePos() - origin;
        ImVec2 mousePosOnGraph = ImGui::GetMousePos() - graphStart;
        bool mouseClicked = ImGui::IsMouseClicked(0);
        bool windowHovered = ImGui::IsWindowHovered();

        int nodeCount = nodes.size();
        bool nodeClicked = false;
        bool edgeClicked = false;

        //Draw edges
        for (int i = 0; i < nodeCount; i++) {
            for (int j = i + 1; j < nodeCount; j++) {
                if (! edgeMatrix[i][j]) {
                    continue;
                }

                ImU32 colour = colourAqua;

                //Select edge
                if (windowHovered && mouseClicked && ! edgeClicked) {
                    ImVec2& a = guiNodes[i].pos;
                    ImVec2& b = guiNodes[j].pos;

                    ImVec2 ab = b - a;
                    ImVec2 ap = mousePosOnGraph - a;
                    float t = (ap.x * ab.x + ap.y * ab.y) / (ab.x * ab.x + ab.y * ab.y);
                    t = std::clamp(t, 0.0f, 1.0f);
                    float dist = std::sqrt(ImLengthSqr(mousePosOnGraph - (a + ab * t)));

                    if (dist < 2.0f) {
                        edgeClicked = true;
                        selectedEdge = { i, j };
                    }
                }

                int& fst = selectedEdge.first;
                int& snd = selectedEdge.second;
                if ((i == fst && j == snd) || (i == snd && j == fst)) {
                    colour = colourGreen;
                }

                drawList->AddLine(graphStart + guiNodes[i].pos, graphStart + guiNodes[j].pos, colour, 3.0f);
            }
        }

        //Draw nodes
        for (int i = 0; i < nodeCount; i++) {
            guiNode& node = guiNodes[i];
            std::string index = std::to_string(node.n);
            ImVec2 textOffset = ImGui::CalcTextSize(index.c_str()) / 2;
            ImVec2 nodeDrawPos = graphStart + node.pos;
            ImU32 colour = colourBlue;

            //Dragging & selecting
            if (windowHovered) {
                bool hovered = ImLengthSqr(mousePosOnGraph - node.pos) < nodeRadius * nodeRadius;

                if (hovered) {
                    if (ImGui::IsMouseDown(0) && dragNode == -1) {
                        dragNode = node.n;
                    }

                    if (mouseClicked && !nodeClicked) {
                        nodeClicked = true;
                        selectedNode = node.n;
                    }
                }

                if (dragNode == node.n) {
                    ImVec2 delta = ImGui::GetIO().MouseDelta;
                    nodeDrawPos += delta;

                    if (nodeDrawPos.x + nodeRadius > graphEnd.x) {
                        nodeDrawPos.x = graphEnd.x - nodeRadius;
                    }
                    else if (nodeDrawPos.x - nodeRadius < graphStart.x) {
                        nodeDrawPos.x = graphStart.x + nodeRadius;
                    }

                    if (nodeDrawPos.y + nodeRadius > graphEnd.y) {
                        nodeDrawPos.y = graphEnd.y - nodeRadius;
                    }
                    else if (nodeDrawPos.y - nodeRadius < graphStart.y) {
                        nodeDrawPos.y = graphStart.y + nodeRadius;
                    }

                    node.pos = nodeDrawPos - graphStart;
                }

                if (selectedNode == node.n) {
                    colour = colourGreen;
                }

                if (ImGui::IsMouseReleased(0)) {
                    dragNode = -1;
                }
            }

            drawList->AddCircleFilled(nodeDrawPos, nodeRadius, colour);
            drawList->AddText(nodeDrawPos - textOffset, colourBlack, index.c_str());
        }

        if (!nodeClicked && mouseClicked) {
            selectedNode = -1;
        }
        else if (!edgeClicked && mouseClicked) {
            selectedEdge = { -1, -1 };
        }

        //------------------------------------------------------------------------------

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    };

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
};