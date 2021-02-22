# ARPi Gardener 
## [A]rduino, [R]aspberry [Pi], Gardener. 

An automatic gardener powered with a Raspberry Pi and an Arduino nano. Combined with [&lt;Blynk&gt;](https://blynk.io/) as a user-friendly front-end, it grants incredible ease of use with plenty of room for customization. 

### Why both use an Arduino and a Raspberry Pi? 

Simply because both Raspberry Pi and Arduino hold various advantages and disadvantages. For example, taking a picture on an ESP12 would take around 80 lines of code, yet on a Raspberry Pi it would only take 1. Reading sensor data however, follows the same logic only backwards, favoring microcontrollers over microcomputers. I then had the issue of figuring out how they will communicate between eachother, and I went about it by creating a very basic language that symbolized commands. This meant sending "A" through serial  symbolized turning something on, and "B" would turn it off.  
