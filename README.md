# probable_fortnight
Grove Dust Sensor integrated into Home Assistant (ESP Home)

ESP Home ProtoType - BME280, D1Mini, Grove Dust Sensor

## Approach
This is using ESP Home (https://esphome.io/index.html) as the foundation coding and integration to Home Assistant (https://www.home-assistant.io/).

What is in this prototype:
1. BME280 using https://esphome.io/components/sensor/bmp280.html
2. 16x2 Line I2C LCD Display using https://esphome.io/components/display/lcd_display.html
3. A Grove Dust Sensor.  The Grove Dust Sensor is a nifty little device to see how dirty my workshop air is (I do woodworking hence it is pretty bad). Reference:  http://wiki.seeedstudio.com/Grove-Dust_Sensor/

### Temperature
The code attempts to calculate the "apparent" temperature from https://planetcalc.com/2089/
However it is currently not giving a reasonable value on overcast and raining days - likely dufus (me) error in the calculation or understanding of how it should be done.

### Dust Sensor 
This is written as a non-blocking custom "Component" model in ESP Home.  The original Grove software was blocking - sampling the P1 pulse over a 30 second interval.  This non-blocking version reads the P1 input level in the loop() override which is supposed to be run up to 60 times per second.  Update() is from the PollingComponent class and is called every 2 mins (settable in the comstructor for the class).

## Compiling etc.
You will need to install ESP Home and Home Assistant if you want to use this.

ESP Home is best initially set up using the wizard as it will set up directories and code structures.  Once you get the hang of it, it is much like any other Arduino ESP8266 coding practice.

I used VSCode as the IDE and linked this back to my GITHUB environment.

ESP Home needs to be compiled in the CLI in a terminal (well for me at least) using:

`esphome probable_fornight.yaml compile`

To deploy it to a connects D1 Mini use:

`esphome probable_fornight.yaml run`