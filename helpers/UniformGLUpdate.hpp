#pragma once

#include <vector>
#include <map>

#include <nv_math/nv_math.h>

namespace nv_helpers_gl
{

  enum UniformParameterType
  {
    UPT_INT8 = 0,     UPT_INT16,      UPT_INT32,      UPT_INT64,
    UPT_UINT8,        UPT_UINT16,     UPT_UINT32,     UPT_UINT64,

    UPT_FLOAT16,      UPT_FLOAT32,    UPT_FLOAT64,

    UPT_VEC2F16,      UPT_VEC2F32,    UPT_VEC2F64,
    UPT_VEC3F16,      UPT_VEC3F32,    UPT_VEC3F64,
    UPT_VEC4F16,      UPT_VEC4F32,    UPT_VEC4F64,

    UPT_VEC2I8,       UPT_VEC2I16,    UPT_VEC2I32,    UPT_VEC2I64,
    UPT_VEC3I8,       UPT_VEC3I16,    UPT_VEC3I32,    UPT_VEC3I64,
    UPT_VEC4I8,       UPT_VEC4I16,    UPT_VEC4I32,    UPT_VEC4I64,

    UPT_VEC2UI8,      UPT_VEC2UI16,   UPT_VEC2UI32,   UPT_VEC2UI64,
    UPT_VEC3UI8,      UPT_VEC3UI16,   UPT_VEC3UI32,   UPT_VEC3UI64,
    UPT_VEC4UI8,      UPT_VEC4UI16,   UPT_VEC4UI32,   UPT_VEC4UI64,

    UPT_MAT2F16,      UPT_MAT2F32,    UPT_MAT2F64,
    UPT_MAT3F16,      UPT_MAT3F32,    UPT_MAT3F64,
    UPT_MAT4F16,      UPT_MAT4F32,    UPT_MAT4F64,

    UPT_NUMTYPES
  };

  namespace UniformGL
  {
    typedef unsigned int UniformLocation;

    struct UniformParameterDesc
    {
      const char* name;
      UniformParameterType uniformParamType;

      UniformParameterDesc( const char* strName, UniformParameterType paramType )
                          : name(strName)
                          , uniformParamType(paramType) {}
    };

    template<typename T>
    inline void setUniform( UniformLocation uloc, const T& value )
    {
      assert(!"Unknown or not implemented yet");
    }

    template<>
    inline void setUniform( UniformLocation uloc, const int& value )
    {
      glUniform1i( uloc, value );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const unsigned int& value )
    {
      glUniform1ui( uloc, value );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const float& value )
    {
      glUniform1f( uloc, value );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::vec2f& value )
    {
      glUniform2fv( uloc, 1, value.vec_array );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::vec3f& value )
    {
      glUniform3fv( uloc, 1, value.vec_array );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::vec4f& value )
    {
      glUniform4fv( uloc, 1, value.vec_array );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::vec2i& value )
    {
      glUniform2iv( uloc, 1, value.vec_array );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::vec3i& value )
    {
      glUniform3iv( uloc, 1, value.vec_array );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::vec4i& value )
    {
      glUniform4iv( uloc, 1, value.vec_array );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::vec2ui& value )
    {
      glUniform2uiv( uloc, 1, value.vec_array );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::vec3ui& value )
    {
      glUniform3uiv( uloc, 1, value.vec_array );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::vec4ui& value )
    {
      glUniform4uiv( uloc, 1, value.vec_array );
    }
    /*
    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::mat2f& value )
    {
      glUniformMatrix2fv( uloc, 1, GL_FALSE, value.mat_array );
    }
    */
    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::mat3f& value )
    {
      glUniformMatrix3fv( uloc, 1, GL_FALSE, value.mat_array );
    }

    template<>
    inline void setUniform( UniformLocation uloc, const nv_math::mat4f& value )
    {
      glUniformMatrix4fv( uloc, 1, GL_FALSE, value.mat_array );
    }
  }
  

  class Uniforms
  {
  public:
    Uniforms(){}
    Uniforms( const char* name, UniformParameterType upType )
    {
      m_uniforms.push_back( UniformGL::UniformParameterDesc(name, upType) );
    }

    inline Uniforms& operator ()( const char* name, UniformParameterType upType )
    {
      m_uniforms.push_back( UniformGL::UniformParameterDesc(name, upType) );
      return *this;
    }

    inline const std::vector<UniformGL::UniformParameterDesc>& get() const
    {
      return m_uniforms;
    }

  private:
    std::vector<UniformGL::UniformParameterDesc> m_uniforms;
  };

  template < typename CacheType >
  class UniformGLUpdate
  {
    inline void initCache( GLuint progName
                         , const std::vector<UniformGL::UniformParameterDesc>& uniformDescs )
                         { assert(false); }
  };

} //namespace nv_helpers_gl
