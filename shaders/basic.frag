#version 330 core
out vec4 FragColor;

in vec3 vNormal;
in vec3 vPos;

uniform vec3 lightPos[3];
uniform vec3 lightColor[3];

uniform vec3 ambientColor;
uniform vec3 uColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 col = ambientColor;

    for (int i = 0; i < 3; i++) {
        vec3 L = normalize(lightPos[i] - vPos);
        float diff = max(dot(N, L), 0.0);
        col += diff * lightColor[i];
    }

    col *= uColor;

    FragColor = vec4(col, 1.0);
}
