#include <Arduino.h>
#include <DHT.h>
#include <Servo.h>

#define INT_DHT_PIN 4
#define EXT_DHT_PIN 5

#define DHTTYPE DHT22

DHT intDht(INT_DHT_PIN, DHTTYPE);
DHT extDht(EXT_DHT_PIN, DHTTYPE);
Servo remote;

float intHumidity;
float intCelcius;
float extHumidity;
float extCelcius;

int humidifier = 12;
int fan = 11;
int light = 10;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 2000;

bool acPwrStatus = false;

char incomingByte[4];

void acSwitch()
{
    remote.write(55);
    delay(400);
    remote.write(100);
    delay(400);
    remote.write(55);
    acPwrStatus = !acPwrStatus;
}

void setup()
{
    Serial.begin(9600);
    remote.attach(6);
    remote.write(55);
    intDht.begin();
    extDht.begin();
    startMillis = millis();
    pinMode(humidifier, OUTPUT);
    pinMode(fan, OUTPUT);
    pinMode(light, OUTPUT);
}

void loop()
{
    if (Serial.available() > 0)
    {
        Serial.readBytesUntil('\n', incomingByte, sizeof(incomingByte));
        if (incomingByte[0] == 'A')
        {
            digitalWrite(light, LOW);
        }
        else if (incomingByte[0] == 'B')
            digitalWrite(light, HIGH);

        if (incomingByte[1] == 'C')
        {
            digitalWrite(humidifier, LOW);
        }
        else if (incomingByte[1] == 'D')
            digitalWrite(humidifier, HIGH);

        if (incomingByte[2] == 'E')
        {
            digitalWrite(fan, LOW);
        }
        else if (incomingByte[2] == 'F')
            digitalWrite(fan, HIGH);

        if (incomingByte[3] == 'G')
        {
            if (acPwrStatus == true)
                ;
            acSwitch();
        }
        else if (incomingByte[3] == 'H')
        {
            if (acPwrStatus == false)
                ;
            acSwitch();
        }
    }
    currentMillis = millis();
    if (currentMillis - startMillis >= period)
    {
        extCelcius = extDht.readTemperature();
        intCelcius = intDht.readTemperature();
        intHumidity = intDht.readHumidity();
        Serial.println(extCelcius);
        Serial.println(intCelcius);
        Serial.println(intHumidity);
        startMillis = currentMillis;
    }
}
