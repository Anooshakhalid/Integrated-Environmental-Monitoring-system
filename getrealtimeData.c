#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "getrealtimeData.h"
#include "cJSON.h"

void getCurrentDateTime(char *date_time) {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    strftime(date_time, 50, "%d %b %Y | %I:%M:%S %p", tm_info);
}

const char *getWindDirection(double degrees) {
    const char *directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    int index = (int)(((degrees + 22.5) / 45.0) + 0.5) % 8;
    return directions[index];
}



size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    char *memory = (char *)userp;

    strcat(memory, contents);
    return realsize;
}



void storerawdata(const char *raw_data) {
    FILE *raw_datafile = fopen("raw_data.txt", "a");  // Open file in append mode
    if (raw_datafile == NULL) {
        fprintf(stderr, "Error opening raw data file for writing\n");
        return;
    }
    fprintf(raw_datafile, "%s\n", raw_data);

    fclose(raw_datafile);
}

void processWeatherData(FILE *inputFile, FILE *outputFile) {
    char line[1024];

    double totalFeelsLike = 0.0, totalPressure = 0.0, totalHumidity = 0.0, totalSpeed = 0.0;
    int dataCount = 0;

    // Read each line from the input file
    while (fgets(line, sizeof(line), inputFile) != NULL) {
        // Parse the line to extract relevant weather data
        cJSON *json = cJSON_Parse(line);
        if (json != NULL) {
            // Extract values from JSON
            cJSON *main = cJSON_GetObjectItem(json, "main");
            cJSON *wind = cJSON_GetObjectItem(json, "wind");

            if (main != NULL && wind != NULL) {
                double feels_like = cJSON_GetObjectItem(main, "feels_like")->valuedouble;
                double pressure = cJSON_GetObjectItem(main, "pressure")->valuedouble;
                double humidity = cJSON_GetObjectItem(main, "humidity")->valuedouble;
                double speed = cJSON_GetObjectItem(wind, "speed")->valuedouble;

                // Update cumulative values
                totalFeelsLike += feels_like;
                totalPressure += pressure;
                totalHumidity += humidity;
                totalSpeed += speed;

                // Increment the data count
                dataCount++;
            }

            // Free cJSON object
            cJSON_Delete(json);
        }
    }

    // Calculate averages
    double avgFeelsLike = totalFeelsLike / dataCount;
    double avgPressure = totalPressure / dataCount;
    double avgHumidity = totalHumidity / dataCount;
    double avgSpeed = totalSpeed / dataCount;

    // Write average values to the output file
    fprintf(outputFile,"╔══════════════════════════════════════════╗\n");
    fprintf(outputFile,"║      Overall Weather Report              ║\n");
    fprintf(outputFile,"╚══════════════════════════════════════════╝\n");
    fprintf(outputFile, "Average Temprature: %.2lf\n", avgFeelsLike);
    fprintf(outputFile, "Average Pressure: %.2lf\n", avgPressure);
    fprintf(outputFile, "Average Humidity: %.2lf\n", avgHumidity);
    fprintf(outputFile, "Average Wind Speed: %.2lf\n", avgSpeed);

    // Close the files
    fclose(inputFile);
    fclose(outputFile);
}



void getWeatherData(const char *api_link, struct WeatherData *weather_data) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        char memory[2048] = "";
        FILE *file = fopen("raw_data.json", "w");
        if (!file) {
            fprintf(stderr, "Error opening file for writing\n");
            exit(EXIT_FAILURE);
        }
        curl_easy_setopt(curl, CURLOPT_URL, api_link);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, memory);

        res = curl_easy_perform(curl);
        fclose(file);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            exit(EXIT_FAILURE);
        } else {
            storerawdata(memory);
            json_object *json_obj = json_tokener_parse(memory);
            json_object *main_obj, *weather_obj, *wind_obj, *sys_obj;

            if (json_object_object_get_ex(json_obj, "main", &main_obj)) {
                weather_data->temperature = json_object_get_double(json_object_object_get(main_obj, "temp"));
                weather_data->humidity = json_object_get_int(json_object_object_get(main_obj, "humidity"));
                weather_data->atmospheric_pressure = json_object_get_double(json_object_object_get(main_obj, "pressure"));
            }

            if (json_object_object_get_ex(json_obj, "weather", &weather_obj)) {
                json_object *description_obj = json_object_array_get_idx(weather_obj, 0);
                strncpy(weather_data->description, json_object_get_string(json_object_object_get(description_obj, "description")), sizeof(weather_data->description));
            }

            if (json_object_object_get_ex(json_obj, "wind", &wind_obj)) {
                weather_data->wind_speed = json_object_get_double(json_object_object_get(wind_obj, "speed"));
                weather_data->wind_direction = json_object_get_double(json_object_object_get(wind_obj, "deg"));
            }
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}


char *get_command_line(); // Function declaration

bool is_running_directly() {
    char *cmdline = get_command_line();
    return strstr(cmdline, "main") != NULL;
}

char *get_command_line() {
    FILE *cmdline_file = fopen("/proc/self/cmdline", "r");

    if (cmdline_file == NULL) {
        perror("Error opening /proc/self/cmdline");
        exit(EXIT_FAILURE);
    }

    size_t bufsize = 1024;
    char *cmdline = (char *)malloc(bufsize);

    if (cmdline == NULL) {
        perror("Error allocating memory for command line");
        exit(EXIT_FAILURE);
    }

    ssize_t read_size = getline(&cmdline, &bufsize, cmdline_file);

    if (read_size == -1) {
        perror("Error reading /proc/self/cmdline");
        exit(EXIT_FAILURE);
    }

    fclose(cmdline_file);

    return cmdline;
}

