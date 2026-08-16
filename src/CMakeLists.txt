#src文件目录中的CMake文件

#源文件
set(SOURCES
	"main.cpp"
	"core/VulkanInstance.cpp"
	"core/VulkanPhysicalDevice.cpp"
	"core/VulkanLogicalDevice.cpp"
    "core/VulkanValidationLayer.cpp"
	"core/VulkanQueueFamily.cpp"
	"core/VulkanSurface.cpp"
	"core/VulkanSwapChain.cpp"
	"core/VulkanGraphicsPipeline.cpp"
	"core/VulkanDescription.cpp"
	"core/VulkanCommandBuffer.cpp"
	"core/VulkanBuffer.cpp"
	"core/VulkanSync.cpp"
	"core/GLFWwindow.cpp"
)

#头文件
set(HEADERS
	"${CMAKE_SOURCE_DIR}/include/core/VulkanInstance.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanPhysicalDevice.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanLogicalDevice.h"
    "${CMAKE_SOURCE_DIR}/include/core/VulkanValidationLayer.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanQueueFamily.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanSurface.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanSwapChain.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanGraphicsPipeline.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanDescription.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanCommandBuffer.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanBuffer.h"
	"${CMAKE_SOURCE_DIR}/include/core/VulkanSync.h"
	"${CMAKE_SOURCE_DIR}/include/core/GLFWwindow.h"
)

set(ASSETS
	"${CMAKE_SOURCE_DIR}/asset/shaders/shader.vert"
	"${CMAKE_SOURCE_DIR}/asset/shaders/shader.frag"
	"${CMAKE_SOURCE_DIR}/asset/spirv/vertexShader.spv"
	"${CMAKE_SOURCE_DIR}/asset/spirv/fragmentShader.spv"
)

#添加可执行文件
add_executable(VulkanEngine ${SOURCES} ${HEADERS} ${ASSETS})

#链接头文件路径
target_include_directories(VulkanEngine PUBLIC
	${CMAKE_SOURCE_DIR}/include
	${GLFW_INCLUDE_DIRS} #GLFW库头文件路径
	${GLM_INCLUDE_DIRS}  #glm库头文件路径
	${VULKAN_INCLUDE_DIRS} #vulkan头文件路径
	${OpenCV_INCLUDE_DIRS} #OpenCV头文件路径
	${Assimp_INCLUDE_DIRS} #Assimp头文件路径
)

#链接静态库文件
target_link_libraries(VulkanEngine PRIVATE
	${GLFW_LIBRARIES}
	${VULKAN_LIBRARIES}
	${OpenCV_LIBRARIES}
	${Assimp_LIBRARIES}
)

#拷贝Assimp动态库到可执行文件目录
file(COPY ${Assimp_Dll} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src/Debug)

#自定义命令
#add_custom_command(TARGET VulkanEngine POST_BUILD
	#COMMAND ${CMAKE_COMMAND} -E copy_if_different
	#${Assimp_DLL} 
	#$<TARGET_FILE_DIR:VulkanEngine>
#)

#创建文件分组
source_group(TREE "${CMAKE_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE "${CMAKE_SOURCE_DIR}/asset" PREFIX "asset Files" FILES ${ASSETS})

#set_directory_properties(${CMAKE_SOURCE_DIR})