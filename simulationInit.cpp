#include "gl_cu_interop_pingpong_st.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::setVisualizationParameters()
{
  // Visualization parameters:
  nv_math::vec2f range(0.0f, 10.0f);
  m_colLow = nv_math::vec3f(0.0f, 0.0f, 1.0f);
  m_colHigh = nv_math::vec3f(1.0f, 0.0157f, 0.0f);

  // Simulation parameters
  m_cellWidth = 1.0f / m_volumeTexDimension;
  m_alpha = 0.00001f; //Just the thermal diffusivity constant for the heat equation

  // Pack and send some visualization parameters to gl
  float factor = 1.0f/(range[1] - range[0]);
  float offset = -range[0]*factor;
  range = nv_math::vec2f(factor, offset);

  m_glslVolume->use();
  m_glslVolume->setUniform(ULOC_MODEL2WORLD, nv_math::mat4f(1.0f) );
  m_glslVolume->setUniform(ULOC_VISRANGE, range);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::createSampleVolume( std::vector<float>& out, unsigned int volumeTexSize, float factor )
{
  m_volumeCoeff = factor;

  int size = volumeTexSize*volumeTexSize*volumeTexSize;

  out.resize(size);

  float endCoeff = m_volumeCoeff / (1.0f + 3.0f*128.0f*128.0f);

  {
    unsigned int x = volumeTexSize-1;
    for(unsigned int y = 0; y < volumeTexSize; y++)
    {
      for(unsigned int z = 0; z < volumeTexSize; z++)
      {
        out[(x + y * volumeTexSize) + (z * volumeTexSize * volumeTexSize)] = 10.0f;
      }
    }
  }

  {
    unsigned int z = 0;
    for(unsigned int x = 0; x < volumeTexSize; x++)
    {
      for(unsigned int y = 0; y < volumeTexSize; y++)
      {
        out[(x + y * volumeTexSize) + (z * volumeTexSize * volumeTexSize)] = endCoeff;
      }
    }
  }

  {
    unsigned int z = volumeTexSize-1;
    for(unsigned int x = 0; x < volumeTexSize; x++)
    {
      for(unsigned int y = 0; y < volumeTexSize; y++)
      {
        out[(x + y * volumeTexSize) + (z * volumeTexSize * volumeTexSize)] = endCoeff;
      }
    }
  }

  {
    unsigned int y = 0;
    for(unsigned int x = 0; x < volumeTexSize; x++)
    {
      for(unsigned int z = 0; z < volumeTexSize; z++)
      {
        out[(x + y * volumeTexSize) + (z * volumeTexSize * volumeTexSize)] = endCoeff;
      }
    }
  }

  {
    unsigned int y = volumeTexSize-1;
    for(unsigned int x = 0; x < volumeTexSize; x++)
    {
      for(unsigned int z = 0; z < volumeTexSize; z++)
      {
        out[(x + y * volumeTexSize) + (z * volumeTexSize * volumeTexSize)] = endCoeff;
      }
    }
  }

  {
    unsigned int x = 0;
    for(unsigned int y = 0; y < volumeTexSize; y++)
    {
      for(unsigned int z = 0; z < volumeTexSize; z++)
      {
        out[(x + y * volumeTexSize) + (z * volumeTexSize * volumeTexSize)] = endCoeff;
      }
    }
  }


  for(unsigned int x = 1; x < volumeTexSize - 1; x++)
  {
    for(unsigned int y = 1; y < volumeTexSize - 1; y++)
    {
      for(unsigned int z = 1; z < volumeTexSize - 1; z++)
      {
        float xFromCenter = 0.5f*volumeTexSize - x;
        float yFromCenter = 0.5f*volumeTexSize - y;
        float zFromCenter = 0.5f*volumeTexSize - z;

        out[x + (y * volumeTexSize) + (z * volumeTexSize * volumeTexSize)] = m_volumeCoeff / (1.0f + xFromCenter*xFromCenter + yFromCenter*yFromCenter + zFromCenter*zFromCenter);

      }
    }
  }
}
