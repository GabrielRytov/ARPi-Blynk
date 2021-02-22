/**
 * @file       main.cpp
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Mar 2015
 * @brief
 */

//#define BLYNK_DEBUG
#define BLYNK_PRINT stdout
#ifdef RASPBERRY
#include <BlynkApiWiringPi.h>
#else
#include <BlynkApiLinux.h>
#endif
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

static const char *auth, *serv;
static uint16_t port;

#include <BlynkWidgets.h>
/*       ^^^       Required Blynk Resources!!      ^^^       */
#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

BlynkTimer timer;

int Serial;
char intTemp[5];
char cacheTemp[5]; 
bool firstCheck = false;
int currentHour, currentMin, totalMin;

struct device
{
public:
  int pin;
  int offHours;
  int offMinutes;
  bool pwrStatus = false;
  int iterations;
  int duration;
  float value;

  void setTime(int setHours, int setMinutes) //This function sets the time which a device turns OFF
  {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    currentHour = tm.tm_hour;
    currentMin = tm.tm_min;
    offHours = setHours + currentHour;
    offMinutes = setMinutes + currentMin;
    if (offMinutes > 59)
    {
      offMinutes = offMinutes - 60;
      offHours++;
    }
    if (offHours > 23)
    {
      offHours = offHours - 24;
    }
  }
} humidifier, fan, lights;

void setupDevices()
{
  humidifier.iterations = 12;
  humidifier.duration = 1;

  fan.iterations = 6;
  fan.duration = 1;
}

void ventilate(int duration)
{
  serialPutchar(Serial, 'F'); // Turns on fan
  Blynk.virtualWrite(V2, 1);
  fan.setTime(0, duration);
  fan.pwrStatus = true;
}

void mist(int duration)
{
  serialPutchar(Serial, 'D'); // Turns on humidifier
  Blynk.virtualWrite(V1, 1);
  humidifier.setTime(0, duration);
  humidifier.pwrStatus = !humidifier.pwrStatus;
}

void routine()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  currentHour = tm.tm_hour;
  currentMin = tm.tm_min;
  totalMin = (currentHour * 60) + currentMin;
  int humIntervals = (((24 / humidifier.iterations) * 60) - 2); // Removed a minute so fan intervals was always 1 minute ahead of hum intervals
  int fanIntervals = (24 / fan.iterations) * 60; 
  printf("\n Current hour: %i \n", currentHour);
  printf("Current min: %i \n", currentMin);
  if ((totalMin % humIntervals) == 0)
  {
    mist(humidifier.duration);
  }
  if ((totalMin % fanIntervals) == 0)
  {
    ventilate(fan.duration);
  }
  if (humidifier.offHours == currentHour && humidifier.offMinutes == currentMin)
  {
    serialPutchar(Serial, 'C'); // Turns off humidifier
    humidifier.pwrStatus = false;
    Blynk.virtualWrite(V1, 0);
  }
  if (fan.offHours == currentHour && fan.offMinutes == currentMin)
  {
    serialPutchar(Serial, 'E'); // Turns off fan
    fan.pwrStatus = false;
    Blynk.virtualWrite(V2, 0);
    // The next segment takes automatically takes a picture after the fan turns off - optional 
    Blynk.virtualWrite(V0, 1);
    printf("Lights turned on.\n");
    lights.pwrStatus = true;
    serialPutchar(Serial, 'B');
    serialFlush(Serial);
    system("/home/pi/scripts/capture.sh");
  }
  if ((currentHour <= 4) || (currentHour >= 17))
  {
    serialPutchar(Serial, 'A'); // Turns off lights
    lights.pwrStatus = false;
    Blynk.virtualWrite(V0, 0);
  }
  else
  {
    serialPutchar(Serial, 'B'); // Turns on lights
    lights.pwrStatus = true;
    Blynk.virtualWrite(V0, 1);
  }
}

void requestData()
{
  char dataBuffer[5];
  int i = 0;
  while (serialDataAvail(Serial) > -1)
  {
    char inChar = serialGetchar(Serial);
    dataBuffer[i] = inChar;
    i++;
    if (i == 5)
    {
      dataBuffer[5] = '\0';
      memcpy(intTemp, dataBuffer, 5);
      cacheTemp = intTemp; 
      Blynk.virtualWrite(V4, intTemp);
      break;
    }
  }
}

void all()
{
  requestData();
  routine();
}

void setup()
{
  setupDevices();
  Blynk.begin(auth, serv, port);
  if (wiringPiSetup() == -1)
  {
    fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
  }
  if ((Serial = serialOpen("/dev/ttyACM0", 9600)) < 0) // CRITICAL: Check that the serial port is correct, or else nothing will work!
  {
    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
  }
  timer.setInterval(30000L, all);
}

void loop()
{
  Blynk.run();
  timer.run();
}

BLYNK_CONNECTED()
{
  Blynk.syncAll();
  Blynk.virtualWrite(V4, intTemp); 
  Blynk.virtualWrite(V0, lights.pwrStatus); 
  Blynk.virtualWrite(V1, humidifier.pwrStatus); 
  Blynk.virtualWrite(V2, fan.pwrStatus); 
}

// Remote restart switch
BLYNK_WRITE(V22)
{
  int pinVal = param.asInt();
  if (pinVal == 1)
  {
    system("reboot");
  }
}

// In-app lights control
BLYNK_WRITE(V0)
{
  int pinData = param.asInt();
  if (pinData == 1)
  {
    lights.pwrStatus = true;
    printf("Lights turned on.\n");
    serialPutchar(Serial, 'B');
    serialFlush(Serial);
  }
  else if (pinData == 0)
  {
    lights.pwrStatus = false;
    printf("Lights turned off.\n");
    serialPutchar(Serial, 'A');
    serialFlush(Serial);
  }
}

// In-app humidifier control
BLYNK_WRITE(V1)
{
  int pinData = param.asInt();
  if (pinData == 1)
  {
    humidifier.pwrStatus = true;
    printf("Humidifier turned on.\n");
    serialPutchar(Serial, 'D');
    serialFlush(Serial);
  }
  else if (pinData == 0)
  {
    humidifier.pwrStatus = false;
    printf("Humidifier turned off.\n");
    serialPutchar(Serial, 'C');
    serialFlush(Serial);
  }
}

// In-app fan control
BLYNK_WRITE(V2)
{
  int pinData = param.asInt();
  if (pinData == 1)
  {
    fan.pwrStatus = true;
    printf("Fan turned on.\n");
    serialPutchar(Serial, 'F');
    serialFlush(Serial);
  }
  else if (pinData == 0)
  {
    fan.pwrStatus = false;
    printf("Fan turned off.\n");
    serialPutchar(Serial, 'E');
    serialFlush(Serial);
  }
}

// Take a photo button
BLYNK_WRITE(V25)
{
  int pinData = param.asInt();
  if (pinData == 1)
  {
    Blynk.virtualWrite(V0, 1);
    printf("Lights turned on.\n");
    lights.pwrStatus = true;
    serialPutchar(Serial, 'B');
    serialFlush(Serial);
    system("/home/pi/scripts/capture.sh"); 
  }
}

int main(int argc, char *argv[])
{
  parse_options(argc, argv, auth, serv, port);

  setup();
  while (true)
  {
    loop();
  }

  return 0;
}
