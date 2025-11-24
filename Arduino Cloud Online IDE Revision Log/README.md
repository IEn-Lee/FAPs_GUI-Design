This section is based on version 2.3.4 and documents fixes for known issues.  
The latest version will be available in the [Arduino Cloud Online IDE](https://app.arduino.cc/sketches/b8669c34-cbed-4f0a-9a7e-f5ab9a029865?view-mode=preview).  

Version 2.3.6
- Change the sensor from BMP280 to BME280.

Version 2.3.5
- Use custom function setMillis() to convert millis() from unsigned long to unsigned long long to prevent overflow during long-term operation.
