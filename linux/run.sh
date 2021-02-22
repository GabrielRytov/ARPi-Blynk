#!/bin/bash
echo "----------------------- BUILDING -----------------------"
echo ""
sudo make clean all target=raspberry
echo ""
echo "-------------------- BUILD FINISHED --------------------"
echo "------------------------ RUNNING -----------------------"
echo ""
sudo ./blynk --token= YOUR TOKEN HERE