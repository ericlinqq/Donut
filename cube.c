#define screen_width 80
#define screen_height 22

struct Angle {
    float cosA;
    float sinA;
    float cosB;
    float sinB;
    float cosC;
    float sinC;
};

const float spacing = 0.1;
const float K2 = 60; // the constant to move torus in front of the viewer
const float cubeWidth = 8;
const float K1 = 40;

float x, y, z;
float ooz;
int xp, yp;
int idx;

char output[screen_width*screen_height];
float zBuffer[screen_width*screen_height];
char* colorBuffer[screen_width*screen_height];

float calculateX(struct Angle info, float i, float j, float k) {
    return i*info.cosB*info.cosC + j * (info.sinA*info.sinB*info.cosC + info.cosA*info.sinC) 
        - k * (-info.cosA*info.sinB*info.cosC + info.sinA*info.sinC);
}

float calculateY(struct Angle info, float i, float j, float k) {
    return -i*info.cosB*info.sinC + j * (-info.sinA*info.sinB*info.sinC + info.cosA*info.cosC)
        - k * (info.cosA*info.sinB*info.sinC + info.sinA*info.cosC);
}

float calculateZ(struct Angle info, float i, float j, float k) {
    return K2 + i*info.sinB - j*info.sinA*info.cosB - k*info.cosA*info.cosB;
}

void calculateSurface(struct Angle info, float cubeX, float cubeY, float cubeZ, char ch, char* color) {
    x = calculateX(info, cubeX, cubeY, cubeZ);
    y = calculateY(info, cubeX, cubeY, cubeZ);
    z = calculateZ(info, cubeX, cubeY, cubeZ);

    ooz = 1/z;

    xp = (int)(screen_width/2 + 2*K1*x*ooz);
    yp = (int)(screen_height/2 - K1*y*ooz);

    idx = yp * screen_width + xp;

    if (screen_width > xp && xp > 0 && yp > 0 && screen_height > yp 
            && ooz > zBuffer[idx]) {
        zBuffer[idx] = ooz;
        output[idx] = ch;
        colorBuffer[idx] = color;
    }
}

void render_frame(float A, float B, float C) {
    // Precompute cosines and sines of A, B and C
    struct Angle info;
    info.cosA = cos(A); info.sinA = sin(A);
    info.cosB = cos(B); info.sinB = sin(B);
    info.cosC = cos(C); info.sinC = sin(C);
    
    // initialize output and zBuffer
    memset(output, 32, sizeof(char) * screen_width * screen_height);
    memset(zBuffer, 0, sizeof(float) * screen_width * screen_height);
    memset(colorBuffer, 0, sizeof(char*) * screen_width * screen_height);

    for (float cubeX = -cubeWidth; cubeX <= cubeWidth; cubeX += spacing) { 
        for (float cubeY = -cubeWidth; cubeY <= cubeWidth; cubeY += spacing) { 
            calculateSurface(info, cubeX, cubeY, -cubeWidth, '#', "\x1b[31m");
            calculateSurface(info, cubeWidth, cubeY, cubeX, '-', "\x1b[32m");
            calculateSurface(info, -cubeWidth, cubeY, -cubeX, '!', "\x1b[33m");
            calculateSurface(info, -cubeX, cubeY, cubeWidth, '~', "\x1b[34m");
            calculateSurface(info, cubeX, -cubeWidth, -cubeY, '.', "\x1b[37m");
            calculateSurface(info, cubeX, cubeWidth, cubeY, ';', "\x1b[93m");
        } 
    }

    printf("\x1b[H");
    for (int k = 0; k < screen_width*screen_height + 1; k++) {
        if (k % screen_width) {
            if (colorBuffer[k])
                printf("%s", colorBuffer[k]); 
            putchar(output[k]);
        }
        else {
            printf("\x1b[0m");
            putchar(10);
        }
    }
}

int main() {
    float A = 0, B = 0, C = 0;

    printf("\x1b[2J");

    for (;;) {
        render_frame(A, B, C);
        A += 0.04;
        B += 0.01;
        C += 0.02;
        usleep(1000);
    }
    return 0;
}
