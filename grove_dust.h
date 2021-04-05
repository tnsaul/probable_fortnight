/* =============================================== */
/*
 Change History:
 v1 20210126 from  v8 of https://github.com/tnsaul/Grove-Dust-Sensor
 v2 20210405 Changed from pulseIn() method to using interrupts on the pin.
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

// STATIC DECLARES
static const char *TAG = "custom.GroveDustCustomSensor";
// We declare the sampletime_ms as static so it can be used in the constructor
// else we end up in a tight loop
static unsigned long sampletime_ms = 120000;       //sample 120s?? 

// GLOBAL VARIABLES
int pin = D5;     // = GPIO14 Need to have a pin WITHOUT a pull-up or pull-down resistor on the input.


// Interrupt handler code
volatile byte interruptCounter = 0;
volatile unsigned long interruptStart;
volatile unsigned long interruptOccupancy=0;
volatile bool gotPulseStart = false;
ICACHE_RAM_ATTR void dust_trigger_isr_change(void){
    // Count the interrupts
    interruptCounter++;
    if (digitalRead(pin) == LOW)
    {
        // Start of a low pulse hopefully
        interruptStart = micros();
        gotPulseStart = true;
    }else if (gotPulseStart && digitalRead(pin) == HIGH)
    {
        // This should be th elow to high transition at the end of a pulse
        gotPulseStart =  false;
        interruptOccupancy += micros() - interruptStart;
    }
}


class GroveDustCustomSensor : public PollingComponent, public Sensor {
    public:

        // Constructor - update_interval	The update interval in ms.
        GroveDustCustomSensor() : PollingComponent(sampletime_ms) {
            ESP_LOGD(TAG, "Instantiated.");
        }

        float get_setup_priority() const override { return esphome::setup_priority::LATE; }


        void setup() override {
            // This will be called once to set up the component
            // think of it as the setup() call in Arduino

            // Grove just uses a standard IO pin
            pinMode(pin, INPUT);

            // Attach an interrupt
            attachInterrupt(digitalPinToInterrupt(pin), dust_trigger_isr_change, CHANGE);
        }

        void loop() override {
            // This will be called by App.loop() as part of the Constructor class override
        }

        void update() override {
            // This will be called every polling period.            
            
            // Log the number of pulses - should correlate with the number of interrupts
            ESP_LOGD(TAG, "Interrupt Count %d", interruptCounter);
            
            // We should be dividing by something to do with actual elapsed time and not sampletime_ms if we are looking
            // to be accurate given update() is not precise
            ratio = interruptOccupancy/(sampletime_ms*10.0);                     // Integer percentage 0 < 100
            concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;   // using spec sheet curve
            ESP_LOGD(TAG, "Interrupt Occupancy = : %lu microseconds", interruptOccupancy);
            ESP_LOGD(TAG, "Ratio = : %.2f %%", ratio);
            ESP_LOGD(TAG, "Concentration = : %.2f pcs/0.01cf", concentration);   
            publish_state(concentration);  

            // Reset the key variables to key off a new sample!
            interruptCounter = 0;
            interruptOccupancy = 0;            
        }

    private:
            /* ==== Grove Class Variables ==== */
            float ratio = 0;
            float concentration = 0;
 };