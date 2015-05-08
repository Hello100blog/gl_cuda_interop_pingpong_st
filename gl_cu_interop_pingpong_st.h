#pragma once

#include <nv_math/nv_math.h>
#include <nv_helpers/cameracontrol.hpp>
#include <nv_helpers_gl/WindowProfiler.hpp>

#include "helpers/cudaTools.hpp"

#include "helpers/ProgramGL.hpp"
#include "helpers/UniformGLUpdateExplicit.hpp"

#include <GL/glew.h>

#include <cuda.h>

using namespace nv_helpers_gl;

class Sample : public nv_helpers_gl::WindowProfiler 
{
public:

  Sample() : m_arrayBufferName(0)
           , m_elementArrayBufferName(0)

           , m_framebufferName(0)

           , m_oContext(0)
           , m_oDevice(0)
           , m_volumeCoeff(1.0f)

           , m_isRunning(true)
  {}

  // Main methods
  bool begin();
  void end();
  void resize(int width, int height);
  void think(double time);
  void viewUpdate();

  // Main supplementary UI methods
  bool mouse_pos    (int x, int y);
  bool mouse_button (int button, int action);
  bool mouse_wheel  (int wheel);
  bool key_button   (int button, int action, int modifier);
  
  // UI methods
  void beginUI();
  void endUI();
  void renderUI();
  void resizeUI(int width, int height);
  static void _simStart(void* inst);
  static void _simPause(void* inst);
  static void _simReset(void* inst);
  void simStart();
  void simPause();
  void simReset();

  // Volume Render GL methods
  void renderGLVolume(bool bOddStep);
  void renderGLFrontface();
  void initGL();
  void initArrayBuffer();
  void initPrograms();
  void initFramebuffer();
  void initFrontfaceInfoTexture();
  void initVolumeTexture(unsigned int volumeTexSize);
  void setVisualizationParameters();

  void createSampleVolume( std::vector<float>& out, unsigned int volumeTexSize, float factor );
  void setInitialVolumeData();

  // Simulate CUDA methods
  void simulateCUDA(float simDt);
  void initCUDA();
  void initCUDAEntryPoint( const std::string& ptxFile, const std::string& entryPointName );
  void setCUDAVars();
  void initCUDAArray();
  void initCUDASurfTexReferences();
  void bindCUDAReadWriteVolumes(bool bOddIter);

private:

  enum
  {
    ULOC_MODEL2CLIP         = 0,
    ULOC_MODEL2WORLD        = 1,

    ULOC_EYEPOS             = 4,

    ULOC_VOLUMETEXDIMENSION = 5,
    ULOC_VOLUMETEX          = 6,
    ULOC_BACKFACETEX        = 7,
    ULOC_VISRANGE           = 8,
    ULOC_COLLOW             = 9,
    ULOC_COLHIGH            = 10,
    ULOC_THICKNESSFACTOR    = 11,
  };

  // View, projection
  nv_math::mat4f m_projection;
  nv_math::mat4f m_viewProjection;
  nv_math::vec4f m_eyePos;
  nv_math::vec4f m_lightDir;
  nv_helpers::CameraControl m_control;

  // Geometry stuff (volume cube)
  GLsizei m_indexCount;
  GLuint m_arrayBufferName;
  GLuint m_elementArrayBufferName;
  GLuint m_vaoName;
  
  // Ping-pong texures
  GLuint m_volumeTextureName[2];
  GLuint m_volumeTexturePrevName;
  GLuint m_volumeTextureCurName;
  CUarray m_cudaArray[2];
  CUgraphicsResource m_cuda_volumeTex[2];
  CUsurfref m_surfWriteRef;
  CUtexref m_surfReadRef;

  // Frontface info FBO
  GLuint m_framebufferName;
  GLuint m_frontfaceInfoTextureName;
  GLuint m_frontfaceInfoDepthTextureName;

  // Simulation + Visualization related
  unsigned int m_volumeTexDimension;
  unsigned int m_volumeTexDimensionNonBoundary;
  float m_volumeCoeff;
  float m_alpha;
  float m_cellWidth;
  nv_math::vec3f m_colLow;
  nv_math::vec3f m_colHigh;
  float m_thicknessFactor;
  bool m_isRunning;

  // CUDA stuff
  CUcontext m_oContext;
  CUdevice  m_oDevice;
  CUmodule m_module;
  CUfunction m_entryPoint;

  // GLSL stuff
  ProgramGL<Explicit>* m_glslFrontFace;
  ProgramGL<Explicit>* m_glslVolume;

  public:
    nv_helpers_cuda::AsyncTimer* m_cudaTimer;
};