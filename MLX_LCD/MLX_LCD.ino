#include <Wire.h> // I2C library, required for MLX90614
#include <SparkFunMLX90614.h> // SparkFunMLX90614 Arduino library
IRTherm therm; // Create an IRTherm object to interact with throughout


void setup() 
{
  Serial.begin(9600); // Initialize Serial to log output
  Serial3.begin(9600);
  therm.begin(); // Initialize thermal IR sensor
  therm.setUnit(TEMP_F); // Set the library's units to Farenheit
  // Alternatively, TEMP_F can be replaced with TEMP_C for Celsius or
  // TEMP_K for Kelvin.
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
}

void loop() 
{
  // Call therm.read() to read object and ambient temperatures from the sensor.
  if (therm.read()) // On success, read() will return 1, on fail 0.
  {
  // Use the object() and ambient() functions to grab the object and ambient
  // temperatures.
  // They'll be floats, calculated out to the unit you set with setUnit().
    Serial.print("Object: " + String(therm.object(), 2));
    Serial.println("F");
    Serial.print("Ambient: " + String(therm.ambient(), 2));
    Serial.println("F");
    Serial3.println("                              ");
    Serial3.println(String(therm.object())+"F: "+(therm.object() < 63 ? "COLD" : "NOT COLD"));
  }

  if(therm.object() < 63){
    digitalWrite(2,HIGH);
  }
  delay(500);
  digitalWrite(2,LOW);
}

