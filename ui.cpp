#include "gl_cu_interop_pingpong_st.h"

#include <nv_helpers/anttweakbar.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::beginUI()
{
  TwInit(TW_OPENGL,NULL);
  TwWindowSize(m_window.m_viewsize[0],m_window.m_viewsize[1]);

  TwBar *bar = TwNewBar("mainbar");
  TwDefine("GLOBAL"
           " contained=true       "
           " help='OpenGL samples.\nCopyright NVIDIA Corporation 2015' "
          );
  TwDefine("mainbar"
           " help=  'This sample demonstrates texture ping-ponging by "
                    "running a simple time-based diffusive heat transfer "
                    "simulation on a cube volume in CUDA and visualizing "
                    "it in OpenGL. \n"
                    "To compute the temperature scalar field for a given "
                    "moment in time, we require the scalar field previously "
                    "computed in the last iteration. For this it is necessary "
                    "to swap read/write textures before each iteration.'"
           " position='0 0'     "
           " color='119 185 0'  "
           " text=light         "
           " alpha=64           "
           " buttonalign=center "
          );

  TwDefine( " mainbar label='Controls'" );

  TwAddButton(bar, "ctlStart", Sample::_simStart, this, " label='START'");
  TwAddButton(bar, "ctlPause", Sample::_simPause, this, " label='PAUSE'");
  TwAddButton(bar, "ctlReset", Sample::_simReset, this, " label='RESET'");
  TwAddSeparator(bar, "controlsEnd", NULL);

  TwAddVarRW(bar, "colLow", TW_TYPE_COLOR3F, m_colLow.get_value(), " label='Low' colormode=rgb ");
  TwAddVarRW(bar, "colHigh", TW_TYPE_COLOR3F, m_colHigh.get_value(), " label='High' colormode=rgb ");

  TwAddVarRW(bar, "thicknessFactor", TW_TYPE_FLOAT,  &m_thicknessFactor,     " label='Thickness'  min=0.0 step=0.02 ");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::renderUI()
{
  NV_PROFILE_SECTION("TwDraw");
  TwDraw();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::endUI()
{
  TwTerminate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Sample::resizeUI(int width, int height)
{
  TwWindowSize(width,height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

// return true to prevent m_window updates
bool Sample::mouse_pos(int x, int y)
{
  return !!TwEventMousePosGLFW(x,y); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

bool Sample::mouse_button(int button, int action)
{
  return !!TwEventMouseButtonGLFW(button, action);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

bool Sample::mouse_wheel(int wheel)
{
  return !!TwEventMouseWheelGLFW(wheel); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

bool Sample::key_button(int button, int action, int mods)
{
  return nv_helpers::handleTwKeyPressed(button,action,mods);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void TW_CALL Sample::_simStart(void* inst)
{
  static_cast<Sample*>(inst)->simStart();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void TW_CALL Sample::_simPause(void* inst)
{
  static_cast<Sample*>(inst)->simPause();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void TW_CALL Sample::_simReset(void* inst)
{
  static_cast<Sample*>(inst)->simReset();
}