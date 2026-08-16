//¶¥µã×ÅÉ«Æ÷

#version 460
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
//layout(location = 2) in vec2 aTexCoords;

layout(location = 0) out vec3 color;
//layout(location = 1) out vec2 texCoords;

layout(set = 0, binding = 0) uniform MVPUniform{
	mat4 projection;
	mat4 view;
	mat4 model;
} ubo;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	color = aColor;
	//texCoords = aTexCoords;
}