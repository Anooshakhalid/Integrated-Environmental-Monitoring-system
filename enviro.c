#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "enviro.h"


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


void getWeatherData(const char *api_link, struct WeatherData *weather_data) {

    CURL *curl;

    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();



    if (curl) {

        char memory[2048] = "";

        curl_easy_setopt(curl, CURLOPT_URL, api_link);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, memory);



        res = curl_easy_perform(curl);



        if (res != CURLE_OK) {

            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

            exit(EXIT_FAILURE);

        } else {

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
