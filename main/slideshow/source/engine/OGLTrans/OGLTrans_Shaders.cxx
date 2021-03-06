/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include <osl/diagnose.hxx>

#include "OGLTrans_Shaders.hxx"

namespace unx
{
#include <GL/glx.h>
#include <GL/glxext.h>
}

bool OGLShaders::cbInitialized = false;

#ifdef GL_VERSION_2_0

PFNGLCREATESHADERPROC OGLShaders::glCreateShader = NULL;
PFNGLSHADERSOURCEPROC OGLShaders::glShaderSource = NULL;
PFNGLCOMPILESHADERPROC OGLShaders::glCompileShader = NULL;
PFNGLGETSHADERIVPROC OGLShaders::glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC OGLShaders::glGetShaderInfoLog = NULL;
PFNGLDELETESHADERPROC OGLShaders::glDeleteShader = NULL;
PFNGLCREATEPROGRAMPROC OGLShaders::glCreateProgram = NULL;
PFNGLATTACHSHADERPROC OGLShaders::glAttachShader = NULL;
PFNGLLINKPROGRAMPROC OGLShaders::glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC OGLShaders::glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC OGLShaders::glGetProgramInfoLog = NULL;
PFNGLUSEPROGRAMPROC OGLShaders::glUseProgram = NULL;
PFNGLDELETEPROGRAMPROC OGLShaders::glDeleteProgram = NULL;
PFNGLGETUNIFORMLOCATIONPROC OGLShaders::glGetUniformLocation = NULL;
PFNGLUNIFORM1IPROC OGLShaders::glUniform1i = NULL;
PFNGLUNIFORM1FPROC OGLShaders::glUniform1f = NULL;
#endif

bool OGLShaders::Initialize()
{
#ifdef GL_VERSION_2_0
    if( !cbInitialized ) {
        glCreateShader = (PFNGLCREATESHADERPROC) unx::glXGetProcAddress( (unsigned char *) "glCreateShader" );
        glShaderSource = (PFNGLSHADERSOURCEPROC) unx::glXGetProcAddress( (unsigned char *) "glShaderSource" );
        glCompileShader = (PFNGLCOMPILESHADERPROC) unx::glXGetProcAddress( (unsigned char *) "glCompileShader" );
        glGetShaderiv = (PFNGLGETSHADERIVPROC) unx::glXGetProcAddress( (unsigned char *) "glGetShaderiv" );
        glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) unx::glXGetProcAddress( (unsigned char *) "glGetShaderInfoLog" );
        glDeleteShader = (PFNGLDELETESHADERPROC) unx::glXGetProcAddress( (unsigned char *) "glDeleteShader" );
        glCreateProgram = (PFNGLCREATEPROGRAMPROC) unx::glXGetProcAddress( (unsigned char *) "glCreateProgram" );
        glAttachShader = (PFNGLATTACHSHADERPROC) unx::glXGetProcAddress( (unsigned char *) "glAttachShader" );
        glLinkProgram = (PFNGLLINKPROGRAMPROC) unx::glXGetProcAddress( (unsigned char *) "glLinkProgram" );
        glGetProgramiv = (PFNGLGETPROGRAMIVPROC) unx::glXGetProcAddress( (unsigned char *) "glGetProgramiv" );
        glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) unx::glXGetProcAddress( (unsigned char *) "glGetProgramInfoLog" );
        glUseProgram = (PFNGLUSEPROGRAMPROC) unx::glXGetProcAddress( (unsigned char *) "glUseProgram" );
        glDeleteProgram = (PFNGLDELETEPROGRAMPROC) unx::glXGetProcAddress( (unsigned char *) "glDeleteProgram" );
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) unx::glXGetProcAddress( (unsigned char *) "glGetUniformLocation" );
        glUniform1i = (PFNGLUNIFORM1IPROC) unx::glXGetProcAddress( (unsigned char *) "glUniform1i" );
        glUniform1f = (PFNGLUNIFORM1FPROC) unx::glXGetProcAddress( (unsigned char *) "glUniform1f" );
        cbInitialized = true;
    }

    return glCreateShader != NULL;
#else
    return false;
#endif
}

GLuint OGLShaders::LinkProgram( const char *vertexShader, const char *fragmentShader )
{
#ifdef GL_VERSION_2_0
    if( !Initialize() )
        return 0;

    GLhandleARB vertexObject, fragmentObject, programObject;
    GLint vertexCompiled, fragmentCompiled, programLinked;
    char log[1024];

    vertexObject = glCreateShader( GL_VERTEX_SHADER );
    fragmentObject = glCreateShader( GL_FRAGMENT_SHADER );
    OSL_TRACE("checkpoint 1: shaders created (%d) vertex: %d fragment: %d", glGetError() == GL_NO_ERROR, vertexObject, fragmentObject );


    glShaderSource( vertexObject, 1, &vertexShader, NULL );
    glShaderSource( fragmentObject, 1, &fragmentShader, NULL );

    glCompileShader( vertexObject );
    glGetShaderInfoLog( vertexObject, sizeof( log ), NULL, log );
    OSL_TRACE("vertex compile log: %s", log);
    glGetShaderiv( vertexObject, GL_COMPILE_STATUS, &vertexCompiled );
    glCompileShader( fragmentObject );
    glGetShaderInfoLog( fragmentObject, sizeof( log ), NULL, log );
    OSL_TRACE("fragment compile log: %s", log);
    glGetShaderiv( fragmentObject, GL_COMPILE_STATUS, &fragmentCompiled );

    if( !vertexCompiled || !fragmentCompiled )
        return 0;

    OSL_TRACE("checkpoint 2: shaders compiled (%d)", glGetError() == GL_NO_ERROR);

    programObject = glCreateProgram();
    glAttachShader( programObject, vertexObject );
    glAttachShader( programObject, fragmentObject );

    glLinkProgram( programObject );
    glGetProgramInfoLog( programObject, sizeof( log ), NULL, log );
    OSL_TRACE("program link log: %s", log);
    glGetProgramiv( programObject, GL_LINK_STATUS, &programLinked );

    if( !programLinked )
        return 0;

    OSL_TRACE("checkpoint 3: program linked (%d)", glGetError() == GL_NO_ERROR);

    return programObject;
#else
    return 0;
#endif
}

