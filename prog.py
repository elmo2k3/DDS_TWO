#!/usr/bin/env python
import serial
import time
import sys

ser = serial.Serial(port=sys.argv[1],baudrate=115200,xonxoff=True);
f = open('DDS_TWO.hex','r');
a = f.read();
ser.write('a');
time.sleep(0.4);
ser.write(a);
