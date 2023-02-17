#include "time.h"
const float theta_spacing = 0.07;
const float phi_spacing = 0.02;
const int screen_width = 80;
const int screen_height = 22;
const float R1 = 1; // radius of circle
const float R2 = 2; // center of circle
const float K2 = 5; // the constant to move torus in front of the viewer

// calculate K1 based on screen width the maximum x value is R1+R2, 
// z = 0 and we want it displaced 1/4 of the screen width, so
// K1 * (R1+R2) / (K2+0) = screen width * 1/4
// K1 = screen width * K2 / 4(R1+R2)
const float K1 = (screen_width * K2) / (4 * (R1 + R2));
// const float K1 = 15;
const float PI = 6.283185;

void render_frame(float A, float B) {
    // Precompute cosines and sines of A and B
    float cosA = cos(A), sinA = sin(A);
    float cosB = cos(B), sinB = sin(B);

    char output[screen_width * screen_height];
    float zBuffer[screen_width * screen_height];
    
    // initialize output and zBuffer
    memset(output, 32, sizeof(char) * screen_width * screen_height);
    memset(zBuffer, 0, sizeof(float) * screen_width * screen_height);

    // theta goes around the cross-sectional circle of a torus
    for (float theta = 0; theta <= PI; theta += theta_spacing) {
        // Precompute cosine and sine of theta
        float cosTheta = cos(theta), sinTheta = sin(theta);
        
        // phi goes around the center of revolution of a torus
        for (float phi = 0; phi <= PI; phi += phi_spacing) {
            // Precompute cosine and sine of phi
            float cosPhi = cos(phi), sinPhi = sin(phi);
            
            // the x, y coordinate of the circle
            float circleX = R2 + R1 * cosTheta;
            float circleY = R1 * sinTheta;

            // final 3D (x, y, z) coordinate after rotations
            float x = circleX * (cosPhi*cosB + sinPhi*sinA*sinB)
                + circleY*cosA*sinB;
            float y = circleX * (-cosPhi*sinB + sinPhi*sinA*cosB) 
                + circleY*cosA*cosB;
            float z = K2 + circleX*sinPhi*cosA - circleY*sinA;
            
            float ooz = 1 / z; // one over z

            // x and y projection. note that y is negated here because y goes up in
            // 3D space but down on 2D displays
            int xp = (int) (screen_width/2 + K1*x*ooz);
            int yp = (int) (screen_height/2 - K1/2*y*ooz);

            // calculate illuminance
            // assume light direction is (0, 1, -1)
            float L = cosTheta * (-cosPhi*sinB + sinPhi*sinA*cosB)
                + cosA * (sinTheta*cosB + cosTheta*sinPhi) - sinTheta*sinA; 

            // test against the z-buffer. larger 1/z means the pixel is closer to the viewer than what's
            // already plotted
            int D = yp * screen_width + xp;
            if (screen_width > xp && xp > 0 && yp > 0 && screen_height > yp
                && ooz > zBuffer[D]) {
                zBuffer[D] = ooz; 
                int luminance_index = 8 * L;
                // we do the correct for light direction here, since it should be a unit vector, while 
                // its magnitude is sqrt(2)
                // we multiply L by 8, so the range of luminance_index is  now 0 - 11 (sqrt(2) * 8 =  11.3)
                output[D] = ".,-~:;=!*#$@"[luminance_index < 0 ? -luminance_index : 0];
            }
        }
    }

    printf("\x1b[H");
    for (int k = 0; k < screen_width * screen_height + 1; k++)
        putchar(k % screen_width ? output[k] : 10);

}

int main() {
    float A = 0, B = 0;
    int c = 17;
    _Bool change = 1;
    time_t startTime, endTime, exTime, temp;

    printf("\x1b[2J");
    startTime = time(NULL);

    for(;;) {  
        if (change) {
            printf("\x1b[38;5;%dm", c);
            c++; 
            if (c > 231) c = 17;
            change = 0;
        }
       render_frame(A, B);
       endTime = time(NULL);
       temp = endTime - startTime;
       exTime = (exTime > temp) ? exTime : temp;

       if (!(exTime % 2)) {
           change = 1;
           exTime++;
       }
        
       A += 0.04;
       B += 0.02; 
       usleep(30000);
    }

    return 0;
}
