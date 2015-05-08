#pragma once

#include <GL/glew.h>

#include <assert.h>
#include <vector>
#include <map>

using namespace std;

#include "ProgramGLBase.inl"
#include "UniformGLUpdate.hpp"

namespace nv_helpers_gl
{
  template<typename TUpdate>
  class ProgramGL : public ProgramGLBase
                  , public UniformGLUpdate<TUpdate>
  {

  public:
    ProgramGL() : ProgramGLBase()
    {

    }

    //
    // Constructor: only vert shader + frag shader
    //
    ProgramGL( const char* vertexShaderFilename
             , const char* fragmentShaderFilename
             , const VertexAttributes& attributeNames
             , const Uniforms& uniformNames = Uniforms() )
    {

      std::string nameStr = createShadersAndProgram( vertexShaderFilename
                                                   , fragmentShaderFilename );

      bindAttributes( attributeNames.get(), nameStr );
      initCache( m_progName, uniformNames.get() );
    }

    //
    // Constructor: only vert shader + geom shader + frag shader
    //
    ProgramGL( const char* vertexShaderFilename
             , const char* geometryShaderFilename
             , const char* fragmentShaderFilename
             , const VertexAttributes& attributeNames
             , const Uniforms& uniformNames = Uniforms() )
    {

      std::string nameStr = createShadersAndProgram( vertexShaderFilename
                                                   , geometryShaderFilename
                                                   , fragmentShaderFilename );

      bindAttributes( attributeNames.get(), nameStr );
      initCache( m_progName, uniformNames.get() );
    }

    //
    // Constructor: vert shader + tess-cont shader + tess-eval shader + geom shader + frag shader
    //
    ProgramGL( const char* vertexShaderFilename
             , const char* tesscontrolShaderFilename
             , const char* tessevalShaderFilename
             , const char* geometryShaderFilename
             , const char* fragmentShaderFilename
             , const VertexAttributes& attributeNames
             , const Uniforms& uniformNames = Uniforms() )
    {

      std::string nameStr = createShadersAndProgram( vertexShaderFilename
                                                   , tesscontrolShaderFilename
                                                   , tessevalShaderFilename
                                                   , geometryShaderFilename
                                                   , fragmentShaderFilename );

      bindAttributes( attributeNames.get(), nameStr );
      initCache( m_progName, uniformNames.get() );
    }

    //
    // Constructor: only vert shader + tess-cont shader + tess-eval shader + frag shader
    //
    ProgramGL( const char* vertexShaderFilename
             , const char* tesscontrolShaderFilename
             , const char* tessevalShaderFilename
             , const char* fragmentShaderFilename
             , const VertexAttributes& attributeNames
             , const Uniforms& uniformNames = Uniforms() )
    {

      std::string nameStr = createShadersAndProgram( vertexShaderFilename
                                                   , tesscontrolShaderFilename
                                                   , tessevalShaderFilename
                                                   , fragmentShaderFilename );

      bindAttributes( attributeNames.get(), nameStr );
      initCache( m_progName, uniformNames.get() );
    }

    //
    // Constructor: only comp shader
    //
    ProgramGL( const char* computeShaderFilename
             , const Uniforms& uniformNames )
    {
      std::string nameStr = createShadersAndProgram( computeShaderFilename );
      bindAttributes( vector<VertexAttributeDesc>(), nameStr );
      initCache( m_progName, uniformNames.get() );
    }

    ~ProgramGL()
    {
      glDeleteProgram(m_progName);
    }

  };
}