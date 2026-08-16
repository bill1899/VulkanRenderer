//GLFW´°¿ÚÀà

#include<iostream>
#include<GLFW/glfw3.h>

namespace me {

	struct WindowInfo {
		const int SCR_WIDTH = 1200;
		const int SCR_HEIGHT = 800;
		GLFWwindow* window = nullptr;
	};
	
	class Window
	{
	public:
		WindowInfo info;

	public:
		Window();
		~Window();

	private:
		void CreateWindow();
	};
}