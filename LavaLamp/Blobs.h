//
//  Blobs.h
//  glpanes
//
//  Created by Ian Voyce on 10/01/2013.
//  Copyright (c) 2013 Ian Voyce. All rights reserved.
//

#ifndef __glpanes__Blobs__
#define __glpanes__Blobs__

#include <iostream>
#include <algorithm>
#include <vector>
//#include <boost/bind.hpp>
//#include <boost/mem_fn.hpp>
#include <math.h>


//#include "Scene.h"
#include "Shader.h"

using namespace std;

class Blobs
{
    float _width, _height;
    float _minz, _maxz;
    GLint _uballs;//, _unumballs;
    GLint _uwidth;
    Shader *_shader;
    
    struct vd
    {
        float x;
        float y;
        float z;
        float dx;
        float dy;
        float dz;
        bool stat;
    };
    vector<vd> _balls;

    typedef struct _vertexStruct
    {
        GLfloat position[3];
        GLfloat tex[2];
    } vertex;
    
    GLuint _vao;
    GLuint _buffer;//[2];
    
    void PrepareQuad(float w, float h)
    {
        vertex vertices[4];
        
        CGPoint v0 = CGPointMake(-(w/2),(h/2));
        CGPoint v1 = CGPointMake(-(w/2),-(h/2));
        CGPoint v2 = CGPointMake((w/2),(h/2));
        CGPoint v3 = CGPointMake((w/2),-(h/2));
        
        float z = 0.0;
        vertices[0].position[0] = v0.x;
        vertices[0].position[1] = v0.y;
        vertices[0].position[2] = z;
        vertices[0].tex[0] = 0.;
        vertices[0].tex[1] = 0.;
        
        vertices[1].position[0] = v1.x;
        vertices[1].position[1] = v1.y;
        vertices[1].position[2] = z;
        vertices[1].tex[0] = 0.;
        vertices[1].tex[1] = 1.0;
        
        vertices[2].position[0] = v2.x;
        vertices[2].position[1] = v2.y;
        vertices[2].position[2] = z;
        vertices[2].tex[0] = 1.;
        vertices[2].tex[1] = 0.;
        
        vertices[3].position[0] = v3.x;
        vertices[3].position[1] = v3.y;
        vertices[3].position[2] = z;
        vertices[3].tex[0] = 1.;
        vertices[3].tex[1] = 1.0;
        
        glGenVertexArraysOES(1, &_vao);
        glBindVertexArrayOES(_vao);
        
        glGenBuffers(1, &_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, _buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex,position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex,tex));
        
        glBindVertexArrayOES(0);
    }
    
    void DrawQuad()
    {
        assert(_vao != -1);
        glBindVertexArrayOES(_vao);
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glBindVertexArrayOES(0);
        
    }

public:
    Blobs(const char *vsource, const char *fsource, float width, float height):
    _width(width), _height(height)
    {
        _shader = new Shader();
        _shader->Load(vsource, fsource);
        
        GLint unumballs = _shader->AddUniform("numballs");
        _uballs = _shader->AddUniform("balls");
        
        GLint unimvp = _shader->AddUniform("modelViewProjectionMatrix");
        _uwidth = _shader->AddUniform("u_width");
        
        PrepareQuad(width, height);
        
        srand(0);//time(NULL));
        
        _minz = height/6;
        _maxz = height/3;
        while (_balls.size() < 5)
        {
            vd v;
            v.x = ((float)rand()/(float)RAND_MAX) * _width;
            v.z = _minz + (((float)rand()/(float)RAND_MAX) * _maxz);
            v.y = (0 - ((float)rand()/(float)RAND_MAX) * 100.0);
            v.dx = -0.15 + (((float)rand()/(float)RAND_MAX) * 0.3);
            v.dy = 0.25 + (((float)rand()/(float)RAND_MAX) * 1.5);
            //v.dy *= 10;
            v.dz = -0.1 + (((float)rand()/(float)RAND_MAX) * 0.2);
            v.stat = false;
            printf("%f,%f r:%f dx:%f,dy:%f,dz:%f\n", v.x, v.y, v.z, v.dx, v.dy, v.dz);
            _balls.push_back(v);
        }
        
        // Set some non-changing values in the shader
        _shader->Use();

        glUniformMatrix4fv(unimvp, 1, 0, GLKMatrix4Identity.m);
        glUniform1i(unumballs, _balls.size());
        glUniform1f(_uwidth, _width);
    }
    
    void Tick()
    {
        for(int n=0; n < _balls.size(); n++)
        {
            if (_balls[n].stat)
                continue;
            _balls[n].x = fminf(fmaxf(_balls[n].x+_balls[n].dx, 0.0), _width);
            _balls[n].y += _balls[n].dy;
            //_balls[n].z += fmaxf(fminf(_balls[n].dz, 300), 0.1);
            _balls[n].z = _balls[n].z+_balls[n].dz;
            if (_balls[n].z <= _minz || _balls[n].z >= _maxz)
                _balls[n].dz = -_balls[n].dz;
        }
        
        for(vector<vd>::iterator it = _balls.begin(); it != _balls.end(); it++)
        {
            if (it->stat)
                continue;
            
            // at the top
            if ((*it).y > _height) {
                // and going up
                if (it->dy > 0.0) {
                    // change direction
                    it->dy = -(it->dy);// -(0.25 + (((float)rand()/(float)RAND_MAX) * 1.5));
                    it->dz = -0.1 + (((float)rand()/(float)RAND_MAX) * 0.2);
                    it->dx = -0.15 + (((float)rand()/(float)RAND_MAX) * 0.3);
                }
            }
            // near the top?
            else if ((*it).y > _height-it->z) {
                if (it->dy > 0.0) {
                    //it->dy *= 0.9;
                    it->dy = fmaxf(it->dy * 0.985, 0.25);
                }
                else {
                    it->dy *= 1.020;
                }
            }
            // at the bottom and going down
            else if ((*it).y < -40) {
                if(it->dy < 0.0) {
                    it->dy = 0.25 + (((float)rand()/(float)RAND_MAX) * 1.5);
                    it->dz = -0.1 + (((float)rand()/(float)RAND_MAX) * 0.2);
                    it->dx = -0.15 + (((float)rand()/(float)RAND_MAX) * 0.3);
                }
            }
            // near the bottom
            else if ((*it).y < 0) {
                if (it->dy > 0.0) {
                    it->dy += 0.01;
                }
                else {
                    it->dy -= 0.01;
                }
            }
        }
    }
    
    void Draw()
    {
        _shader->Use();

        float v[_balls.size() * 3];
        for(int n=0; n < _balls.size(); n++)
            memcpy(&v[n * 3], &_balls[n], sizeof(float)*3);
        glUniform3fv(_uballs, _balls.size() * 3, (float *)v);
        
        DrawQuad();
    }
};

#endif /* defined(__glpanes__Blobs__) */
