#!/usr/bin/env python3
import serial
from firebase import firebase

firebase = firebase.FirebaseApplication("https://rpitest1datetime-default-rtdb.firebaseio.com/",None)

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.reset_input_buffer()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            #firebase.post("/rpitest1datetime-default-rtdb/DateTime",line)
            firebase.put("/rpitest1datetime-default-rtdb/DateTime/-MqKvgXOGOb5NHRyAWNj","Date",line)
            print(line)