// Header guard to prevent multiple inclusions of the header
#ifndef GETREALTIMEDATA_H
#define GETREALTIMEDATA_H

// Definition of the WeatherData structure
struct WeatherData {
    double temperature;
    char description[50];
    int humidity;
    double wind_speed;
    double wind_direction;
    double atmospheric_pressure;
};

// Function declarations
void getCurrentDateTime(char *date_time);
const char *getWindDirection(double degrees);
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
void getWeatherData(const char *api_link, struct WeatherData *weather_data);
void storerawdata(const char *raw_data);
void processWeatherData(FILE *inputFile, FILE *outputFile);



// Header guard closing
#endif // GETREALTIMEDATA_H


