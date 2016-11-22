#!/usr/bin/python

import Adafruit_DHT
import sys, getopt

## Supported Sensor Types
DHT11 = Adafruit_DHT.DHT11
DHT22 = Adafruit_DHT.DHT22
AM2302 = Adafruit_DHT.AM2302

def readData(inputConfigs={},tFormat='N'):
    for inputConfig in inputConfigs:
        inputPin = inputConfig
        inputSensorType = inputConfigs[inputConfig]
        hResult, tResult = Adafruit_DHT.read_retry(inputSensorType, inputPin)
        if hResult is not None and tResult is not None:
            if tFormat == 'F':
                print('Pin %s Temp={0:0.1f}*F Humidity={1:0.1f}%%'.format(tResult * 9/5 + 32, hResult)) % (inputPin)
            elif tFormat == 'C':
                print('Pin %s Temp={0:0.1f}*C Humidity={1:0.1f}%%'.format(tResult, hResult)) % (inputPin)
            elif tFormat == 'N':
                print('OK|temp_pin_%s={0:0.1f};;hResult_pin_%s={1:0.1f};;'.format(tResult, hResult)) % (inputPin, inputPin)
            elif tFormat == 'T':
                print('{0:0.0f}'.format(tResult*10))
            elif tFormat == 'H':
                print('{0:0.0f}'.format(hResult*10))
        else:
            if tFormat == 'N':
                print('FAIL|temp_pin_%s=NaN;;hResult_pin_%s=NaN;;') % (inputPin, inputPin)
            else:
                print('Failed to get reading for pin %s. Try again!') % (inputPin)


def main(argv):
    ## Default Config
    inputConfigs = {17:DHT11, 22:DHT11, 23:DHT11, 24:DHT11, 27:DHT11}
    tFormat = 'N'

    try:
        opts, args = getopt.getopt(argv,"c:f:h",["config=","format="])
    except getopt.GetoptError:
        print 'dhtDatasource.py -c "{pin#:sensorType,}" -f [C, F, N, H, T]'
        print 'C - Deg C Human readable'
        print 'F - Deg F Human readable'
        print 'N - Deg C Nagios readable'
        print 'T - Deg C SNMP readable temp only. Value * 10 to maintain first significant digit.'
        print 'H - SNMP readable humidity only. Value * 10 to maintain first significant digit.'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'dhtDatasource.py -c "{pin#:sensorType,}" -f [C, F, N, H, T]'
            print 'C - Deg C Human readable'
            print 'F - Deg F Human readable'
            print 'N - Deg C Nagios readable'
            print 'T - Deg C SNMP readable temp only. Value * 10 to maintain first significant digit.'
            print 'H - SNMP readable humidity only. Value * 10 to maintain first significant digit.'
            sys.exit()
        elif opt in ("-c", "--config"):
            inputConfigs = eval(arg)
        elif opt in ("-f", "--format"):
            tFormat = str(arg).upper()
    readData(inputConfigs=inputConfigs,tFormat=tFormat)

if __name__ == "__main__":
   main(sys.argv[1:])
