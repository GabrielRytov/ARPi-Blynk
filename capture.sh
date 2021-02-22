#!/bin/bash

raspistill -v -o OUTPUT_FILENAME
ncftpput -u HOST -p PASSWORD URL DIRECTORY LOCALFILE
