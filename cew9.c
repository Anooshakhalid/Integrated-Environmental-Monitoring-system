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
};

void getCurrentDateTime(char *date_time) {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    strftime(date_time, 20, "%d %b %Y | %I:%M:%S ", tm_info);
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
            json_object *main_obj, *weather_obj, *wind_obj;

            if (json_object_object_get_ex(json_obj, "main", &main_obj)) {
                weather_data->temperature = json_object_get_double(json_object_object_get(main_obj, "temp"));
                weather_data->humidity = json_object_get_int(json_object_object_get(main_obj, "humidity"));
            }

            if (json_object_object_get_ex(json_obj, "weather", &weather_obj)) {
                json_object *description_obj = json_object_array_get_idx(weather_obj, 0);
                strncpy(weather_data->description, json_object_get_string(json_object_object_get(description_obj, "description")), sizeof(weather_data->description));
            }

            if (json_object_object_get_ex(json_obj, "wind", &wind_obj)) {
                weather_data->wind_speed = json_object_get_double(json_object_object_get(wind_obj, "speed"));
            }
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

int main() {
    const char *api_key = "d2eae61a9dd11b21858134fe58a5d36c";
    char location[100];
    char zipcode[10];
    char countrycode[5];
    char choice_1[2];
    struct WeatherData weather_data;
    char date_time[20]="";

    printf("╔══════════════════════════════════════════╗\n");
    printf("║      ENVIRONMENTAL MONITORING SCREEN     ║\n");
    printf("╚══════════════════════════════════════════╝\n");

    printf("Select method by which you want to know weather\n");
    printf("1) By City Name\n2) By Zip Code & Country Code\n");
    printf("Enter the option no:");
    scanf("%1s", choice_1);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);
       if (strcmp(choice_1, "1") == 0) {
          printf("Enter your city name: ");
          fgets(location, sizeof(location), stdin);
          location[strcspn(location, "\n")] = '\0';
    
          char complete_api_link[256];
          snprintf(complete_api_link, sizeof(complete_api_link), "https://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", location, api_key);

        getWeatherData(complete_api_link, &weather_data);
    } else if (strcmp(choice_1, "2") == 0) {
        printf("Enter Zip Code e.g 94040: ");
        scanf("%9s", zipcode);
        printf("Enter Country Code e.g (US, PK): ");
        scanf("%4s", countrycode);

        char complete_api_link[256];
        snprintf(complete_api_link, sizeof(complete_api_link), "https://api.openweathermap.org/data/2.5/weather?zip=%s,%s&appid=%s", zipcode, countrycode, api_key);

        getWeatherData(complete_api_link, &weather_data);
    } else {
        fprintf(stderr, "Invalid choice\n");
        return 1;
    }

    getCurrentDateTime(date_time);

    printf("+_____________________________________________________________________+\n");
    if (strcmp(choice_1, "1") == 0) {
        printf("|      WEATHER STATS FOR - %s  ||  %s          |\n", location, date_time);
    } else if (strcmp(choice_1, "2") == 0) {
        printf("|      WEATHER STATS FOR - %s  ||  %s          |\n", zipcode, date_time);
    }
    printf("+---------------------------------------------------------------------+\n");

    printf("CURRENT FORECAST:\n");
    printf("Temperature: %.2f°C\n", weather_data.temperature - 273.15);
    printf("Weather: %s\n", weather_data.description);
    printf("Humidity: %d%%\n", weather_data.humidity);
    printf("Wind Speed: %.2f km/h\n", weather_data.wind_speed);
    printf("-----------****------------\n");

    return 0;
}

