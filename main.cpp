#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h> // for usleep

float A = 0, B = 0, C = 0;
int width = 160, height = 44;
float cubeSize = 20.0f;
float distanceFromCam = 100.0f;
float K1 = 40.0f;
float horizontalOffset = -2.0f * cubeSize;
float incrementSpeed = 0.6f;

float calcX(int i, int j, int k) {
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C)
         + j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calcY(int i, int j, int k) {
    return j * cos(A) * cos(C) + k * sin(A) * cos(C)
         - j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C)
         - i * cos(B) * sin(C);
}

float calcZ(int i, int j, int k) {
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void renderCube(std::vector<char>& buffer, std::vector<float>& zBuffer) {
    for (float cubeX = -cubeSize; cubeX < cubeSize; cubeX += incrementSpeed) {
        for (float cubeY = -cubeSize; cubeY < cubeSize; cubeY += incrementSpeed) {
            float faces[6][3] = {
                {cubeX, cubeY, -cubeSize},
                {cubeSize, cubeY, cubeX},
                {-cubeSize, cubeY, -cubeX},
                {-cubeX, cubeY, cubeSize}, 
                {cubeX, -cubeSize, -cubeY},
                {cubeX, cubeSize, cubeY}
            };

            char chars[6] = {'@', '$', '~', '#', ';', '+'};

            for (int f = 0; f < 6; ++f) {
                float x = calcX(faces[f][0], faces[f][1], faces[f][2]);
                float y = calcY(faces[f][0], faces[f][1], faces[f][2]);
                float z = calcZ(faces[f][0], faces[f][1], faces[f][2]) + distanceFromCam;

                float ooz = 1.0f / z;
                int xp = int(width / 2 + horizontalOffset + K1 * ooz * x * 2);
                int yp = int(height / 2 + K1 * ooz * y);
                int idx = xp + yp * width;

                if (idx >= 0 && idx < width * height) {
                    if (ooz > zBuffer[idx]) {
                        zBuffer[idx] = ooz;
                        buffer[idx] = chars[f];
                    }
                }
            }
        }
    }
}

int main() {
    std::vector<char> buffer(width * height, ' ');
    std::vector<float> zBuffer(width * height, 0.0f);

    std::cout << "\x1b[2J"; // Clear screen

    while (true) {
        std::fill(buffer.begin(), buffer.end(), ' ');
        std::fill(zBuffer.begin(), zBuffer.end(), 0.0f);

        renderCube(buffer, zBuffer);

        std::cout << "\x1b[H"; // Move cursor to top-left
        for (int i = 0; i < width * height; ++i) {
            std::putchar(i % width ? buffer[i] : '\n');
        }

        A += 0.05f;
        B += 0.05f;
        C += 0.01f;

        usleep(16000); // ~60 FPS
    }

    return 0;
}
