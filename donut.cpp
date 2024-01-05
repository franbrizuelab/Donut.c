#include <stdio.h>
#include <math.h>
#include <string.h>

const char newline = '\n';
const float theta_step = 0.07;
const float phi_step = 0.02;
const char* shade_chars = ".,-~:;=!*#$@";
float TWO_PI = 6.28;

const float R1 = 1;
const float R2 = 2;
const float K2 = 5;

int main() {
    float A = 0;
    float B = 0;
    const size_t width = 25;
    const size_t height = 25;
    const size_t buffer_size = height * width;
    float zbuf[width][height];
    char buffer[width][height];
    //printf("\x1b[2J");
    for (;;) {
        memset(buffer, ' ', buffer_size);
        memset(zbuf, 0, buffer_size * sizeof(float));
        const float K1 = width * K2 * 3 / (8 * (R1 + R2));
        float cos_B = cos(B);
        float sin_B = sin(B);
        float sin_A = sin(A);
        float cos_A = cos(A);
        for (float theta = 0; theta < TWO_PI; theta += theta_step) {
            float cos_theta = cos(theta);
            float sin_theta = sin(theta);
            for (float phi = 0; phi < TWO_PI; phi += phi_step) {
                float sin_phi = sin(phi);
                float cos_phi = cos(phi);
                // the x,y coordinate of the circle, before revolving
                float circlex = R2 + R1 * cos_theta;
                float circley = R1 * sin_theta;
                // final 3D (x,y,z) coordinate after rotations, directly from the equations
                float x = circlex * (cos_B * cos_phi + sin_A * sin_B * sin_phi)
                    - circley * cos_A * sin_B;
                float y = circlex * (sin_B * cos_phi - sin_A * cos_B * sin_phi)
                    + circley * cos_A * cos_B;
                float z = K2 + cos_A * circlex * sin_phi + circley * sin_A;
                float ooz = 1 / z;  // "one over z"

                // x and y projection.  note that y is negated here, because y
                // goes up in 3D space but down on 2D displays.
                int xp = (int)(width / 2 + K1 * ooz * x);
                int yp = (int)(height / 2 + K1 * ooz * y);

                // calculate luminance.  ugly, but correct.
                float L = cos_phi * cos_theta * sin_B - cos_A * cos_theta * sin_phi -
                    sin_A * sin_theta + cos_B * (cos_A * sin_theta - cos_theta * sin_A * sin_phi);
                // L ranges from -sqrt(2) to +sqrt(2).  If it's < 0, the surface
                // is pointing away from us, so we won't bother trying to plot it.
                if (L > 0) {
                    // test against the z-buffer.  larger 1/z means the pixel is
                    // closer to the viewer than what's already plotted.

                    if (zbuf[xp][yp] < ooz) {
                        zbuf[xp][yp] = ooz;
                        int luminance_index = L * 8;
                        // luminance_index is now in the range 0..11 (8*sqrt(2) = 11.3)
                        // now we lookup the character corresponding to the
                        // luminance and plot it in our output:
                        buffer[xp][yp] = shade_chars[luminance_index];
                    }
                }
            }
        }


        //  display_buffer(canvas);
        printf("\x1b[H");
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                putchar(buffer[i][j]);
            }
            putchar('\n');
        }
        A += 0.05;
        B += 0.04;
    }
}