//
//  Shader.fsh
//  LavaLamp
//
//  Created by Ian Voyce on 16/01/2013.
//  Copyright (c) 2013 Ian Voyce. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
