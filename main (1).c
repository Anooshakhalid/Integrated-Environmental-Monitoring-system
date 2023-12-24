#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <curl/curl.h>
#include <json-c/json.h>

struct WeatherData {
    double temperature;
    char description[50];
    int humidity;
    double wind_speed;
    double wind_direction;
    double atmospheric_pressure;
};

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

    // Open a file for writing (append mode)
    FILE *file = fopen("dashboard_output.txt", "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing\n");
        return EXIT_FAILURE;
    }

    printToTerminalAndFile(file, "╔══════════════════════════════════════════╗\n");
    printToTerminalAndFile(file, "║      ENVIRONMENTAL MONITORING SCREEN     ║\n");
    printToTerminalAndFile(file, "╚══════════════════════════════════════════╝\n");

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


