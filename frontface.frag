#version 450 core

layout(location = 4) uniform vec4 eyePos;

in vec4 vRayHit;

layout(location = 0, index = 0) out float Distance;


void main()
{
  Distance = length(eyePos.xyz - vRayHit.xyz);
}
