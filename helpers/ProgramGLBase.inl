#pragma once

#include <nv_helpers/misc.hpp>

namespace nv_helpers_gl
{

  enum ShaderType
  {
    ST_VERTEX_SHADER =0,
    ST_TESS_CONTROL_SHADER,
    ST_TESS_EVAL_SHADER,
    ST_GEOMETRY_SHADER,
    ST_FRAGMENT_SHADER,
    ST_COMPUTE_SHADER,
    SHADERTYPE_NUM
  };

  inline GLenum toGLType( ShaderType shaderType )
  {
    switch(shaderType)
    {
    case ST_VERTEX_SHADER:
      return GL_VERTEX_SHADER;
    case ST_TESS_CONTROL_SHADER:
      return GL_TESS_CONTROL_SHADER;
    case ST_TESS_EVAL_SHADER:
      return GL_TESS_EVALUATION_SHADER;
    case ST_GEOMETRY_SHADER:
      return GL_GEOMETRY_SHADER;
    case ST_FRAGMENT_SHADER:
      return GL_FRAGMENT_SHADER;
    case ST_COMPUTE_SHADER:
      return GL_COMPUTE_SHADER;
    default:
      assert(!"Not supported!");
      return 0;
    }
  }

  // Preliminary VertexAttribute specification structs
  //
  typedef pair<const char*, unsigned int> VertexAttributeDesc;

  class VertexAttributes
  {
  public:
    VertexAttributes(){}
    VertexAttributes( const char* name, unsigned int loc )
    {
      m_vas.push_back( VertexAttributeDesc(name, loc) );
    }

    inline VertexAttributes& operator ()( const char* name, unsigned int loc )
    {
      m_vas.push_back( VertexAttributeDesc(name, loc) );
      return *this;
    }

    inline const std::vector<VertexAttributeDesc>& get() const
    {
      return m_vas;
    }

  private:
    std::vector<VertexAttributeDesc> m_vas;
  };


  // ProgramGLBase
  //
  class ProgramGLBase
  {
  public:
    ProgramGLBase()
    {
      m_progName = 0;
      for( unsigned int i = 0; i < SHADERTYPE_NUM; ++i )
      {
        m_shaderName[i] = 0;
      }
    }
    virtual ~ProgramGLBase(){}

  protected:
    GLuint m_progName;
    //TODO: Might consider removing this entirely (but keep in the pipeline stages base)
    GLuint m_shaderName[SHADERTYPE_NUM];

    std::string createShadersAndProgram( const char* vertexShaderFilename
                                  , const char* fragmentShaderFilename )
    {
      std::string progNameStr;
      m_shaderName[ST_VERTEX_SHADER] = compileShader(vertexShaderFilename, ST_VERTEX_SHADER, progNameStr);
      m_shaderName[ST_FRAGMENT_SHADER] = compileShader(fragmentShaderFilename, ST_FRAGMENT_SHADER, progNameStr);

      m_progName = glCreateProgram();
      glAttachShader(m_progName, m_shaderName[ST_VERTEX_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_FRAGMENT_SHADER]);
      glDeleteShader(m_shaderName[ST_VERTEX_SHADER]);
      glDeleteShader(m_shaderName[ST_FRAGMENT_SHADER]);

      return progNameStr;
    }

    std::string createShadersAndProgram( const char* vertexShaderFilename
                                  , const char* geometryShaderFilename
                                  , const char* fragmentShaderFilename )
    {
      std::string progNameStr;
      m_shaderName[ST_VERTEX_SHADER] = compileShader( vertexShaderFilename, ST_VERTEX_SHADER, progNameStr );
      m_shaderName[ST_GEOMETRY_SHADER] = compileShader( geometryShaderFilename, ST_GEOMETRY_SHADER, progNameStr );
      m_shaderName[ST_FRAGMENT_SHADER] = compileShader( fragmentShaderFilename, ST_FRAGMENT_SHADER, progNameStr );

      m_progName = glCreateProgram();
      glAttachShader(m_progName, m_shaderName[ST_VERTEX_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_GEOMETRY_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_FRAGMENT_SHADER]);
      glDeleteShader(m_shaderName[ST_VERTEX_SHADER]);
      glDeleteShader(m_shaderName[ST_GEOMETRY_SHADER]);
      glDeleteShader(m_shaderName[ST_FRAGMENT_SHADER]);

      return progNameStr;
    }

