a = arduino('/dev/tty.usbmodem1421','uno');
RELAY1 = 7;
for i = 1:10
      writeDigitalPin(a, RELAY1, 0);
      pause(2);
      writeDigitalPin(a, RELAY1, 0);
      pause(2);
end