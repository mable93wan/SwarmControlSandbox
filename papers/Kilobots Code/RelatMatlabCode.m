a = arduino('/dev/tty.usbmodem1411','uno');

RELAY1 = 7;
RELAY2 = 6;
RELAY3 = 5;
RELAY4 = 4;
RELAY5 = 3;
RELAY6 = 2;
RELAY7 = 1;
RELAY8 = 0;
for i = 1:10
      writeDigitalPin(a, RELAY3, 0);
      pause(2);
end