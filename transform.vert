#version 450 core

layout(location = 0) uniform mat4 model2clip;
layout(location = 1) uniform mat4 model2world;

layout(location = 0) in vec4 Position;

out vec4 vRayHit;

void main()
{
	gl_Position = model2clip * Position;
	vRayHit = model2world * Position;
}
