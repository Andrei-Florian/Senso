# Overview
Senso is an application that aims to combat illegal logging. Senso is an affordable way to notify authorities when logging takes place so that they can take action on the concern. 

Senso relies on an array of devices on the field monitoring sound frequency in search for sounds emitted by forest cutting machinery. When a positive frequency match occurs, the device will send a notification to the backend via SigFox. This message is then processed, and the user receives a notification that the sensor was triggered.

# Device Overview
The application will use an Arduino MKR FOX as the main microcontroller in the fronend. The MKR GSM is used due to its autonomy on the field and long battery life thanks to its low power mode.

# Use Cases
The application is aimed at detecting logging and informing appropriate authorities about the logging. The system also implies that the authorities can check if the logging was authorised and take appropriate action.
