
#include <core/GLFWwindow.h>

me::Window::Window()
{
	CreateWindow();
}

me::Window::~Window()
{
	
}

void me::Window::CreateWindow()
{
	glfwInit(); //初始化GLFW库，否则无法执行后续操作
	if (!glfwInit()) {
		std::cerr << "GLFW window init failed" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //禁止GLFW创建OpenGL的窗口后端
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   //暂时禁用调整窗口大小的功能

	info.window = glfwCreateWindow(info.SCR_WIDTH, info.SCR_HEIGHT, "Cloud Engine", nullptr, nullptr);
	if (info.window == nullptr) {
		std::cerr << "GLFW window create falied" << std::endl;
		return;
	}
	else
	{
		std::clog << "GLFW window create success" << std::endl;
	}
}
