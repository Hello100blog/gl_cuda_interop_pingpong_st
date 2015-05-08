#pragma once

#include <assert.h>
#include <iostream>
#include <cuda.h>

#include <nv_helpers_gl/profiler.hpp>

#ifdef _DEBUG
# define CHECK_CUDA_CALL(cerr, str) if(cerr != CUDA_SUCCESS)\
                                    {\
                                      std::cout << str << ": " << cerr << "\n";\
                                      assert(false);\
                                    }
#else
# define CHECK_CUDA_CALL(cerr, str) cerr
#endif

namespace nv_helpers_cuda
{
  // General initialization call to pick the best CUDA Device
  inline CUdevice findCudaDeviceDRV(bool verbose = false)
  {
    CUdevice cuDevice;
    int devID = 0;

    // Otherwise pick the device with highest Gflops/s
    char name[100];
    devID = 0;
    CHECK_CUDA_CALL( cuDeviceGet(&cuDevice, devID)
                    , "Couldn't get the device");
    cuDeviceGetName(name, 100, cuDevice);

    if(verbose)
    {
      std::cout << "Using CUDA Device "
                << devID << ": " << name
                << std::endl;
    }

    cuDeviceGet(&cuDevice, devID);

    return cuDevice;
  }

  class AsyncTimer : public nv_helpers_gl::Profiler::GPUInterface
  {
  public:
    AsyncTimer()
    {
      TimerGrow(nv_helpers_gl::Profiler::START_TIMERS);
    }
    ~AsyncTimer();
  public:
    const char* TimerTypeName();
    bool    TimerAvailable( nv_helpers_gl::Profiler::TimerIdx idx );
    void    TimerSetup( nv_helpers_gl::Profiler::TimerIdx idx );
    unsigned long long  TimerResult( nv_helpers_gl::Profiler::TimerIdx idxBegin
                                   , nv_helpers_gl::Profiler::TimerIdx idxEnd );
    void    TimerGrow( unsigned int timers );

  private:

    std::vector<CUevent> m_entries;
  };

}