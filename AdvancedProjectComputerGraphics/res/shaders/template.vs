#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;

out vec4 v_Color;
uniform mat4 u_Projection;
uniform mat4 u_Transformation;

void main()
{
    gl_Position = u_Projection * u_Transformation * aPos;
    v_Color = aColor;  
}