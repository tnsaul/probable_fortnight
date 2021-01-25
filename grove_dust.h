/* =============================================== */
/*
 Change History:
 v1 from  v8 of https://github.com/tnsaul/Grove-Dust-Sensor
*/
/* =============================================== */


/*
 Some or much of this code is from Grove - Dust Sensor Demo v1.0
 Interface to Shinyei Model PPD42NS Particle Sensor
 Program by Christopher Nafis
 Written April 2012
 http://www.seeedstudio.com/depot/grove-dust-sensor-p-1050.html
 http://www.sca-shinyei.com/pdf/PPD42NS.pdf
 JST Pin 1 (Black Wire)  =&gt; //Arduino GND
 JST Pin 3 (Red wire)    =&gt; //Arduino 5VDC
 JST Pin 4 (Yellow wire) =&gt; //WeMOS D3 = GPIO0
*/



#include "esphome.h"

static const char *TAG = "custom.GroveDustCustomSensor";

class GroveDustCustomSensor : public PollingComponent, public Sensor {
    public:
        // Constructor
        GroveDustCustomSensor() : PollingComponent(15000) {
            // /* ==== Grove Class Variables ==== */
            // int pin = D5;                              // Need to have a pin WITHOUT a pull-up or pull-down resistor on the input.
            // unsigned long duration;
            // unsigned long starttime;
            // unsigned long sampletime_ms = 30*1000;     //sample 30s??
            // unsigned long lowpulseoccupancy = 0;
            // float ratio = 0;
            // float concentration = 0;

        }

        float get_setup_priority() const override { return esphome::setup_priority::IO; }


        void setup() override {
            // This will be called once to set up the component
            // think of it as the setup() call in Arduino

            // Grove just uses a standard IO pin
            pinMode(pin, INPUT);
        }
        void update() override {
            // This will be called every polling period.
            /*
            * The Grove sensor runs in a tight loop, so it will need to consider impacts if there is any other looping code.
            *
            *
            * Reads a pulse (either HIGH or LOW) on a pin. For example, if value is HIGH, pulseIn() 
            * waits for the pin to go HIGH, starts timing, then waits for the pin to go LOW and 
            * stops timing. Returns the length of the pulse in microseconds or 0 if no complete 
            * pulse was received within the timeout.
            */
            lowpulseoccupancy = 0;  
            starttime = millis();                                               //get the current time; 
            // Loop for sampltime_ms duration reading the pulse measurements and accumulating them
            while ((millis()-starttime) < sampletime_ms){
                duration = pulseIn(pin, LOW);                                     // Timeout is 1 second as default when not specified.
                lowpulseoccupancy = lowpulseoccupancy+duration;                   // Duration is in microseconds
            }


            /*
            * This could do with some tweaking as the sample period could extend if pulseIn() forced the 
            * sample time to extend beyond 30 secs.  Perhaps setting a timeout is appropriate?
            */
            ratio = lowpulseoccupancy/(sampletime_ms*10.0);                     // Integer percentage 0 < 100
            concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;   // using spec sheet curve
            ESP_LOGD(TAG, "Lowpulseoccupancy = : %d microseconds", lowpulseoccupancy);
            ESP_LOGD(TAG, "Concentration = : %d pcs/0.01cf", concentration);
        }

    private:
            /* ==== Grove Class Variables ==== */
            int pin = D5;                              // Need to have a pin WITHOUT a pull-up or pull-down resistor on the input.
            unsigned long duration;
            unsigned long starttime;
            unsigned long sampletime_ms = 30*1000;     //sample 30s??
            unsigned long lowpulseoccupancy = 0;
            float ratio = 0;
            float concentration = 0;
};