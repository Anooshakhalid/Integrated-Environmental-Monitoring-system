//HEADER FILE


// Header guard to prevent multiple inclusions of the header
#ifndef GETREALTIMEDATA_H
#define GETREALTIMEDATA_H



// Definition of the EnvironmentalData structure
//struct for storing various environment-related parameters obtained from the API response

struct WeatherData {
    double temperature;
    char description[50];
    int humidity;
    double wind_speed;
    double wind_direction;
    double atmospheric_pressure;
    double longitude;
    double latitude;
    int timezone;
    int visibility;
};



// Function declarations
void getCurrentDateTime(char *date_time);   //for date,time 


const char *getWindDirection(double degrees);    // for wind directions


//size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);    // for memory allocation


size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);



void getWeatherData(const char *api_link, struct WeatherData *weather_data);



void storerawdata(const char *raw_data);      // for storing raw data fetched from API


const char *getTimeOfDay(int hour);   // for dayperiod


void sendEmailAlert(const char *recipient, const char *subject, const char *body, const char *attachmentFileName);   // for send an email alert




// Header guard closing
#endif // GETREALTIMEDATA_H

