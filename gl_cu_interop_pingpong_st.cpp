#include "gl_cu_interop_pingpong_st.h"

#include <main.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////

bool Sample::begin()
{
  beginUI();

  initGL();
  initCUDA();

  m_control.m_sceneOrbit      = nv_math::vec3f(-0.25f, 0.0f, 0.0f);
  m_control.m_sceneDimension  = float(10) * 0.2f;
  m_control.m_viewMatrix      = nv_math::look_at(m_control.m_sceneOrbit - nv_math::vec3f(0,0,-m_control.m_sceneDimension), m_control.m_sceneOrbit, nv_math::vec3f(0,1,0));

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::think(double time)
{
  static bool oddStep = true;

  //
  // CUDA Compute
  //

  // Setup the current read/write ping-pong textures
  bindCUDAReadWriteVolumes(oddStep);

  // PASS 0 (compute): compute the volume scalar field for the next time step
  simulateCUDA( /*dt =*/ 0.5f );  // 0.5 happens to be numerically stable for this simple explicit heat propagator


  //
  // OpenGL Visualize
  //

  // Update any camera changes
  viewUpdate();

  // PASS 1: Frontface distance
  renderGLFrontface();

  // PASS 2: Volume raycasting
  renderGLVolume(oddStep);
  oddStep = !oddStep;

  // ...Draw the UI
  renderUI();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::end()
{
  // Delete objects
  glDeleteBuffers(1, &m_arrayBufferName);
  glDeleteBuffers(1, &m_elementArrayBufferName);
  glDeleteVertexArrays(1, &m_vaoName);
  glDeleteTextures(1, &m_volumeTextureName[0]);
  glDeleteTextures(1, &m_frontfaceInfoTextureName);

  delete m_glslVolume;
  delete m_glslFrontFace;
  
  delete m_cudaTimer;
  cuCtxDestroy( m_oContext );

  endUI();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::viewUpdate()
{
  NV_PROFILE_SECTION("viewUpdate");

  m_control.processActions( m_window.m_viewsize
                          , nv_math::vec2f(m_window.m_mouseCurrent[0]
                          , m_window.m_mouseCurrent[1])
                          , m_window.m_mouseButtonFlags
                          , m_window.m_wheel );

  nv_math::mat4f projection = nv_math::perspective( rad2deg(nv_quarter_pi)
                                                  , float(m_window.m_viewsize[0]) / float(m_window.m_viewsize[1])
                                                  , 1.0f/m_volumeTexDimensionNonBoundary  //near-plane criteria so that it's not shaply visible when volume boundaries are in front of the near plane
                                                  , 1000.0f);
  m_viewProjection = projection * m_control.m_viewMatrix;

  //Get the inverse of the view matrix;
  nv_math::mat4f invView = nv_math::invert_rot_trans(m_control.m_viewMatrix);
  m_eyePos = nv_math::vec4f( invView[0][3], invView[1][3], invView[2][3], 1.0f );
  m_lightDir = m_eyePos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

int sample_main(int argc, const char** argv)
{
  Sample sample;
  return sample.run(
    PROJECT_NAME,
    argc, argv,
    768, 512,
    4, 5);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void sample_print(int level, const char * fmt)
{

}
