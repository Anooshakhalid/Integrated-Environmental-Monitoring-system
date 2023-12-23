#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <json-c/json.h>

struct WeatherData {
    double temperature;
    char description[50];
    int humidity;
    double wind_speed;
    double atmospheric_pressure;
};

void getCurrentDateTime(char *date_time) {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    strftime(date_time, 30, "%d %b %Y | %I:%M:%S %p", tm_info);
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
            }

            printf("Location: Karachi, Pakistan\n");

            if (json_object_object_get_ex(json_obj, "sys", &sys_obj)) {
                int sunrise = json_object_get_int(json_object_object_get(sys_obj, "sunrise"));
                int sunset = json_object_get_int(json_object_object_get(sys_obj, "sunset"));
                time_t current_time = time(NULL);

                if (current_time > sunrise && current_time < sunset) {
                    // Daytime
                    printf("Day Period: Noon\n");
                } else if (current_time < sunrise) {
                    // Morning
                    printf("Day Period: Morning\n");
                } else {
                    // Evening
                    printf("Day Period: Evening\n");
                }
            }
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}


int main() {
    const char *api_key = "d2eae61a9dd11b21858134fe58a5d36c";
    struct WeatherData weather_data;
    char date_time[50] = "";

    printf("╔══════════════════════════════════════════╗\n");
    printf("║      ENVIRONMENTAL MONITORING SCREEN     ║\n");
    printf("╚══════════════════════════════════════════╝\n");

    char complete_api_link[256];
    snprintf(complete_api_link, sizeof(complete_api_link), "https://api.openweathermap.org/data/2.5/weather?q=Karachi,PK&appid=%s", api_key);

    getWeatherData(complete_api_link, &weather_data);
    getCurrentDateTime(date_time);

    printf("+_______________________________________________________+\n");
    printf("|      CURRENT STATS ||  %s     |\n", date_time);
    printf("+------------------------------------------------------+\n");

    printf("Temperature: %.2f°C\n", weather_data.temperature - 273.15);
    printf("Weather: %s\n", weather_data.description);
    printf("Humidity: %d%%\n", weather_data.humidity);
    printf("Wind Speed: %.2f m/s\n", weather_data.wind_speed);
    printf("Atmospheric Pressure: %.2f hPa\n", weather_data.atmospheric_pressure);
    printf("--------------*****-------------\n");

    return 0;
}

