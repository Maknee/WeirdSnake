#version 330 core

uniform vec3 Color;

void main()
{
    color = vec4(Color, 1.0f);
}