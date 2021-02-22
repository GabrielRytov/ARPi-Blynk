#include <Arduino.h>
#include <DHT.h>

#define INT_DHT_PIN 5

#define DHTTYPE DHT22

DHT intDht(INT_DHT_PIN, DHTTYPE);

float intCelcius;
float cacheIntC;


int humidifier = 12;
int fan = 11;
int light = 10;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 30000;

char incomingByte; 

void setup()
{
    Serial.begin(9600);
    intDht.begin();
    startMillis = millis();
    pinMode(humidifier, OUTPUT);
    pinMode(fan, OUTPUT);
    pinMode(light, OUTPUT);
}

void loop()
{
    if (Serial.available() > 0)
    {
        incomingByte = Serial.read(); 
        if (incomingByte == 'A')
        {
            digitalWrite(light, LOW);
        }
        else if (incomingByte == 'B')
            digitalWrite(light, HIGH);

        if (incomingByte == 'C')
        {
            digitalWrite(humidifier, LOW);
        }
        else if (incomingByte == 'D')
            digitalWrite(humidifier, HIGH);

        if (incomingByte == 'E')
        {
            digitalWrite(fan, LOW);
        }
        else if (incomingByte == 'F')
            digitalWrite(fan, HIGH);

    }
    currentMillis = millis();
    if (currentMillis - startMillis >= period)
    {
        intCelcius = intDht.readTemperature();
         if(String(intCelcius) != "nan"){
            cacheIntC = intCelcius; 
        }
        Serial.println(cacheIntC);

        startMillis = currentMillis;
    }
}
