# Overview
Senso is an application that aims to combat illegal logging. Senso is an affordable way to notify authorities when logging takes place so that they can take action on the concern. 

Senso relies on an array of devices on the field monitoring sound frequency in search for sounds emitted by forest cutting machinery. When a positive frequency match occurs, the device will send a notification to the backend via SigFox. The data is then passed through Azure and into Power Bi where the user can visualise where logging takes place and when it did being able to take action.

The device also sends its current battery voltage (feature in development) to the backend. The user can visualise this data on a graph in the backend and predict when the device will require maintenance.

# Device Overview
The application will use an Arduino MKR FOX as the main microcontroller in the fronend. The MKR GSM is used due to its autonomy on the field and long battery life thanks to its low power mode.

# Use Cases
The application is aimed at detecting logging and informing appropriate authorities about the logging. The authorities can easily visualise areas in which logging takes place on a map on the dashboard.

# Project Architecture
The device is split into a frontend and a backend. The frontend of the device will wake up at intervals of time and take samples of sound searching for logging machines. If there is a match, the device will send the event to the backend through SigFox and will then go back to sleep.

The data is then relayed from SigFox to Azure which then illustrates it in a Power Bi report. The user can access this report. The areas where there were trees cut are plotted in red on the map and the battery voltage is plotted on a graph.

# Link
A link to the full publication has been included below. https://www.hackster.io/LightPro/senso-c00153