    std::string createShadersAndProgram( const char* vertexShaderFilename
                                  , const char* tesscontrolShaderFilename
                                  , const char* tessevalShaderFilename
                                  , const char* fragmentShaderFilename )
    {
      std::string progNameStr;
      m_shaderName[ST_VERTEX_SHADER] = compileShader( vertexShaderFilename, ST_VERTEX_SHADER, progNameStr );
      m_shaderName[ST_TESS_CONTROL_SHADER] = compileShader( tesscontrolShaderFilename, ST_TESS_CONTROL_SHADER, progNameStr );
      m_shaderName[ST_TESS_EVAL_SHADER] = compileShader( tessevalShaderFilename, ST_TESS_EVAL_SHADER, progNameStr );
      m_shaderName[ST_FRAGMENT_SHADER] = compileShader( fragmentShaderFilename, ST_FRAGMENT_SHADER, progNameStr );

      m_progName = glCreateProgram();
      glAttachShader(m_progName, m_shaderName[ST_VERTEX_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_TESS_CONTROL_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_TESS_EVAL_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_FRAGMENT_SHADER]);
      glDeleteShader(m_shaderName[ST_VERTEX_SHADER]);
      glDeleteShader(m_shaderName[ST_TESS_CONTROL_SHADER]);
      glDeleteShader(m_shaderName[ST_TESS_EVAL_SHADER]);
      glDeleteShader(m_shaderName[ST_FRAGMENT_SHADER]);

      return progNameStr;
    }

    std::string createShadersAndProgram( const char* vertexShaderFilename
                                  , const char* tesscontrolShaderFilename
                                  , const char* tessevalShaderFilename
                                  , const char* geometryShaderFilename
                                  , const char* fragmentShaderFilename )
    {
      std::string progNameStr;
      m_shaderName[ST_VERTEX_SHADER] = compileShader( vertexShaderFilename, ST_VERTEX_SHADER, progNameStr );
      m_shaderName[ST_TESS_CONTROL_SHADER] = compileShader( tesscontrolShaderFilename, ST_TESS_CONTROL_SHADER, progNameStr );
      m_shaderName[ST_TESS_EVAL_SHADER] = compileShader( tessevalShaderFilename, ST_TESS_EVAL_SHADER, progNameStr );
      m_shaderName[ST_GEOMETRY_SHADER] = compileShader( geometryShaderFilename, ST_GEOMETRY_SHADER, progNameStr );
      m_shaderName[ST_FRAGMENT_SHADER] = compileShader( fragmentShaderFilename, ST_FRAGMENT_SHADER, progNameStr );

      m_progName = glCreateProgram();
      glAttachShader(m_progName, m_shaderName[ST_VERTEX_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_TESS_CONTROL_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_TESS_EVAL_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_GEOMETRY_SHADER]);
      glAttachShader(m_progName, m_shaderName[ST_FRAGMENT_SHADER]);
      glDeleteShader(m_shaderName[ST_VERTEX_SHADER]);
      glDeleteShader(m_shaderName[ST_TESS_CONTROL_SHADER]);
      glDeleteShader(m_shaderName[ST_TESS_EVAL_SHADER]);
      glDeleteShader(m_shaderName[ST_GEOMETRY_SHADER]);
      glDeleteShader(m_shaderName[ST_FRAGMENT_SHADER]);

      return progNameStr;
    }

    std::string createShadersAndProgram( const char* computeShaderFilename )
    {
      std::string progNameStr;
      m_shaderName[ST_COMPUTE_SHADER] = compileShader(computeShaderFilename, ST_COMPUTE_SHADER, progNameStr);

      m_progName = glCreateProgram();
      glAttachShader(m_progName, m_shaderName[ST_COMPUTE_SHADER]);
      glDeleteShader(m_shaderName[ST_COMPUTE_SHADER]);

      return progNameStr;
    }

