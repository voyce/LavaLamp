#define MAX_BALLS 10
precision mediump float;

varying lowp vec2 v_texCoord;

uniform float u_width;
uniform int numballs;
uniform vec3 balls[MAX_BALLS]; //.xy is position .z is radius

float energyField(in vec2 p, in float iso)
{
    float en = 0.0;
    bool result = false;
    for(int i=0; i < numballs; ++i)
    {
        float dist = length(vec2(balls[i].xy - p));
        float radius = balls[i].z;
        // avoid div by zero
        float denom =  max(0.0001, dist);
        en += (radius / denom);
    }
    return (en - iso);
}

void main()
{
    vec2 t = v_texCoord;
    float power = energyField(gl_FragCoord.xy, 3.0);
    // got the power, now add our 'flourish'...
    float hwidth = u_width/2.0;
    // rescale from 0 -> width to -1 -> 1
    float left = 1.0 - (gl_FragCoord.x/hwidth);
    left = (left * left) * (left * left);
    gl_FragColor = vec4(power-(left*2.0), power-left, power-left, left+0.5);
}