Integrated-Environmental-Monitoring-system
-------------------------------------------

Description
-----------
Integrated-Environmental-Monitoring-system is a C shell application that retrieve weather data from https://openweathermap.org/ public API.

[image](https://github.com/Anooshakhalid/Integrated-Environmental-Monitoring-system/assets/144916298/16ca2f12-0763-4613-8894-7f3a554e72c3)


Dependencies
------------
The application has two main dependencies:

1) https://curl.se/libcurl/ to make an HTTP call to the API and get the data in raw JSON format.
2) https://github.com/DaveGamble/cJSON, a library used to parse the JSON intp a workable data structure.
While cJSON is included into the project libcurl is not, so be sure that you run the application on a system where libcurl is installed.

Build
-----
Integrated-Environmental-Monitoring-system is built with gcc; all generated objects files are placed in repository and folders in it and linked with the required dependencies.
The Integrated-Environmental-Monitoring-system is executable by main1.c file.

How it works
------------
We hard coded the city "Karachi" to the API. Any city whose data has to be monitored can be hard coded to the free api. 
Our program run by Auotomation on our Linux machine 7 days of the week. For Automation we have made a bash file named enviro_system.sh which runs by cron jab applied in our machine. In each execution it fatches the weather data from he API in the json form, store raw data, shows a short report on terminal showing Temperature, Weather, Humidity, Wind Speed, Wind Direction, Atmospheric Pressure. A threshold value of Humidity has been set. Whenever the Humidity eceeds the threshold value an email alert is sent to the user while keeping track of the email record in an another text file. At the end of the week when the record of all 7 days weather condition has been recorded, our program generates a Report of whole week's weather trends.

Usage
-----
Clone the repository
Just compile the main1.c file and run its executable
