/* =============================================== */
/*
 Change History:
 v1 20210126 from  v8 of https://github.com/tnsaul/Grove-Dust-Sensor
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
// We declare the sampletime_ms as static so it can be used in the constructor
// else we end up in a tight loop
static unsigned long sampletime_ms = 120000;       //sample 120s?? 


// Interrupt handler code
volatile byte interruptCounter = 0;
ICACHE_RAM_ATTR void dust_trigger_isr(void){
    interruptCounter++;
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

            // Try some interrupts
            attachInterrupt(pin, dust_trigger_isr, FALLING);
        }

        void loop() override {
            // This will be called by App.loop() as part of the Constructor class override
            /*
            * Reads a pulse (either HIGH or LOW) on a pin. For example, if value is HIGH, pulseIn() 
            * waits for the pin to go HIGH, starts timing, then waits for the pin to go LOW and 
            * stops timing. Returns the length of the pulse in microseconds or 0 if no complete 
            * pulse was received within the timeout.
            */
            // Parameters::
            // pin: the number of the Arduino pin on which you want to read the pulse. Allowed data types: int.
            // value: type of pulse to read: either HIGH or LOW. Allowed data types: int.
            // timeout (optional): the number of microseconds to wait for the pulse to start; default is one second. Allowed data types: unsigned long.

            duration = pulseIn(pin, LOW, 500000);                             
            
            lowpulseoccupancy = lowpulseoccupancy+duration; 
         }

        void update() override {
            // We aren't using interrupts, but it is handy to count the number of pulses we see.
            ESP_LOGD(TAG, "Interrupts %d", interruptCounter);
            interruptCounter = 0;

            // This will be called every polling period.

            // We should be dividing by something to do with elapsed time and not sampletime_ms if we are looking
            // to be accurate.
            ratio = lowpulseoccupancy/(sampletime_ms*10.0);                     // Integer percentage 0 < 100
            concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;   // using spec sheet curve
            ESP_LOGD(TAG, "Lowpulseoccupancy = : %ld milliseconds", lowpulseoccupancy);
            ESP_LOGD(TAG, "Concentration = : %f pcs/0.01cf", concentration);   
            publish_state(concentration);  

            // Reset the key variables to key off a new sample!
            lowpulseoccupancy = 0;  
        }

    private:
            /* ==== Grove Class Variables ==== */
            int pin = D5;                               // Need to have a pin WITHOUT a pull-up or pull-down resistor on the input.
            unsigned long duration;
            unsigned long lowpulseoccupancy = 0;
            float ratio = 0;
            float concentration = 0;
            volatile boolean interruptFlag = 0;
            int numberOfInterrupts = 0;
};