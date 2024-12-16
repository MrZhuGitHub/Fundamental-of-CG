#version 330 core
out vec4 FragColor;

in vec4 normal;
in vec4 vertexPosition;

void main()
{    
    float depth = vertexPosition.z/vertexPosition.w;
    //depth = (depth - 0.999)*1000.0;
    FragColor = vec4(normalize(normal.xyz), depth);
}