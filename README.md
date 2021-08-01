# ESP8266-MCP9808-sensor-plotter
Lots of code found online! Sensitive info: MAC address, Wifi password, API channel and passcode.

115200.

Note:
wifi passcode disabled for joining school network using MAC address. Re-enable if you want to use WPA2 personal.

Data stream https://thingspeak.com/channels/1458402/private_show

Fix done in V2:
- The MCP9808 can't be read immedietly after wakeup command. Or the temperature readout will seem stuck on a value. Solution: never sleep.
- Added detailed serial print info
- Push server to go as fast as possible. Ditched energy saving. 

Note: set the address of the sensor to suit your physical sensor module

![IMG_20210729_005845](https://user-images.githubusercontent.com/22672963/127437670-1c4f9799-a845-419b-8586-e919f29c363f.jpg)
