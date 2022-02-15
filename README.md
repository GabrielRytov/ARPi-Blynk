# ARPi Gardener 
## [A]rduino, [R]aspberry [Pi], Gardener. 

An automatic gardener powered with a Raspberry Pi and an Arduino nano. Combined with [Blynk](https://blynk.io/) as a user-friendly front-end, it grants incredible ease of use with plenty of room for customization. 

### Why use both an Arduino and a Raspberry Pi? 

The Raspberry Pi and Arduino are very different. Microcontrollers can interpret non-binary data from sensors, and generally tend to have native PWM. The Raspberry Pi however, being a mini-computer, is much better at being a webserver and handling process heavy tasks rather than reading sensor data. So why not get the best of both worlds and combine the two? 

### How does it work? 

The Raspberry Pi (master) manages all the more complex functions that interpret data, compute basic calculations, and most importantly sync with the blynk cloud. The Arduino Nano (slave) however, only knows how to receive commands and send sensor data. The intervals are static and have to be implemented before starting the blynk server, however there are options to remotely modify variables for those looking to broaden their range. 

I then had the issue of figuring out how they will communicate between each other, and I went about it by creating a very basic dictionary to execute commands. This meant sending "A" through serial would execute `digitalWrite(x, HIGH)` on the nano; "B" would do just the opposite, executing `digitalWrite(x, LOW)`. As written in `src.cpp`:

```cpp
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
```

Modifying the code is very straight forward. All the blynk related code is on the Raspberry Pi under `/linux/main.cpp`, and the code for the nano is at `/src.cpp`. 
