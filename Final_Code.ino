/* 
*  Arduino Code for 11059 Project Plant Project
*  By Jehru Harris
*  
*  If changing the plant or soil then you must make sure to calibrate the new values
*  Change the dryValue, wetValue, interval, and lux measurements in the LOOP code 
*/

// ---------
// LIBRARIES 
// ---------
#include <FastLED.h>

#include <Wire.h>
#include <BH1750.h>
    
//  -------------------
// DECLARATION VARIABLES
// --------------------
// To Calibrate adjust these dry and wet values to whatever they are measured to be, this will make the friendly value an appropriate percetage
// Moisture sensor values 
  int dryValue = 919;
  int wetValue = 695;
  int DryValuePercentage = 0;
  int WetValuePercentage = 100;

// LEDS
  #define NUM_LEDS 14
  #define DATA_PIN 4
  #define BRIGHTNESS 64

  CRGB leds[NUM_LEDS];

// Ultrasonic
  const int pingPin = 9; // Trigger Pin of Ultrasonic Sensor
  const int echoPin = 8; // Echo Pin of Ultrasonic Sensor

// Light Sensor
  BH1750 lightMeter;
  
  // will store last time LED was updated
  long previousMillis = 0; 
  // the follow variables is a long because the time, measured in miliseconds,
  // will quickly become a bigger number than can be stored in an int.
  long interval = 15000;           // interval time, 15 seconds in milliseconds
  
  // Over 1 minute
  // 60000
  // Over 1 hour
  // 3600000
  // Over 1 day 
  // 86400000
  // Over 3 days 
  // 259200000


// -----------------
// SETUP
// -----------------
  void setup() {

    // sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(2000);

    // Serial Speed    
    Serial.begin(115200);
    
    // LED startup    
    FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS );

    // Lightmeter startup
    Wire.begin();
    lightMeter.begin();
    Serial.println(F("Lightmeter Starting"));

}
  
// ----------------
// LOOP
// ----------------
  void loop() {
    
// Moisture Sensor Readout
    // Prints out a raw value which is the direct reading of moisture sensor
    int rawValue = analogRead(A0); 

    // Prints out the raw value
    Serial.print("Raw: ");
    Serial.print(rawValue);
    Serial.print(" | ");

    // Maps out the values
    int ValuePercentage = map(rawValue, dryValue, wetValue, DryValuePercentage, WetValuePercentage);
  
    // Prints out the percentage value on the serial monitor
    Serial.print("Percentage: ");
    Serial.print(ValuePercentage);
    Serial.println("%");

    // Light Sensor Readout
    // reads the input on analog pin A0 (value between 0 and 1023)
//    uint16_t lux = lightMeter.readLightLevel();
    float lux = lightMeter.readLightLevel(true);
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
    delay(500);


// Ultrasonic Readout
    long duration, cm;
    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);
    pinMode(echoPin, INPUT);
   
    duration = pulseIn(echoPin, HIGH);
   
    cm = microsecondsToCentimeters(duration);
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();
    delay(500);
  

// IF STATMENTS
// This is measured over the interval time 
  unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
        // save the last time you blinked the LED 
        previousMillis = currentMillis;   
  
        // while the lux is more than 2500 then its too bright
        while(lux > 2500) {
           Serial.println("Help, I need less light its too bright");
           
           // Make the Leds purple for less light          
           fill_solid( leds, NUM_LEDS, CRGB::Purple);
           FastLED.show();
           
           // Add some delay           
           delay(500);
           
           // Take a new reading from the light sensor           
           uint16_t lux = lightMeter.readLightLevel();
           Serial.println(lux);
           
            // If the light is then less than 2501 then break the while loop 
            if(lux < 2501) {
              Serial.println("Break");
              break;
            }
        }
        
        // While the lux is less than 500 then its too dark
        while(lux < 500) {
            Serial.println("Help, I need more light its too dark");

            // Make the led yellow for more light            
            fill_solid( leds, NUM_LEDS, CRGB::Yellow);
            FastLED.show();
            
            // Add some delay            
            delay(500);

            // Take a new reading from the light sensor             
            uint16_t lux = lightMeter.readLightLevel();
            Serial.println(lux);
            
              // If the light is more than 500 then this will break the while loop             
              if(lux > 499) {
                Serial.println("Break");
                break;
              }
          }
        
      // The light is perfectly between 500 and 2500 lux. 
      // No Leds are turned on as its in the perfect state      
      Serial.println("The light is perfect");
      fill_solid( leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      
    }
    
    // If there is an object in front of the plant then the plant gets excited
    // Less than 60cm
    if (cm < 60) {
      
       // For loop creates the round and round effect. One led is loaded in consequative order  
       for(int orangeLed = 0; orangeLed < 14; orangeLed = orangeLed + 1) {
         
           // Turn our current led on to orange, then show the leds
           leds[orangeLed] = CRGB::Orange;
           // Show the leds (only one of which is set to orange, from above)
           FastLED.show();
           delay(100);
           leds[orangeLed] = CRGB::Black; // Turns back to black for the next loop around 
       }
     }
     // If human interaction is not detected     
     else {
          // The leds start off. This is their blank state          
//          fill_solid( leds, NUM_LEDS, CRGB::Black);
//          FastLED.show();
    
          // The plant is thirsty, so the Led turns red (ANGRY)
          // This is above 75% moisture          
          if(ValuePercentage > 75){
            fill_solid( leds, NUM_LEDS, CRGB::Red);
            FastLED.show();
          }
          // The plant is overwatered, so the Led turns blue (FULL)
          // This is below 15% moisture
          else if(ValuePercentage < 15){
            fill_solid( leds, NUM_LEDS, CRGB::Blue);
            FastLED.show();
          }
          // The plant is neither thirsty, nor overwatered
          // This is the default state          
          else {
            fill_solid( leds, NUM_LEDS, CRGB::Black);
            FastLED.show();
          }
     }
       
 }

// Converts micronseconds to centimeters
// Required for the ultrasonic 
long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}
