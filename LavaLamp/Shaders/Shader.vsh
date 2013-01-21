attribute vec4 position;
attribute vec2 texcoord;

varying lowp vec2 v_texCoord;

uniform mat4 modelViewProjectionMatrix;

void main()
{
    gl_Position = modelViewProjectionMatrix * position;
    v_texCoord = texcoord.xy;
}
