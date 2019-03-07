#include <Wire.h>
#include <math.h>
#include <Adafruit_TCS34725.h>

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);


void update_averages(uint16_t r, uint16_t g, uint16_t b);
uint16_t mean(uint16_t *data);

// flags which indicate if we're over color strips
uint8_t red = 0;
uint8_t blue = 0;
uint8_t green = 0;
uint8_t yellow = 0;

// colors
uint16_t red_target[3] = {142, 69, 56};
uint16_t green_target[3] = {89, 113, 58};
uint16_t blue_target[3] = {74, 94, 94};
uint16_t yellow_target[3] = {120, 90, 48};
uint16_t white_target[3] = {91, 90, 76};

//running average
uint16_t len = 30;
uint16_t red_avg_data[30];
uint16_t green_avg_data[30];
uint16_t blue_avg_data[30];

double red_avg;
double green_avg;
double blue_avg;

void setup(void) {
    Serial.begin(9600);

    if (tcs.begin()) {
        Serial.println("Found sensor");
    } else {
        Serial.println("No TCS34725 found ... check your connections");
    while (1);
    }

    //initialize average colors
    uint16_t r, g, b, c;
    getRGB(&r, &g, &b, &c);

    for (int i = 0; i < len; i++) {
        red_avg_data[i] = r;
        green_avg_data[i] = g;
        blue_avg_data[i] = b;
    }

    red_avg = mean(red_avg_data);
    green_avg = mean(green_avg_data);
    blue_avg = mean(blue_avg_data);
}

void update_averages(uint16_t r, uint16_t g, uint16_t b) {
    for (int i = (len - 1); i > 0; i--) {
        red_avg_data[i] = red_avg_data[i - 1];
        green_avg_data[i] = green_avg_data[i - 1];
        blue_avg_data[i] = blue_avg_data[i - 1];
    }

    red_avg_data[0] = r;
    green_avg_data[0] = g;
    blue_avg_data[0] = b;

    red_avg = mean(red_avg_data);
    green_avg = mean(green_avg_data);
    blue_avg = mean(blue_avg_data);
}

// try squareing each value then sqrt the whole thing
uint16_t mean(uint16_t *data) {
    uint16_t sum = 0;
    for (int i = 0; i < len; i++) {
        sum += data[i];
    }

    uint16_t avg = sum / len;
    return avg;
}

uint16_t min_distance(double *distances) {
    double min = distances[0];
    uint16_t min_index = 0;
    for (int i = 1; i < 5; i++) {
        if (distances[i] < min) {
            min = distances[i];
            min_index = i;
        }
    }

    return min_index;
}

void RGBtoHSL(uint16_t r, uint16_t g, uint16_t b, double &h, double &s, double &l) {
    double colors[3] = {r / 255, g / 255, b / 255};

    double max = colors[0];
    uint16_t max_color = 0;
    double min = colors[0];
    uint16_t min_color = 0;
    
    for (int i = 1; i < 3; i++) {
        if (colors[i] > max) {
            max = colors[i];
            max_color = i;
        } else if (colors[i] < min) {
            min = colors[i];
            max_color = i;
        }
    }

    double hue = 0;

    if (max != min) {
        switch (max_color) {
            case 0:
                hue = 60 * (colors[1] - colors[2])/(max - min);
                break;
            case 1:
                hue = 60 * (2 + (colors[2] - colors[0])/(max - min));
                break;
            case 2:
                hue = 60 * (4 + (colors[0] - colors[1])/(max - min));
                break;
        }
    } else {
        hue = 0;
    }

    if (hue < 0) {
        hue += 360;
    }

    *h = hue;

    if (max == 0 || min == 1) {
        *s = 0;
    } else {
        *s = (max - min)/(1 - fabs(max + min - 1));
    }

    *l = (max + min)/2;
}


// determine which color strip we are over (if at all)
void get_color_state(uint16_t r, uint16_t g, uint16_t b) {

    double distance_threshold = 100;
    double distance = sqrt(pow((r - red_avg), 2) + pow((g - green_avg), 2) + pow((b - blue_avg), 2));
    Serial.println(distance);    
    // check if we're on a color strip
    if (distance >= distance_threshold) {
        // figure out which color that is
        double red_distance = sqrt(pow((r - red_target[0]), 2) + pow((g - red_target[1]), 2) + pow((b - red_target[2]), 2));
        double green_distance = sqrt(pow((r - green_target[0]), 2) + pow((g - green_target[1]), 2) + pow((b - green_target[2]), 2));
        double blue_distance = sqrt(pow((r - blue_target[0]), 2) + pow((g - blue_target[1]), 2) + pow((b - blue_target[2]), 2));
        double yellow_distance = sqrt(pow((r - yellow_target[0]), 2) + pow((g - yellow_target[1]), 2) + pow((b - yellow_target[2]), 2));
        double white_distance = sqrt(pow((r - white_target[0]), 2) + pow((g - white_target[1]), 2) + pow((b - white_target[2]), 2));

        // get minimum distance
        double distances[5] = {red_distance, green_distance, blue_distance, yellow_distance, white_distance};
        uint16_t min_index = min_distance(distances);

        // are we close enough to that color? does it matter?
        switch (min_index) {
            case 0:
                Serial.println("RED!");
                break;
            case 1:
                Serial.println("GREEN!");
                break;
            case 2:
                Serial.println("BLUE!");
                break;
            case 3:
                Serial.println("YELLOW!");
                break;
            case 4:
                Serial.println("WHITE!");
                break;
        }

    } else {
        // update the averages if we aren't on a color strip
        update_averages(r, g, b);
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

  get_color_state(r, g, b);

  double h, s, l;

  RGBtoHSL(r, g, b, &h, &s, &l);

  Serial.print("H: "); Serial.print(h, DEC); Serial.print(" ");
  Serial.print("S: "); Serial.print(s, DEC); Serial.print(" ");
  Serial.print("L: "); Serial.print(l, DEC); Serial.print(" ");
  Serial.println(" ");

  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  //colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  //lux = tcs.calculateLux(r, g, b);
  //Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  //Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
}