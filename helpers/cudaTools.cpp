#pragma once

#include "cudaTools.hpp"

namespace nv_helpers_cuda
{
  const char* AsyncTimer::TimerTypeName()
  {
    static const char cudaStr[] = "CUDA";
    return cudaStr;
  }

  bool AsyncTimer::TimerAvailable( nv_helpers_gl::Profiler::TimerIdx idx )
  {
    CUresult cuResult = cuEventQuery( (CUevent)m_entries[idx] );
    
    bool resultsAvailable = cuResult == CUDA_SUCCESS;
    assert( cuResult == CUDA_ERROR_NOT_READY || resultsAvailable );
    return resultsAvailable;
  }

  void AsyncTimer::TimerSetup( nv_helpers_gl::Profiler::TimerIdx idx )
  {
    //Only use stream 0 for now
    CHECK_CUDA_CALL( cuEventRecord( (CUevent)m_entries[idx], 0 ), "Failed to create CUDA event" );
  }

  unsigned long long AsyncTimer::TimerResult( nv_helpers_gl::Profiler::TimerIdx idxBegin
                                            , nv_helpers_gl::Profiler::TimerIdx idxEnd )
  {
    float milliseconds;
    cuEventElapsedTime( &milliseconds, (CUevent)m_entries[idxBegin], (CUevent)m_entries[idxEnd] );

    unsigned long long nanoseconds = unsigned long long(milliseconds * 1000000.0f);
    return nanoseconds;
  }

  void AsyncTimer::TimerGrow( unsigned int timers )
  {
    size_t prevSize = m_entries.size();
    m_entries.resize( prevSize + timers, 0 );
    for ( size_t i = prevSize; i < m_entries.size(); i++ )
    {
      CHECK_CUDA_CALL( cuEventCreate( (CUevent*)&m_entries[i], CU_EVENT_BLOCKING_SYNC ), "Failed to create CUDA event" );
    }
  }

  AsyncTimer::~AsyncTimer()
  {
    for ( std::vector<CUevent>::const_iterator it = m_entries.begin()
        ; it != m_entries.end()
        ; it++ )
    {
      cuEventDestroy( (CUevent)*it );
    }
  }

}