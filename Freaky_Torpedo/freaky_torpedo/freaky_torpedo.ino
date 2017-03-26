#include "Adafruit_VL53L0X.h"
#include "Servo.h" 

Adafruit_VL53L0X lox = Adafruit_VL53L0X();


Servo azimuthServo;
Servo elevationServo;

int azimuth = 0;
int azimuthIncrement = 1;
int azimuthDirection = 1;

int elevation = 45;
int elevationIncrement = 15;
int elevationDirection = 1;

String commandString = "";         // a string to hold incoming commands

bool outputToStream = false;

void printHelp()
{
  Serial.println("###############################");
  Serial.println("# '?' -> Help");
  Serial.println("# 'b' -> (b)egin streaming");
  Serial.println("# 'e' -> (e)nd streaming");
  Serial.println("###############################");
}

void setup() 
{
  Serial.begin(115200);
  
  // wait until serial port opens for native USB devices
  while (! Serial) 
  {
    delay(1);
  }
  Serial.println("###############################");
  Serial.println("# Freaky Torpedo");
  Serial.println("# Version 0.1");
  Serial.println("###############################");
  
  if (!lox.begin()) 
  {
    Serial.println(F("# Failed to boot VL53L0X"));
    while(1);
  }
  
  
  Serial.println(F("# send '?' for help\n")); 

  // reserve bytes for the commandString:
  commandString.reserve(32);
  
  azimuthServo.attach(9); 
  elevationServo.attach(10); 


  azimuthServo.write(azimuth);
  elevationServo.write(elevation);
  delay(1000);
}


void loop() 
{
  VL53L0X_RangingMeasurementData_t measure;
 
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

    
    float ambientRate = ((float)measure.AmbientRateRtnMegaCps)/65536.0f;
  

  if(outputToStream)
  {
    Serial.print("R\t"); Serial.print(elevation); Serial.print("\t"); Serial.print(azimuth); Serial.print("\t"); Serial.print(ambientRate); Serial.print("\t");

    if (measure.RangeStatus != 4) 
    {  
      Serial.println(measure.RangeMilliMeter);
    } 
    else
    {
      // Phase failures have incorrect data - so leave them blank - (Out Of Range)
      Serial.println();
    }
  
    // adjust the azimuth
    bool adjustElevation = false; 
    int delayForServos = 5;
  
    
    azimuth += azimuthIncrement *  azimuthDirection;
    if(azimuth > 180)
    {
      azimuth = 180;
      azimuthDirection = -1;
      adjustElevation = true;
    }
    else if(azimuth < 0)
    {
      azimuth = 0;
      azimuthDirection = 1;
      adjustElevation = true;
    }
    
    azimuthServo.write(azimuth);
  
    if(adjustElevation)
    {
      elevation += elevationIncrement *  elevationDirection;
      if(elevation > 90)
      {
        elevation = 45;
        elevationDirection = 1;
        // Longer delay
        delayForServos = 100;
      }
  
      elevationServo.write(elevation);
    }
    
    delay(delayForServos);

  }
}


void processCommand()
{
  if( commandString == "?")
  {
    printHelp();
  }
  else if( commandString == "b")
  {
    outputToStream = true;
  }
  else if( commandString == "e")
  {
    outputToStream = false;
  }
  
}


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\r') 
    {
      // Ignore
    }
    else if (inChar == '\n') 
    {
      processCommand();
      commandString = "";
    }
    else
    {
      // add it to the inputString:
      commandString += inChar;
    }
  }
}
