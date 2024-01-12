#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdarg.h>

#include <time.h>

#include <stdbool.h>

#include <curl/curl.h>

#include <cjson/cJSON.h>

#include <json-c/json.h>

#include <cjson/cJSON.h>

#include "cJSON.h"

#include "reportgenerator.h"

#include "getrealtimedata.h"


// Declare functions from realtimedata.c
extern void getCurrentDateTime(char *date_time);
extern const char *getTimeOfDay(int hour);


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
    
    
    // Get current date and time
    time_t t;
    struct tm *tm_info;
    time(&t);
    tm_info = localtime(&t);


    // Get time of day
    const char *dayPeriod = getTimeOfDay(tm_info->tm_hour);
    char timestamp[50];  // Adjust the size as needed

   // Format time_t value using strftime
   strftime(timestamp, sizeof(timestamp), "%a %b %d | %Y | %I:%M:%S %p", tm_info);






    // Write average values to the output file
     fprintf(outputFile,"\n\n");
   
    fprintf(outputFile, "╓────────────────────────────────────────────────────────────╖\n");
    fprintf(outputFile, "║           OVERALL ENVIRONMENTAL MONITORING SYSTEM          ║\n");
    fprintf(outputFile, "╙────────────────────────────────────────────────────────────╜\n");

 
    fprintf(outputFile, "REPORT GENERATED ON: %s\n", timestamp);
    fprintf(outputFile, "DAY PERIOD: %s\n", dayPeriod);
    
    // Executive Summary
    fprintf(outputFile, "\n\n***  SUMMARY  ***\n-----------------\n");
    fprintf(outputFile, "This report presents a brief overview of environmental conditions in the Generated Area.\n\n");
    
    // Introduction
    fprintf(outputFile, "***  INTRODUCTION  ***\n------------\n");
    fprintf(outputFile, "- The Generated Area is located in (Karachi,Pakistan) covering 24.8607°N latitude and 67.0011°E longitude\n. "
                        "- The report aims to assess the current state of the environment to inform decision-making and potential mitigation measures.\n\n");
                        
    // Study Area Description
    fprintf(outputFile, "***  STUDY AREA DESCRIPTION  ***\n----------------------\n");
    fprintf(outputFile, "- The Generated Area is a diverse landscape that encompasses residential, industrial, recreational, and coastal zones.\n\n"); 
    
    fprintf(outputFile, "***  METHODOLOGY  ***\n------------\n");
    fprintf(outputFile, "- Data was systematically collected through algorithmic processes and monitoring stations.\n"
                        "- Analysis included temperature, pressure, humidity, and wind speed assessments, providing a comprehensive understanding of the environmental dynamics within the Karachi.\n\n");
    
    
    fprintf(outputFile, "-----------------------------------\n");
    fprintf(outputFile, "  📈 ** TRENDS AND ANALYSIS ** \n");
    fprintf(outputFile, "-----------------------------------\n\n");
    
    fprintf(outputFile, "The trends and analysis based on the collected data from environmental monitoring system. Here are the average values for key meteorological factors:\n\n");
    fprintf(outputFile, "Mean Temprature: %.2lf degrees Celsius\n", avgFeelsLike - 273.15);
    fprintf(outputFile, "Average Atmospheric Pressure: %.2lf hPa\n", avgPressure);
    fprintf(outputFile, "Average Humidity: %.2lf%%\n", avgHumidity);
    fprintf(outputFile, "Average Wind Speed: %.2lf km/h\n\n", avgSpeed);

    fprintf(outputFile, "These insights can guide decision-makers in implementing targeted strategies for environmental management and sustainability.\n\n");
    fprintf(outputFile, "----------------------- STAY SAFE ---------------------------\n\n");
    fprintf(outputFile, "* This report has been automatically generated to provide a detailed analysis of environmental conditions in Karachi.*");
    

    // Close the files
    fclose(inputFile);
    fclose(outputFile);
}



void GeneratingReport(const char *inputFileName, const char *outputFileName) {
    FILE *file = fopen(inputFileName, "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening %s for reading\n", inputFileName);
        return;
    }

    int lineCount = 0;
    char line[900];  // Assuming each line has at most 100 characters

    // Count the lines in the file
    while (fgets(line, sizeof(line), file) != NULL) {
        lineCount++;
    }

    // Close the file
    fclose(file);

    // Check if there are at least 4 lines
    if (lineCount >= 4) {
        // Open the input file for reading
        FILE *inputFile = fopen(inputFileName, "r");

        if (inputFile == NULL) {
            fprintf(stderr, "Error opening %s for reading\n", inputFileName);
            return;
        }

        // Open the output file for appending
        FILE *outputFile = fopen(outputFileName, "a");

        if (outputFile == NULL) {
           fprintf(stderr, "Error opening %s for writing\n", outputFileName);
           fclose(inputFile);
        }

        // Process weather data and generate the report
        processWeatherData(inputFile, outputFile);
        
        
    } 
}

