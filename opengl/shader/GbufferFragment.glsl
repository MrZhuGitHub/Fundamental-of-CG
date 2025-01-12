#version 330 core
precision highp float;

out highp vec4 FragColor;

in vec4 normal;
in vec4 vertexPosition;

uniform bool enableDepth;

void main()
{    
    highp float depth = vertexPosition.z/vertexPosition.w;

    // depth = (depth + 1.0)/2.0;

    if (enableDepth) {
        FragColor = vec4(normalize(normal.xyz), depth);
    } else {
        FragColor = vec4(normalize(normal.xyz), 1.0);
    }

    // FragColor = vec4(depth, depth, depth, 1.0);

}