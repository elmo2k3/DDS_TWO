#!/usr/bin/env python
import serial
import time

ser = serial.Serial(port='/dev/ttyUSB0',baudrate=115200,xonxoff=True);
f = open('DDS_TWO.hex','r');
a = f.read();
ser.write('a');
time.sleep(0.4);
ser.write(a);
