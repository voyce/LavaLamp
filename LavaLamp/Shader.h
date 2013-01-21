//
//  Shader.h
//  glpanes
//
//  Created by Ian Voyce on 20/12/2012.
//  Copyright (c) 2012 Ian Voyce. All rights reserved.
//

#ifndef __glpanes__Shader__
#define __glpanes__Shader__

#include <iostream>

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/glext.h>

#include <map>
#include <string>

using namespace std;

class Shader {
public:
    Shader():_program(0) {}
    
    ~Shader()
    {
        if (_program) {
            glDeleteProgram(_program);
            _program = 0;
        }
    }
//    bool Load(const char *vert, const char *frag);
//    bool Link();
    
    bool Validate()
    {
        GLint logLength, status;
        
        glValidateProgram(_program);
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(_program, logLength, &logLength, log);
            printf("%s",log);
            free(log);
        }
        
        glGetProgramiv(_program, GL_VALIDATE_STATUS, &status);
        if (status == 0) {
            return false;
        }
        
        return true;
    }

    bool Load(const char *vert, const char *frag)
    {
        GLuint vertShader = 0, fragShader = 0;
        
        // Create shader program.
        _program = glCreateProgram();
        
        // Create and compile vertex shader.
        if (!Compile( GL_VERTEX_SHADER, vert, vertShader)) {
            return false;
        }
        
        // Create and compile fragment shader.
        if (!Compile(GL_FRAGMENT_SHADER, frag, fragShader)) {
            return false;
        }
        
        // Attach vertex shader to program.
        glAttachShader(_program, vertShader);
        
        // Attach fragment shader to program.
        glAttachShader(_program, fragShader);
        
        // Bind attribute locations.
        // This needs to be done prior to linking.
        glBindAttribLocation(_program, 0, "position");
        
        // Link program.
        if (!Link()) {
            if (vertShader) {
                glDeleteShader(vertShader);
                vertShader = 0;
            }
            if (fragShader) {
                glDeleteShader(fragShader);
                fragShader = 0;
            }
            if (_program) {
                glDeleteProgram(_program);
                _program = 0;
            }
            
            return false;
        }
        
        // Release vertex and fragment shaders.
        if (vertShader) {
            glDetachShader(_program, vertShader);
            glDeleteShader(vertShader);
        }
        if (fragShader) {
            glDetachShader(_program, fragShader);
            glDeleteShader(fragShader);
        }
        
        return true;
    }
    
    GLint AddUniform(string name)
    {
        if (_uniforms.find(name) == _uniforms.end())
        {
            GLint n = glGetUniformLocation(_program, name.c_str());
            if (n == -1)
                return n;
            _uniforms[name] = n;
            return n;
        }
        else
            return _uniforms[name];
    }
    
    
    bool Link()
    {
        GLint status;
        glLinkProgram(_program);
        
#if defined(DEBUG)
        GLint logLength;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(_program, logLength, &logLength, log);
            printf("%s",log);
            free(log);
        }
#endif
        
        glGetProgramiv(_program, GL_LINK_STATUS, &status);
        if (status == 0) {
            return false;
        }
        
        return true;
    }

    void Use()
    {
        glUseProgram(_program);
    }
private:
    bool Compile(GLenum type, const char *src, GLuint &shader)
    {
        assert(src != NULL);
        GLint status;
        
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);
        
#if defined(DEBUG)
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            printf("%s",log);
            free(log);
        }
#endif
        
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == 0) {
            glDeleteShader(shader);
            return false;
        }
        
        return true;
    }
    GLuint _program;
    map<string, int> _uniforms;
};





#endif /* defined(__glpanes__Shader__) */
