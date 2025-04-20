
#include "graphics.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// #include "../external/glad/src/glad.h"
#include "glad.h"
#include <GLFW/glfw3.h>

#include <iostream>

namespace Graphics
{

	GLFWwindow* window;
    ImGuiIO* io;

	int Initialize()
    {
        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            return -1;
        }

        int windowWidth = 600, windowHeight = 400;

        // Set OpenGL version (3.3 Core)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create GLFW window
        window = glfwCreateWindow(windowWidth, windowHeight, "File Server", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window! Make sure OpenGL 3.3+ is supported." << std::endl;
            glfwTerminate();
            return -1;
        }

         // Get the primary monitor's video mode
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vidMode = glfwGetVideoMode(primaryMonitor);

        if (vidMode) {
            int screenWidth = vidMode->width;
            int screenHeight = vidMode->height;

            // Calculate the centered position
            int posX = (screenWidth - windowWidth) / 2;
            int posY = (screenHeight - windowHeight) / 2;

            // Set the window position
            glfwSetWindowPos(window, posX, posY);
        }   


        glfwMakeContextCurrent(window);

        // Initialize GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD!" << std::endl;
            return -1;
        }

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = &ImGui::GetIO(); (void)io;
        // ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        return 0;
    }

	void Deinitialize()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();


		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool CheckWindowOpen()
	{
		return !glfwWindowShouldClose(window);
	}

    void Render()
    {
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static int selected_radio = 0;  // 0 or 1
        static char input1[128] = "";
        static char input2[128] = "";

        // Center the window
        ImVec2 windowSize = ImVec2(300, 150); // Set desired window size
        ImVec2 displaySize = io->DisplaySize;
        ImVec2 windowPos = ImVec2((displaySize.x - windowSize.x) * 0.5f, (displaySize.y - windowSize.y) * 0.5f);
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        ImGui::Begin(
            "Login Form", 
            nullptr, 
            ImGuiWindowFlags_AlwaysAutoResize 
            | ImGuiWindowFlags_NoCollapse 
            | ImGuiWindowFlags_NoResize 
            | ImGuiWindowFlags_NoNavFocus 
            | ImGuiWindowFlags_NoMove 
        );

        // Radio Buttons (Horizontal Layout)
        // ImGui::Text("Select an option:");
        ImGui::SameLine();
        if (ImGui::RadioButton("Public", selected_radio == 0)) selected_radio = 0;
        ImGui::SameLine();
        if (ImGui::RadioButton("Private", selected_radio == 1)) selected_radio = 1;

        // Input Fields (Vertical Layout)
        ImGui::Text("Enter Details:");
        ImGui::InputText("Username", input1, IM_ARRAYSIZE(input1));
        ImGui::InputText("Password", input2, IM_ARRAYSIZE(input2), 
            ImGuiInputTextFlags_Password 
            | ImGuiInputTextFlags_CharsNoBlank 
            | ImGuiInputTextFlags_EscapeClearsAll 
            | ImGuiInputTextFlags_AlwaysOverwrite 
            | ImGuiInputTextFlags_AutoSelectAll
            | ImGuiInputTextFlags_NoHorizontalScroll 
            | ImGuiInputTextFlags_NoUndoRedo 
        );

        // Submit Button
        if (ImGui::Button("Submit")) {
            // TODO: Need to make an api call and check if correct login details
            std::cout << "Submitted Data: " << input1 << ", " << input2 << ", Option " << (selected_radio + 1) << std::endl;
        }

        ImGui::End();

        // Render
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }


	void Draw()
	{
			
	}
}

