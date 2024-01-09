#pragma execution_character_set("utf-8")
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdarg.h>

#include <time.h>

#include <curl/curl.h>

#include <json-c/json.h>

#include "getrealtimedata.h"

#include "getrealtimedata.c"








void printToTerminalAndFile(FILE *file, const char *format, ...) {

    va_list args;

    va_start(args, format);



    // Make a copy of va_list before using it for terminal printing

    va_list terminal_args;

    va_copy(terminal_args, args);



    // Print to terminal

    vprintf(format, terminal_args);



    // Reset va_list for file printing

    va_end(terminal_args);

    va_start(args, format);



    // Print to file

    vfprintf(file, format, args);



    va_end(args);

}

int main() {

    const char *api_key = "d2eae61a9dd11b21858134fe58a5d36c";

    struct WeatherData weather_data;

    char date_time[50] = "";
    
    
    if (is_running_directly()) {
        printf("Program is running directly.\n");
    } else {
        printf("Program is not running directly.\n");
    }

    FILE *file = fopen("dashboard_output.txt", "a");

    if (file == NULL) {

        fprintf(stderr, "Error opening file for writing\n");

        return EXIT_FAILURE;

    }


    printf( "╔══════════════════════════════════════════╗\n");

    printf( "║      ENVIRONMENTAL MONITORING SCREEN     ║\n");

    printf( "╚══════════════════════════════════════════╝\n");


    char complete_api_link[256];

    snprintf(complete_api_link, sizeof(complete_api_link), "https://api.openweathermap.org/data/2.5/weather?q=Karachi,PK&appid=%s", api_key);



    getWeatherData(complete_api_link, &weather_data);

    getCurrentDateTime(date_time);
    printToTerminalAndFile(file, "Location: Karachi, Pakistan\n");



    printToTerminalAndFile(file, "+_______________________________________________________+\n");

    printToTerminalAndFile(file, "|      CURRENT STATS ||  %s     |\n", date_time);

    printToTerminalAndFile(file, "+------------------------------------------------------+\n");



    printToTerminalAndFile(file, "Temperature: %.2f°C\n", weather_data.temperature - 273.15);

    printToTerminalAndFile(file, "Weather: %s\n", weather_data.description);

    printToTerminalAndFile(file, "Humidity: %d%%\n", weather_data.humidity);

    printToTerminalAndFile(file, "Wind Speed: %.2f m/s\n", weather_data.wind_speed);

    printToTerminalAndFile(file, "Wind Direction: %.2f° %s\n", weather_data.wind_direction, getWindDirection(weather_data.wind_direction));

    printToTerminalAndFile(file, "Atmospheric Pressure: %.2f hPa\n", weather_data.atmospheric_pressure);

    printToTerminalAndFile(file, "--------------*****-------------\n");

    printToTerminalAndFile(file, "\n");

    



    // Close the file

    fclose(file);




    return 0;

}