    void bindAttributes( const std::vector<VertexAttributeDesc>& attributeNames, std::string progNameStr = std::string() )
    {
      for( unsigned int i = 0; i < attributeNames.size(); ++i )
      {
        if( attributeNames[i].first )
        {
          glBindAttribLocation(m_progName, attributeNames[i].second, attributeNames[i].first );
        }
      }

      glLinkProgram(m_progName);
      validateProgram(m_progName, progNameStr.c_str());
    }

  private:
    static GLuint compileShader(const char* filepath, ShaderType shaderType, std::string& nameAppend = std::string())
    {
      if(!filepath)
      {
        return 0;
      }

      std::string checkedFilepath = nv_helpers::findFile(filepath, *(std::vector<std::string>*)addSearchDirectory("") );
      std::string shaderSourceStr = nv_helpers::loadFile(checkedFilepath);
      const char* shaderSource = shaderSourceStr.c_str();
      GLuint shaderName = glCreateShader( toGLType(shaderType) );
      assert(shaderName);
      glShaderSource(shaderName, 1, &shaderSource, 0);
      glCompileShader(shaderName);

      std::string filename = nv_helpers::getFileName(filepath);

      GLint shaderLogLength;
      glGetShaderiv(shaderName, GL_INFO_LOG_LENGTH, &shaderLogLength);

      if(shaderLogLength > 1)
      {
        // Print the log:
        std::vector<char> shaderLog(shaderLogLength);
        glGetShaderInfoLog(shaderName, shaderLogLength, 0, &shaderLog[0]);

        printf("%s:\n", filename.c_str());
        for(int i = 0; i < filename.size(); i++) printf("-");
        printf("\n%s\n", &shaderLog[0]);
      }

      // Check if it compiled
      GLint status;
      glGetShaderiv(shaderName, GL_COMPILE_STATUS, &status);
      if(status != GL_TRUE)
      {
        glDeleteShader(shaderName);
        return 0;
      }

      if(!nameAppend.empty())
      {
        nameAppend += "/";
      }
      nameAppend += filename;

      return shaderName;
    }

    static GLuint createProgram(GLuint vertexShader, GLuint fragmentShader)
    {
      GLuint progName = glCreateProgram();
      assert(progName);
      glAttachShader(progName, vertexShader);
      glAttachShader(progName, fragmentShader);
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);

      return progName;
    }

    static GLuint createProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader)
    {
      GLuint progName = glCreateProgram();
      assert(progName);
      glAttachShader(progName, vertexShader);
      glAttachShader(progName, geometryShader);
      glAttachShader(progName, fragmentShader);
      glDeleteShader(vertexShader);
      glDeleteShader(geometryShader);
      glDeleteShader(fragmentShader);

      return progName;
    }

    static GLuint createProgram(GLuint computeShader)
    {
      GLuint progName = glCreateProgram();
      assert(progName);
      glAttachShader(progName, computeShader);
      glDeleteShader(computeShader);

      return progName;
    }

    static bool validateProgram(GLuint programName, const char* nameStr)
    {
      // Print the log:
      GLint programLogLength;
      glGetProgramiv(programName, GL_INFO_LOG_LENGTH, &programLogLength);

      if(programLogLength > 1)
      {
        // Print the log:
        std::vector<char> programLog(programLogLength);
        glGetProgramInfoLog( programName, programLogLength, 0, &programLog[0] );

        printf("%s:\n", nameStr);
        for(int i = 0; i < strlen(nameStr); i++) printf("-");
        printf("\n%s\n", &programLog[0]);
      }

      return true;
    }

  public:

    inline void use()
    {
      glUseProgram(m_progName);
    }

    //TODO: Really bad style but I don't want to deal with global variables or for every instance to reference a static variable.
    static const void* addSearchDirectory(const char* searchDir)
    {
      static std::vector<std::string> g_searchDirs;

      if( searchDir )
      {
        g_searchDirs.push_back(searchDir);
      }

      return &g_searchDirs;
    }
  };

} //namespace nv_gl_helpers.