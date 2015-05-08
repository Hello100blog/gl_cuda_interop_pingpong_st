#version 450 core

layout(location = 4) uniform vec4 eyePos;

layout(location = 5) uniform uint volumeTexDimension;
layout(location = 6) uniform sampler3D volumeTex;
layout(location = 7) uniform sampler2D frontfaceTex;

layout(location = 8) uniform vec2 visRange;
layout(location = 9) uniform vec3 colLow;
layout(location = 10) uniform vec3 colHigh;

layout(location = 11) uniform float thicknessFactor;

in vec4 vRayHit;

layout(location = 0, index = 0) out vec4 Color;

void main()
{
  
  vec3 dRay = vRayHit.xyz - eyePos.xyz;
  float distToEnd = length(dRay);
  vec3 dir = dRay/distToEnd;
  float rayTravelDist = distToEnd - texelFetch( frontfaceTex, ivec2(gl_FragCoord.xy), 0 ).r;
  
  float cellsPerUnitLength = 1.0f*volumeTexDimension;
  int numSteps = int(rayTravelDist*cellsPerUnitLength);
  vec3 rayStep = rayTravelDist*dir/numSteps;
  //Lookup is currently hard-coded to a cube volume.
  //This should ideally be replaced by a function that maps from world-space to 3D texture space.
  vec3 rayCurPos = 0.5*(vec3(1.0f) + rayStep) + vRayHit.xyz - rayTravelDist*dir;
  
  float rayCurVal = 0.0f;
 
  for( int i = 0; i < numSteps; i++ )
  {
    rayCurVal += clamp
				 (
				   fma( texture(volumeTex, rayCurPos).r, visRange[0], visRange[1] )
			 	 , 0.0f
				 , 1.0f
				 );
					  
    rayCurPos += rayStep;
  }
  
  vec3 finalColor = mix( colLow, colHigh, rayCurVal );

  Color = vec4( finalColor, max( thicknessFactor * rayTravelDist, 0.0f ) );
}
