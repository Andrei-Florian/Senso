/*
 * Senso
 * 20/AUG/2019 - 4/OCT/2019 | Andrei Florian
 */

#define ARDUINO_MKR

#include <TinyGPS++.h>
#include <SigFox.h>
#include <ArduinoLowPower.h>
#include <Universum_Logo.h>
#include <AudioAnalyzer.h>

int baudGPS = 9600;
int freqVal = 0;
int nrSamples = 6; // the number of samples to take

bool proDebug = true; // debugging? True requires permanent connection with serial monitor

struct Data // data to send
{
  float geoLat;
  float geoLng;
  float battery;
};

typedef union // conversion to bytes for send
{
  Data data;
  byte bytes[12];
} DataUnion;

DataUnion dataUnion;
Analyzer Audio = Analyzer(4,5,0); // S to 4, R to 5, Analog to A0
TinyGPSPlus gps;

void reboot() 
{
  Serial.println("  Restarting Device");
  NVIC_SystemReset();
  while(1);
}

bool getGPS()
{
  while(Serial1.available() > 0)
  {
    if(gps.encode(Serial1.read()))
    {
      if(gps.location.isValid() && gps.time.isValid() && gps.date.isValid() && (gps.location.lat() != 0) && (gps.location.lng() != 0))
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }

  if(millis() > 10000 && gps.charsProcessed() < 10)
  {
    Serial.println("Error - GPS Module Responded with Error");
    Serial.println("  Terminating Code");
    Serial.println("________________________________________");
    while(1) {};
  }
}

// only gets voltage id powered through terminals
void getBatteryVoltage()
{
  analogReadResolution(10);
  analogReference(AR_INTERNAL1V0);
  
  int sensorValue = analogRead(ADC_BATTERY);
  float batteryVoltage = sensorValue * (3.25 / 1023.0);
  dataUnion.data.battery = batteryVoltage;
}

void extractGPS()
{
  dataUnion.data.geoLat = gps.location.lat();
  dataUnion.data.geoLng = gps.location.lng();
}

bool processData()
{
  Serial.println("Processing Data");
  Serial.println("________________________________________");

  Serial.println("Getting Samples");
  Serial.println("  OK - Setting Up");
  bool anomaly[nrSamples]; // 6 samples will be taken of 100 reads

  Serial.println("  OK - Taking Samples");
  for(int y = 0; y < nrSamples; y++)
  {
    long valueMean[7] = {0, 0, 0, 0, 0, 0, 0}; // final data mean
    long avg[7] = {0, 0, 0, 0, 0, 0, 0}; // add data together to get mean
  
    for(int i = 0; i < 100; i++) // take 100 samples
    {
      int rawFreq[7]; // array for data
      long refinedFreq[7]; // array of refined data
  
      for(int a = 0; a < 7; a++)
      {
        rawFreq[a] = 0;
        refinedFreq[a] = 0;
      }
  
      Audio.ReadFreq(rawFreq); // get the frequencies
      
      // process the analogic data
      for(int a = 0; a < 7; a++)
      {
        refinedFreq[a] = max((rawFreq[a]-100),0);
      }
      
      // add the refined data to the variables
      for(int a = 0; a < 7; a++)
      {
        avg[a] += refinedFreq[a];
      }
  
      delay(20);
    }

    // get the mean of all the values
    for(int i = 0; i < 7; i++)
    {
      valueMean[i] = (avg[i] / 100);
    }

    long comparison = ((valueMean[0] + valueMean[1] + valueMean[2] + valueMean[3]) / 1.9);
    
    if(valueMean[5] > comparison)
    {
      anomaly[y] = true;
    }
    else
    {
      anomaly[y] = false;
    }
  }

  Serial.println("  OK - Checking Samples");
  // check if at least 2 of the samples signalled anomalies
  int anomalyNr = 0;
  for(int i = 0; i < nrSamples; i++)
  {
    if(anomaly[i] == true)
    {
      anomalyNr++;
    }
  }

  Serial.print("  OK - Samples detected with anomanies: "); Serial.println(anomalyNr);
  if(anomalyNr > 1)
  {
    Serial.println("  Success - Sending Warning Message");
    Serial.println("________________________________________");
    Serial.println("");
    return true;
  }
  else
  {
    Serial.println("  Success - All Values are Normal");
    Serial.println("________________________________________");
    Serial.println("");
    return false;
  }
}


bool sendToSigFox()
{
  Serial.println("  OK - Getting Values to Compile");

  Serial.println("  OK - Sending Data");
  
  Serial.print("  --> OUTPUT  ");
  for(int i = 0; i < 12; i++)
  {
    Serial.print(dataUnion.bytes[i], HEX);
  }
  
  Serial.println("");
  delay(100);
  
  SigFox.beginPacket();
  SigFox.write(dataUnion.bytes, HEX);

  Serial.println("  Success - Data is Sent");
  Serial.println("");
}

void encodeData(float sigLat, float sigLng, float sigBattery)
{
  Serial.println("  OK - Encoding Data");

  Serial.print("  --> INPUT   "); 
  Serial.print(sigLat, 6); Serial.print(", ");
  Serial.print(sigLng, 6); Serial.print(", ");
  Serial.println(sigBattery);
  
  Serial.print("  --> OUTPUT  ");
  for(int i = 0; i < 12; i++)
  {
    Serial.print(dataUnion.bytes[i], HEX);
  }
  Serial.println("");
}

void setup()
{
  Serial.begin(9600);

  if(proDebug)
  {
    while(!Serial) {};
  }
  
  logoStart("Senso");

  Serial.println("Initialising SigFox Module");
  if(!SigFox.begin())
  {
    Serial.println("  Error - SigFox Module Responded with Error");
    reboot();
  }
  Serial.println("  Success - Module Initialised");

  Serial.println("Starting SigFox");
  SigFox.begin();
  Serial.println("  Success - SigFox Module Online");
  
  Serial.println("Initialising Software Serial");
  Serial1.begin(baudGPS);
  Serial.println("  Success - Software Serial Running");

  Serial.println("Initialising Sound Analyser Module");
  Audio.Init();
  Serial.println("  Success Module is Online");
  
  Serial.println("Setup Complete");
  Serial.println("");
  Serial.println("");
}

void loop()
{
  delay(1000); // delay after sleep
  
  if(processData()) // if anomaly detected
  {
    Serial.println("Getting GPS");
    Serial.println("________________________________________");

    Serial.println("Getting Geolocation from GPS");
    Serial.print("  ");
    while(!gps.location.isValid())
    {
      getGPS();
      Serial.print(".");
      delay(500);
    }

    Serial.println("");
    Serial.println("  OK - Location Fixed, extracting");
    extractGPS(); // save GPS co-ordinates to global variables
    Serial.print("  OK - Got "); Serial.print(dataUnion.data.geoLat, 6); Serial.print(","); Serial.println(dataUnion.data.geoLng, 6);
    delay(2000); // delay for debug visualisation
    
    Serial.println("________________________________________");
    Serial.println("");

    Serial.println("Sending Data to SigFox");
    Serial.println("________________________________________");
    
    Serial.println("Parsing Data");
    getBatteryVoltage();
    encodeData(dataUnion.data.geoLat, dataUnion.data.geoLng, dataUnion.data.battery);

    Serial.println("");
    Serial.println("Sending Data to SigFox");
    sendToSigFox();
    
    Serial.println("________________________________________");
  }

  Serial.println("");
  Serial.println("Algorithm Complete");
  Serial.println("Going to Sleep");
  SigFox.endPacket();
  LowPower.deepSleep(900000); // sleep for 15 minutes
}
