// Author: Michael Tuskan
// Creation Date: 08-30-2022
// Program Function: HPA Pressure Transducer Sensor

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID  "TMPLM4LKi4fO"
#define BLYNK_DEVICE_NAME  "HPA Pressure Transducer Sensor"
#define BLYNK_AUTH_TOKEN   "AaLfWtnPxhns_ri-qd2_bAGwiDpYeba2"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "CenturyLink2043";
char pass[] = "td5q2qix87hs6u";

#define VPIN_CURRENT_PRESSURE  V4
#define VPIN_PRESSURE_ALERT    V5

const double    pressureInput        = A0;      // Analog Output Channel
const double    pressureZero         = 104.5;   // 0.32 Volts (Actual low voltage level) = 0.0 +-0.2  (went UP from 104.5)
const double    pressureMax          = 919.5;   // 4.5 Volts
const double    pressureTransMaxPsi  = 150;     // pressure transducer 150PSI MAX output
const double    sensorReadDelay      = 250;     //
double          pressureValue        = 0;       // PSI value of current pressure of system
String          overPressureAlert;
String          underPressureAlert;
boolean         overAlertCounter;
boolean         underAlertCounter;
BlynkTimer      timer;

// RESET function
// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);

  Blynk.virtualWrite(V5, "Normal Operation");
  overAlertCounter = 0;
  underAlertCounter = 0;
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
//  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
//  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
//  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

void criticalPressureEvent()
{
  if(overAlertCounter = true){
    Blynk.logEvent("over_pressure_alert");
    overAlertCounter = false;
  }
  if(underAlertCounter = true){
    Blynk.logEvent("under_pressure_alert");
    underAlertCounter = false;
  }
}

void criticalPressureAlert()
{
  if (pressureValue >= 125) {
    Blynk.virtualWrite(V5, overPressureAlert);  //Send alert to Blynk if pressure is over 120 PSI
    overAlertCounter = true;
  } else if (pressureValue <= 80) {
    Blynk.virtualWrite(V5, underPressureAlert);  //Send alert to Blynk if pressure is under 80 PSI
    underAlertCounter = true;
  } else {
    Blynk.virtualWrite(V5, "Normal Operation");  //Send alert to Blynk if pressure 
  }
}

void pressureTransducer() 
{
  pressureValue = analogRead(pressureInput);      
  pressureValue = (((pressureValue - pressureZero) * pressureTransMaxPsi) / (pressureMax - pressureZero));
  Serial.print(pressureValue, 2);
  Serial.println("PSI");
  delay(sensorReadDelay);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  overPressureAlert              = "PRESSURE OVER 125 PSI!!!";
  underPressureAlert             = "PRESSURE UNDER 80 PSI!!!";
  overAlertCounter               = false;
  underAlertCounter              = false;
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  pressureTransducer();
  Blynk.virtualWrite(VPIN_CURRENT_PRESSURE, pressureValue);
  criticalPressureAlert();
  //criticalPressureEvent();
}
