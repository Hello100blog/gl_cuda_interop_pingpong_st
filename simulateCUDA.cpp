#include "gl_cu_interop_pingpong_st.h"

#include <cudaGL.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::simulateCUDA(float dt)
{
  NV_PROFILE_SECTION_EX("simulateCUDA", *m_cudaTimer);

  if(!m_isRunning)
  {
    return;
  }

  int block_size = 8;
    
  void *args[1] = { &dt };

  CHECK_CUDA_CALL( cuLaunchKernel( m_entryPoint, (m_volumeTexDimensionNonBoundary/block_size), (m_volumeTexDimensionNonBoundary/block_size), (m_volumeTexDimensionNonBoundary/block_size)
                                  , block_size, block_size, block_size
                                  , 0, NULL, args, NULL)
                 , "FAILED to launch kernel" );


  //CHECK_CUDA_CALL( cuCtxSynchronize()
  //               , "FAILED to synchronize" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initCUDA()
{
  CUresult errRes = cuInit(0);

  CUdevice cuda_device = nv_helpers_cuda::findCudaDeviceDRV();

  CHECK_CUDA_CALL( cuDeviceGet(&m_oDevice, cuda_device)
                 , "Couldn't get the device" );

  CHECK_CUDA_CALL( cuGLCtxCreate(&m_oContext, CU_CTX_SCHED_AUTO, m_oDevice)
                 , "Couldn't create a context" );


  initCUDAEntryPoint( std::string(QUOTED_SAMPLE_DIR) + "/heatEquation.ptx", "heatEquationStep" );
  initCUDAArray();
  setCUDAVars();
  initCUDASurfTexReferences();

  m_cudaTimer = new nv_helpers_cuda::AsyncTimer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::bindCUDAReadWriteVolumes(bool bOddIter)
{
  NV_PROFILE_SECTION_EX("bindCUDAReadWriteVolumes", *m_cudaTimer);

  if(!m_isRunning)
  {
    return;
  }

  CHECK_CUDA_CALL( cuSurfRefSetArray( m_surfWriteRef, reinterpret_cast<CUarray>(m_cudaArray[bOddIter ? 1 : 0]), 0 )
                 , "Failed to set array to surface reference" );

  CHECK_CUDA_CALL( cuTexRefSetArray( m_surfReadRef, reinterpret_cast<CUarray>(m_cudaArray[bOddIter ? 0 : 1]), 0 )
                 , "Failed to set array to surface reference" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initCUDAArray()
{
  // We only need to do these gl-cuda bindings once.

  // Register both volume textures (ping-ponged) in CUDA
  CHECK_CUDA_CALL( cuGraphicsGLRegisterImage( &m_cuda_volumeTex[0], m_volumeTextureName[0], GL_TEXTURE_3D, CU_GRAPHICS_REGISTER_FLAGS_SURFACE_LDST )
                 , "Failed to register CUDA image for the volume texture" );
  CHECK_CUDA_CALL( cuGraphicsGLRegisterImage( &m_cuda_volumeTex[1], m_volumeTextureName[1], GL_TEXTURE_3D, CU_GRAPHICS_REGISTER_FLAGS_SURFACE_LDST )
                 , "Failed to register CUDA image for the volume texture" );

  CHECK_CUDA_CALL( cuGraphicsMapResources(1, &m_cuda_volumeTex[0], 0 )
                 , "Failed to map resource" );
  CHECK_CUDA_CALL( cuGraphicsMapResources(1, &m_cuda_volumeTex[1], 0 )
                 , "Failed to map resource" );

    // Bind the volume textures to their respective cuda arrays.
    CHECK_CUDA_CALL( cuGraphicsSubResourceGetMappedArray(&m_cudaArray[0], m_cuda_volumeTex[0], 0, 0)
                   , "Failed to get mapped array" );
    CHECK_CUDA_CALL( cuGraphicsSubResourceGetMappedArray(&m_cudaArray[1], m_cuda_volumeTex[1], 0, 0)
                   , "Failed to get mapped array" );

  CHECK_CUDA_CALL( cuGraphicsUnmapResources(1, &m_cuda_volumeTex[0], 0 )
                 , "Failed to unmap resource" );
  CHECK_CUDA_CALL( cuGraphicsUnmapResources(1, &m_cuda_volumeTex[1], 0 )
                 , "Failed to unmap resource" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initCUDASurfTexReferences()
{
  CHECK_CUDA_CALL( cuModuleGetSurfRef( &m_surfWriteRef, m_module, "volumeTexOut" )
                 , "Failed to get surface reference" );
  
  CHECK_CUDA_CALL( cuModuleGetTexRef( &m_surfReadRef, m_module, "volumeTexIn" )
                 , "Failed to get surface reference" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::setCUDAVars()
{
  // Set the volume dimension

  CUdeviceptr  d_volumeDim;
  size_t       d_volumeDimBytes;

  // First grab the global device pointers from the CUBIN
  CHECK_CUDA_CALL( cuModuleGetGlobal(&d_volumeDim,  &d_volumeDimBytes, m_module, "volumeDim")
                 , "Failed to get a pointer to constant in device memory" );

  // Copy the constants to video memory
  CHECK_CUDA_CALL( cuMemcpyHtoD(d_volumeDim, reinterpret_cast<const void *>(&m_volumeTexDimensionNonBoundary), d_volumeDimBytes)
                 , "Failed to set a constant in device memory" );


  // Set the volume coefficient

  CUdeviceptr  d_volumeCoeff;
  size_t       d_volumeCoeffBytes;

  // First grab the global device pointers from the CUBIN
  CHECK_CUDA_CALL( cuModuleGetGlobal(&d_volumeCoeff,  &d_volumeCoeffBytes, m_module, "volumeCoeff")
                 , "Failed to get a pointer to constant in device memory" );

  // Copy the constants to video memory
  CHECK_CUDA_CALL( cuMemcpyHtoD(d_volumeCoeff, reinterpret_cast<const void *>(&m_volumeCoeff), d_volumeCoeffBytes)
                 , "Failed to set a constant in device memory" );
  


  CUdeviceptr  d_cellWidth;
  size_t       d_cellWidthBytes;

  // First grab the global device pointers from the CUBIN
  CHECK_CUDA_CALL( cuModuleGetGlobal(&d_cellWidth,  &d_cellWidthBytes, m_module, "cellWidth")
                 , "Failed to get a pointer to constant in device memory" );

  // Copy the constants to video memory
  CHECK_CUDA_CALL( cuMemcpyHtoD(d_cellWidth, reinterpret_cast<const void *>(&m_cellWidth), d_cellWidthBytes)
                 , "Failed to set a constant in device memory" );

  
  CUdeviceptr  d_alpha;
  size_t       d_alphaBytes;

  // First grab the global device pointers from the CUBIN
  CHECK_CUDA_CALL( cuModuleGetGlobal(&d_alpha,  &d_alphaBytes, m_module, "alpha")
                  , "Failed to get a pointer to constant in device memory" );

  // Copy the constants to video memory
  CHECK_CUDA_CALL( cuMemcpyHtoD(d_alpha, reinterpret_cast<const void *>(&m_alpha), d_alphaBytes)
                 , "Failed to set a constant in device memory" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::initCUDAEntryPoint( const std::string& ptxFile, const std::string& entryPointName )
{
  CHECK_CUDA_CALL( cuModuleLoad( &m_module, ptxFile.c_str() )
                 , "Failed to load PTX module" );


  CHECK_CUDA_CALL( cuModuleGetFunction( &m_entryPoint, m_module, entryPointName.c_str() )
                 , "Failed to load entry point" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::simStart()
{
  m_isRunning = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::simPause()
{
  m_isRunning = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::simReset()
{
  setInitialVolumeData();
}
