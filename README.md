# ARPi Gardener 
## [A]rduino, [R]aspberry [Pi], Gardener. 

An automatic gardener powered with a Raspberry Pi and an Arduino nano. Combined with [&lt;Blynk&gt;](https://blynk.io/) as a user-friendly front-end, it grants incredible ease of use with plenty of room for customization. 

### Why both use an Arduino and a Raspberry Pi? 

Simply because both the Raspberry Pi and Arduino hold various advantages and disadvantages. For example, taking a picture on an ESP12 would take around 80 lines of code, yet on a Raspberry Pi it would only take 1. Reading sensor data however, follows the same logic only backwards, favoring microcontrollers over microcomputers. I then had the issue of figuring out how they will communicate between eachother, and I went about it by creating a very basic language where letters symbolized commands. This meant sending "A" through serial symbolized turned an object on, and "B" would turn it off.  

### How does it work? 

The Raspberry Pi manages all the more complex functions that interpret data, compute basic calculations, and most importantly sync to the blynk cloud. The Arduino Nano however, only knows how to receive commands and send sensor data. 
