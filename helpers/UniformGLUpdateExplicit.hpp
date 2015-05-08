#pragma once

#include <vector>
#include <map>

#include <nv_math/nv_math.h>

#include "UniformGLUpdate.hpp"

namespace nv_helpers_gl
{
  struct Explicit { };

  template<>
  class UniformGLUpdate<Explicit>
  {
  protected:
    std::vector< UniformParameterType> m_uniformInsts;

    inline void initCache( GLuint progName
                         , const std::vector<UniformGL::UniformParameterDesc>& uniformDescs )
    {
      size_t cacheSize = uniformDescs.size();
      m_uniformInsts.resize( cacheSize );
      for( size_t i = 0; i < cacheSize; ++i )
      {
        m_uniformInsts[ i ] = uniformDescs[i].uniformParamType;
      }
    }

  public:
    //TODO: Not easy to validate at runtime. Need more sophisticated mechanism under debug build.
    template< typename T >
    inline void setUniform( unsigned int index, const T& value )
    {
      UniformGL::setUniform(index, value);
    }
  };

} //namespace nv_helpers_gl
