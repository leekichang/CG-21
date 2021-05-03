#version 330 core
layout (location = 0) in vec3 aPos;
uniform float accumulatedDeltaX;
uniform float accumulatedDeltaY;

void main()
{
    gl_Position = vec4(aPos[0] + accumulatedDeltaX, aPos[1] + accumulatedDeltaY, aPos[2], 1.0);
}

