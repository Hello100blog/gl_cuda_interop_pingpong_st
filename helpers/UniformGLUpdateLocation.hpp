#pragma once

#include <vector>
#include <map>

#include <nv_math/nv_math.h>

#include "UniformGLUpdate.hpp"

namespace nv_helpers_gl
{
  struct Location { };

  template<>
  class UniformGLUpdate<Location>
  {
  private:

    struct UniformParameterInst
    {
      UniformParameterType uniformParamType;
      UniformGL::UniformLocation uniformLocation;
    };

  protected:
    std::vector< UniformParameterInst> m_uniformInsts;

    inline void initCache( GLuint progName
                         , const std::vector<UniformGL::UniformParameterDesc>& uniformDescs )
    {
      size_t cacheSize = uniformDescs.size();
      m_uniformInsts.resize( cacheSize );
      for( size_t i = 0; i < cacheSize; ++i )
      {
        m_uniformInsts[ i ].uniformLocation   = glGetUniformLocation( progName, uniformDescs[i].name );
        m_uniformInsts[ i ].uniformParamType  = uniformDescs[i].uniformParamType;
      }
    }

  public:

    //TODO: Need to make a debug implementation that uses the type flags and does assertions 
    // at run time - whereas the release build disregard these checks.
    template< typename T >
    inline void setUniform( unsigned int index, const T& value )
    {
      assert( index < m_uniformInsts.size() );
      //UniformParameterType utype = m_uniformInsts[index].first;

      UniformGL::setUniform(m_uniformInsts[index].uniformLocation, value);
    }
  };

} //namespace nv_helpers_gl
