//SOURCE FILE


#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdarg.h>

#include <time.h>

#include <curl/curl.h>

#include <json-c/json.h>

#include "getrealtimedata.h"




// main email function to send mail
// by using 465 port
// by smtp server (simple mail transfer protocol) 


void sendEmailAlert(const char *recipient, const char *subject, const char *body, const char *attachmentFileName) {
    CURL *curl;

    // Initialize the libcurl library
    curl_global_init(CURL_GLOBAL_DEFAULT);


    // Create a CURL object
    curl = curl_easy_init();


    if (curl) {
        // Set the SMTP server details
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
        curl_easy_setopt(curl, CURLOPT_USERNAME, "anoosharaza95@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "qavgqlpevnnhdlhk");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
      

        
        // SSL version to TLSv1.2
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

        // timeout for the email send operation
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

        // recipient email address
        struct curl_slist *recipients = NULL;
        recipients = curl_slist_append(recipients, recipient);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);


        // sender email address
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "anoosharaza95@gmail.com");



        // email headers (including the subject)
        struct curl_slist *headers = NULL;
        char toHeader[100];
        snprintf(toHeader, sizeof(toHeader), "To: %s", recipient);
        headers = curl_slist_append(headers, toHeader);
        char subjectHeader[100];
        snprintf(subjectHeader, sizeof(subjectHeader), "Subject: %s", subject);
        headers = curl_slist_append(headers, subjectHeader);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
        
         // Set up multipart/form-data for both body and attachment
        struct curl_mime *mime;
        struct curl_mimepart *part;

        mime = curl_mime_init(curl);



        // Text part (body)
        part = curl_mime_addpart(mime);
        curl_mime_data(part, body, CURL_ZERO_TERMINATED);
        curl_mime_type(part, "text/plain");


        // Attachment part
        if (attachmentFileName) {
            part = curl_mime_addpart(mime);
            curl_mime_filedata(part, attachmentFileName);
        }

        
        // Set the form data
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        
        // Perform the email send operation
        CURLcode res = curl_easy_perform(curl);


        // Check for errors in sending mail
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Cleanup all values after sending mail
        curl_slist_free_all(recipients);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_mime_free(mime);
        curl_global_cleanup();
    }
}




// function for date and time
void getCurrentDateTime(char *date_time) {

    time_t t;

    struct tm *tm_info;


    time(&t);

    tm_info = localtime(&t);



    strftime(date_time, 50, "%A: %d %b %Y | %I:%M:%S %p", tm_info);
       
}



// function for day
const char *getTimeOfDay(int hour) {
    if (hour >= 0 && hour < 6) {
        return "Midnight";
    } else if (hour < 12) {
        return "Morning";
    } else if (hour == 12) {
        return "Noon";
    } else if (hour < 18) {
        return "Afternoon";
    } else if (hour < 24) {
        return "Night";
    } else {
        return "Invalid hour";
    }
}




// function for wind degrees plus direction (North,East,West,South)
const char *getWindDirection(double degrees) {

    const char *directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};

    int index = (int)(((degrees + 22.5) / 45.0) + 0.5) % 8;

    return directions[index];

}



// Callback function for collecting response data from libcurl requests.
// Appends received data to a dynamically growing buffer.

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {

    size_t realsize = size * nmemb;
    char *memory = (char *)userp;
    strcat(memory, contents);
    return realsize;
}




// function to store raw data retreive from API key
void storerawdata(const char *raw_data) {
    FILE *raw_datafile = fopen("raw_data.txt", "a");  // Open file in append mode

    if (raw_datafile == NULL) {
        fprintf(stderr, "Error opening raw data file for writing\n");
        return;
    }

    fprintf(raw_datafile, "Writing raw data to file:\n%s\n\n", raw_data);

    fclose(raw_datafile);
}




// main weather function
// Function to retrieve weather data from a given API link and populate the WeatherData structure

void getWeatherData(const char *api_link, struct WeatherData *weather_data) {
    CURL *curl;
    CURLcode res;

    
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
      
        //char *buffer = (char *)malloc(1);
        // Memory to store the JSON response
        char memory[2048] = "";


        // Set all the necessary libcurl options
        curl_easy_setopt(curl, CURLOPT_URL, api_link);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, memory);


        // Perform the HTTP request
        res = curl_easy_perform(curl);


        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            exit(EXIT_FAILURE);
        } else {
            
            // Parse JSON response and populate the WeatherData structure
            json_object *json_obj = json_tokener_parse(memory);
            
            
            // Extract and store all the relevant environmental information
            json_object *main_obj, *weather_obj, *wind_obj, *sys_obj, *coord_obj, *lon_obj, *lat_obj, *timezone_obj;

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

            if (json_object_object_get_ex(json_obj, "coord", &coord_obj)) {
                if (json_object_object_get_ex(coord_obj, "lon", &lon_obj)) {
                    weather_data->longitude = json_object_get_double(lon_obj);
                }
                if (json_object_object_get_ex(coord_obj, "lat", &lat_obj)) {
                    weather_data->latitude = json_object_get_double(lat_obj);
                }
            }

            if (json_object_object_get_ex(json_obj, "timezone", &timezone_obj)) {
                weather_data->timezone = json_object_get_int(timezone_obj);
            }
           
        }


        // Cleanup
        curl_easy_cleanup(curl);
    }

    // Cleanup
    curl_global_cleanup();
    
 
}

