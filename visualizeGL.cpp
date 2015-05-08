#include "gl_cu_interop_pingpong_st.h"
#include <nv_helpers_gl/glresources.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::setInitialVolumeData()
{
  std::vector<float> volumeData;
  createSampleVolume( volumeData, m_volumeTexDimension, 10.0f );

  glBindTexture(GL_TEXTURE_3D, m_volumeTextureName[0]);
  glTexSubImage3D( GL_TEXTURE_3D, 0, 0, 0, 0, m_volumeTexDimension, m_volumeTexDimension, m_volumeTexDimension, GL_RED, GL_FLOAT, &volumeData[0] );

  glBindTexture(GL_TEXTURE_3D, m_volumeTextureName[1]);
  glTexSubImage3D( GL_TEXTURE_3D, 0, 0, 0, 0, m_volumeTexDimension, m_volumeTexDimension, m_volumeTexDimension, GL_RED, GL_FLOAT, &volumeData[0] );

  m_thicknessFactor = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::renderGLVolume(bool bOddStep)
{
  NV_PROFILE_SECTION("renderGLVolume");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Clear color buffer with black
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Face culling
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);

  // Blending
  if(true)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  // Volume rendering GLSL program
  m_glslVolume->use();
  m_glslVolume->setUniform( ULOC_MODEL2CLIP, m_viewProjection );

  m_glslVolume->setUniform( ULOC_EYEPOS, m_eyePos );
  m_glslVolume->setUniform( ULOC_VOLUMETEXDIMENSION, m_volumeTexDimension );

  m_glslVolume->setUniform( ULOC_VOLUMETEX, 0 );
  m_glslVolume->setUniform( ULOC_BACKFACETEX, 1 );

  m_glslVolume->setUniform( ULOC_COLLOW, m_colLow);
  m_glslVolume->setUniform( ULOC_COLHIGH, m_colHigh);
  m_glslVolume->setUniform( ULOC_THICKNESSFACTOR, m_thicknessFactor );

  // Set needed textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, m_volumeTextureName[ bOddStep ? 1 : 0 ]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_frontfaceInfoTextureName);

  // Draw!
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementArrayBufferName);
  glDrawElements( GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0 );


  glDisable(GL_BLEND);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::renderGLFrontface()
{
  NV_PROFILE_SECTION("renderGLFrontface");

  {
    NV_PROFILE_SECTION("switchGL");
    // Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferName);
    float Depth(1.0f);
    glClearBufferfv(GL_DEPTH , 0, &Depth);
    glClearBufferfv(GL_COLOR, 0, &nv_math::vec4f(0.0f, 0.0f, 0.0f, 0.0f)[0]);

    // Frontface GLSL program
    m_glslFrontFace->use();
    m_glslFrontFace->setUniform( ULOC_MODEL2CLIP, m_viewProjection );
    m_glslFrontFace->setUniform( ULOC_MODEL2WORLD, nv_math::mat4f(1.0f) );
    m_glslFrontFace->setUniform( ULOC_EYEPOS, m_eyePos );

    // Face culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
  }

  // Draw!
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementArrayBufferName);
  glDrawElements( GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initGL()
{
  initVolumeTexture(258);
  initFrontfaceInfoTexture();
  initPrograms();
  initArrayBuffer();

  setInitialVolumeData();
  setVisualizationParameters();

  // Set up initial GL state

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glFrontFace(GL_CCW);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initPrograms()
{
  // Volume rendering GLSL program

  m_glslVolume = new ProgramGL<Explicit>( (std::string(PROJECT_ABSDIRECTORY) + "/transform.vert").c_str()
                                        , (std::string(PROJECT_ABSDIRECTORY) + "/volume.frag").c_str()
                                        , VertexAttributes
                                          ("Position", 0)
                                        , Uniforms
                                          ("model2clip",         UPT_MAT4F32)
                                          ("model2world",        UPT_MAT4F32)
                                          ("eyePos",             UPT_VEC4F32)
                                          ("volumeTexDimension", UPT_UINT32 )
                                          ("volumeTex",          UPT_UINT32 )
                                          ("frontfaceTex",        UPT_UINT32 )
                                          ("visRange",           UPT_VEC2F32)
                                          ("colLow",             UPT_VEC3F32)
                                          ("colHigh",            UPT_VEC3F32)
                                          ("thicknessFactor",    UPT_FLOAT32)
                                         );


  // Frontface ray length encoding program

  m_glslFrontFace = new ProgramGL<Explicit>( (std::string(PROJECT_ABSDIRECTORY) + "/transform.vert").c_str()
                                          , (std::string(PROJECT_ABSDIRECTORY) + "/frontface.frag").c_str()
                                          , VertexAttributes
                                            ("Position", 0)
                                          , Uniforms
                                            ("model2clip", UPT_MAT4F32)
                                            ("model2world",UPT_MAT4F32)
                                            ("eyePos",     UPT_VEC4F32)
                                          );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initFramebuffer()
{
  bool Validated(true);

  glGenFramebuffers(1, &m_framebufferName);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferName);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_frontfaceInfoTextureName, 0);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_frontfaceInfoDepthTextureName, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::resize(int width, int height)
{
  m_window.m_viewsize[0] = width;
  m_window.m_viewsize[1] = height;

  newTexture(m_frontfaceInfoTextureName);
  glBindTexture(GL_TEXTURE_2D, m_frontfaceInfoTextureName);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, width, height);

  newTexture(m_frontfaceInfoDepthTextureName);
  glBindTexture(GL_TEXTURE_2D, m_frontfaceInfoDepthTextureName);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height);

  initFramebuffer();

  glViewport(0, 0, m_window.m_viewsize[0], m_window.m_viewsize[1]);

  resizeUI(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initFrontfaceInfoTexture()
{
  glGenTextures(1, &m_frontfaceInfoTextureName);
  glGenTextures(1, &m_frontfaceInfoDepthTextureName);

  glBindTexture(GL_TEXTURE_2D, m_frontfaceInfoTextureName);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glBindTexture(GL_TEXTURE_2D, m_frontfaceInfoDepthTextureName);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  resize(m_window.m_viewsize[0], m_window.m_viewsize[1]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initVolumeTexture(unsigned int volumeTexSize)
{
  nv_math::vec4f borderCol(0.0f, 0.0f, 0.0f, 1.0f);
  m_volumeTexDimension = volumeTexSize;
  m_volumeTexDimensionNonBoundary = m_volumeTexDimension - 2;


  glGenTextures( 2, m_volumeTextureName );

  //Texture 0
  glBindTexture(GL_TEXTURE_3D, m_volumeTextureName[0]);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, borderCol.get_value() );
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  glTexStorage3D( GL_TEXTURE_3D, 1, GL_R32F, m_volumeTexDimension, m_volumeTexDimension, m_volumeTexDimension );

  //Texture 1
  glBindTexture(GL_TEXTURE_3D, m_volumeTextureName[1]);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, borderCol.get_value() );
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  glTexStorage3D( GL_TEXTURE_3D, 1, GL_R32F, m_volumeTexDimension, m_volumeTexDimension, m_volumeTexDimension );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initArrayBuffer()
{
  nv_math::vec4f const dataPositionAttribute[] =
  {
    // Z+
    nv_math::vec4f(-0.5f, -0.5f, 0.5f, 1.0f),
    nv_math::vec4f( 0.5f, -0.5f, 0.5f, 1.0f),
    nv_math::vec4f( 0.5f,  0.5f, 0.5f, 1.0f),
    nv_math::vec4f(-0.5f,  0.5f, 0.5f, 1.0f),

    // X+
    nv_math::vec4f( 0.5f, -0.5f,  0.5f, 1.0f),
    nv_math::vec4f( 0.5f, -0.5f, -0.5f, 1.0f),
    nv_math::vec4f( 0.5f,  0.5f, -0.5f, 1.0f),
    nv_math::vec4f( 0.5f,  0.5f,  0.5f, 1.0f),

    // Z-
    nv_math::vec4f(-0.5f, -0.5f, -0.5f, 1.0f),
    nv_math::vec4f(-0.5f,  0.5f, -0.5f, 1.0f),
    nv_math::vec4f( 0.5f,  0.5f, -0.5f, 1.0f),
    nv_math::vec4f( 0.5f, -0.5f, -0.5f, 1.0f),


    // X-
    nv_math::vec4f(-0.5f, -0.5f,  0.5f, 1.0f),
    nv_math::vec4f(-0.5f,  0.5f,  0.5f, 1.0f),
    nv_math::vec4f(-0.5f,  0.5f, -0.5f, 1.0f),
    nv_math::vec4f(-0.5f, -0.5f, -0.5f, 1.0f),

    // Y-
    nv_math::vec4f( 0.5f, -0.5f,  0.5f, 1.0f),
    nv_math::vec4f(-0.5f, -0.5f,  0.5f, 1.0f),
    nv_math::vec4f(-0.5f, -0.5f, -0.5f, 1.0f),
    nv_math::vec4f( 0.5f, -0.5f, -0.5f, 1.0f),

    // Y+
    nv_math::vec4f( 0.5f,  0.5f,  0.5f, 1.0f),
    nv_math::vec4f( 0.5f,  0.5f, -0.5f, 1.0f),
    nv_math::vec4f(-0.5f,  0.5f, -0.5f, 1.0f),
    nv_math::vec4f(-0.5f,  0.5f,  0.5f, 1.0f),
  };

  GLsizeiptr const sizePositionAttributeData = sizeof(dataPositionAttribute);
  
  glGenVertexArrays(1, &m_vaoName);
  glBindVertexArray(m_vaoName);

  glGenBuffers(1, &m_arrayBufferName);
  glBindBuffer(GL_ARRAY_BUFFER, m_arrayBufferName);
  glBufferData(GL_ARRAY_BUFFER, sizePositionAttributeData, &dataPositionAttribute[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);


  // Fill our index buffer
  unsigned int const dataIndices[] = 
  {
    0, 1, 2,
    2, 3, 0,

    4, 5, 6,
    6, 7, 4,

    8, 9, 10,
    10, 11, 8,

    12, 13, 14,
    14, 15, 12,

    16, 17, 18,
    18, 19, 16,

    20, 21, 22,
    22, 23, 20,
  };

  GLsizeiptr sizeIndexData = sizeof(dataIndices);
  m_indexCount = sizeIndexData / sizeof(dataIndices[0]);


  // Create our gl buffer that will be used as an element buffer

  glGenBuffers(1, &m_elementArrayBufferName);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementArrayBufferName);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndexData, &dataIndices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



  glBindBuffer(GL_ARRAY_BUFFER, m_arrayBufferName);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementArrayBufferName);

  glEnableVertexAttribArray(0);
}
