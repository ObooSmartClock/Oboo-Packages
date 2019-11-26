#include <Adafruit_NeoPixel.h>

#define LIGHTBAR_PIN                A3
#define LIGHTBAR_NUM_LEDS           4
#define LIGHTBAR_NUM_COMPONENTS     3

// TODO: fix this so that the fade works nicely
#define LIGHTBAR_MAX_NUM_STEPS      16   // number of steps between start and target color
// #define LIGHTBAR_MAX_NUM_STEPS      5
#define LIGHTBAR_DEFAULT_BRIGHTNESS 16    // out of 255

// lightbar led values
// target value
uint32_t lightBarTarget[LIGHTBAR_NUM_LEDS];
// starting value
uint32_t lightBarStart[LIGHTBAR_NUM_LEDS];
// difference between starting and target color (for each led, each color component)
float colorStep[LIGHTBAR_NUM_LEDS][LIGHTBAR_NUM_COMPONENTS];

// counter for progress to target color
int lightBarCounter;
// total number of incremental steps between starting and target color
int lightBarNumSteps;

// lightbar control
Adafruit_NeoPixel lightBar = Adafruit_NeoPixel(LIGHTBAR_NUM_LEDS, LIGHTBAR_PIN, NEO_GRB + NEO_KHZ800);

void lightBarInit () {
    lightBarCounter = LIGHTBAR_MAX_NUM_STEPS;
    lightBarNumSteps = LIGHTBAR_MAX_NUM_STEPS;

    lightBar.begin();
    lightBar.setBrightness(LIGHTBAR_DEFAULT_BRIGHTNESS);
    lightBar.show();
}

// helper function to return a byte component of a color
//  ex: input = 0x123456
//  compNumber      return value
//  0               0x56
//  1               0x34
//  2               0x12
uint32_t isolateComponent(uint32_t input, int compNumber) {
    return (uint32_t)((input >> (compNumber*8))&0xff);
}

// helper function to return intermediate progress color
//  note: difference between start and target color is computed every iteration
//      (instead of just at the beginning - this way the LEDs are completely independent of each other)
uint32_t calculateColorProgress(uint32_t startColor, uint32_t targetColor, int stepNumber, int numSteps) {
    float stepSize;
    int32_t output = startColor;

    for (int i=0; i<LIGHTBAR_NUM_COMPONENTS; i++){
        stepSize = (float)((float)isolateComponent(targetColor, i) - (float)isolateComponent(startColor, i)) / (float)numSteps ;
        output += (int32_t)((int32_t)(stepSize*(float)stepNumber))<<(i*8);
    }
    return (uint32_t)output;
}

// helper function that does all the work setting an led to change to a specific color
void setLedTargetColor (int index, uint32_t color) {
    // set the starting point based on the current status of the light
    lightBarStart[index] = calculateColorProgress(lightBarStart[index], lightBarTarget[index], lightBarCounter+1, lightBarNumSteps);

    // set the new target
    lightBarTarget[index] = color;
}

void lightBarSetColor (uint32_t color) {
    for (int i=0; i<LIGHTBAR_NUM_LEDS; i++){
        setLedTargetColor(i, color);
    }
    // reset the counter to start the fade
    lightBarCounter = 0;
}


void lightBarSetButton (int buttonIndex, uint32_t color) {
    if (buttonIndex >= 0 && buttonIndex < LIGHTBAR_NUM_LEDS) {
        setLedTargetColor(buttonIndex, color);

        // reset the counter to start the fade
        lightBarCounter = 0;
    }
}

void lightBarFade () {
    if (lightBarCounter < lightBarNumSteps) {
        for (int i=0; i<LIGHTBAR_NUM_LEDS; i++){
            lightBar.setPixelColor(i, calculateColorProgress(lightBarStart[i], lightBarTarget[i], lightBarCounter+1, lightBarNumSteps));

            // if (i==0){
                // Serial.print("For step ");
                // Serial.print(lightBarCounter);
                // Serial.print(" color is ");
                // Serial.println(calculateColorProgress(lightBarStart[i], colorStep[i], lightBarCounter), HEX);
            // }
        }
        lightBar.show();
        lightBarCounter++;

        // when the target color is reached, update the start value to be the same as the target
        //  this is required so that leds can be independent of each other
        if (lightBarCounter >= lightBarNumSteps) {
            // Serial.println("Reached target - Setting start colors to target value");
            for (int i=0; i<LIGHTBAR_NUM_LEDS; i++){
                lightBarStart[i] = lightBarTarget[i];
            }
        }
    }
}
