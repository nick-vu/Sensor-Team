#include <Wire.h>
#include <math.h>
#include <Adafruit_TCS34725.h>

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

// flags which indicate if we're over color strips
uint8_t red = 0;
uint8_t blue = 0;
uint8_t green = 0;
uint8_t yellow = 0;

//running average
int len = 3;
char status[3];

void setup(void) {
    Serial.begin(9600);

    if (tcs.begin()) {
        Serial.println("Found sensor");
    } else {
        Serial.println("No TCS34725 found ... check your connections");
    while (1);
    }
}

// check if we've been in the color for len cycles
void updateStatus(char color) {
    for (int i = (len - 1); i > 0; i--) {
        status[i] = status[i - 1];
    }

    status[0] = color;
}

uint16_t isInColor(char target) {
    for (int i = 0; i < len; i++) {
        if (status[i] != target) {
            return 0;
        }
    }
    return 1;
}

void RGBtoHSL(uint16_t r, uint16_t g, uint16_t b, double *h, double *s, double *s2, double *l, double *v) {
    double colors[3] = {(double) r / 255, (double) g / 255, (double) b / 255};

    double max = colors[0];
    int max_color = 0;
    double min = colors[0];
    int min_color = 0;

    for (int i = 1; i < 3; i++) {
        if (colors[i] > max) {
            max = colors[i];
            max_color = i;
        } else if (colors[i] < min) {
            min = colors[i];
            min_color = i;
        }
    }

    double hue = 0;

    if (max != min) {
        switch (max_color) {
            case 0:
                hue = 60 * ((colors[1] - colors[2])/(max - min));
                break;
            case 1: // working
                hue = 60 * (2 + (colors[2] - colors[0])/(max - min));
                break;
            case 2: // working
                hue = 60 * (4 + (colors[0] - colors[1])/(max - min));
                break;
        }

        if (hue < 0) {
            hue += 360;
        }
    }

    *h = hue;

    if (max == 0 || min == 1) {
        *s = 0;
    } else {
        *s = (max - min)/(1 - fabs(max + min - 1));
    }

    if (max == 0) {
        *s2 = 0;
    } else {
        *s2 = (max - min)/max;
    }

    *l = (max + min)/2;

    *v = max;
}


// determine which color strip we are over (if at all)
void getColorState(uint16_t r, uint16_t g, uint16_t b) {
    double h, s, s2, l, v;

    RGBtoHSL(r, g, b, &h, &s, &s2, &l, &v);

    // for data logging purposes
    Serial.print(h); Serial.print(" ");
    Serial.print(s); Serial.print(" ");
    Serial.println(l);

    // check if we're over a color
    if (s > 0.18) {
        if (h <= 11 || h >= 355) { // red
            if (isInColor('r')) {
                //Serial.println("Got RED!");
            } else {
                updateStatus('r');
            }
        } else if (h >= 52 && h <= 117) { // green
            if (isInColor('g')) {
                //Serial.println("Got GREEN!");
            } else {
                updateStatus('g');
            }
        } else if (h >= 190 && h <= 200) { // blue
            if (isInColor('b')) {
                //Serial.println("Got BLUE!");
            } else {
                updateStatus('b');
            }
        } else if (h >= 30 && h <= 44) { // yellow
            if (isInColor('y')) {
                //Serial.println("Got YELLOW!");
            } else {
                updateStatus('y');
            }
        }
    } else {
        // not in a color so start filling up the buffer with fails
        updateStatus('x');  
    }
}

void getRGB(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c) {

    tcs.getRawData(r, g, b, c);

    uint32_t sum = *c;

    // Avoid divide by zero errors ... if clear = 0 return black
    if (*c == 0) {
        *r = *g = *b = 0;
        return;
    }

    *r = (float)*r / sum * 255.0;
    *g = (float)*g / sum * 255.0;
    *b = (float)*b / sum * 255.0;
}

void loop(void) {
    uint16_t r, g, b, c;

    getRGB(&r, &g, &b, &c);

    getColorState(r, g, b);

    int currentMillis = millis();
    
    while (millis() - currentMillis < 98) {
      // spin for 98 milliseconds
    }

  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  //colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  //lux = tcs.calculateLux(r, g, b);
  //Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  //Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
}