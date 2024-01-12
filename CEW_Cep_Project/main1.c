#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdarg.h>

#include <stdbool.h>

#include <time.h>

#include <curl/curl.h>

#include <cjson/cJSON.h>

#include <json-c/json.h>

#include "getrealtimedata.h"

#include "getrealtimedata.c"

#include "cJSON.h"

#include "reportgenerator.h"

#include "reportgenerator.c"


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


// main interface
int main() {


    // API KEY
    const char *api_key = "d2eae61a9dd11b21858134fe58a5d36c";


    struct WeatherData weather_data;


    char date_time[50] = "";
    
    
    // For Day period
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    int currentHour = tm_info->tm_hour;

    const char *timeOfDay = getTimeOfDay(currentHour);



    // Filing for Dashboard, to store processed data

    FILE *dashboardfile = fopen("dashboard_output.txt", "a");

    if (dashboardfile == NULL) {

        fprintf(stderr, "Error opening file for writing\n");

        return EXIT_FAILURE;

    }
    
    
    
    // pretty printing for dashboard
    
    printf( "╔════════════════════════════════════════════════════════════╗\n");

    printf( "║                ENVIRONMENTAL MONITORING SYSTEM             ║\n");

    printf( "╚════════════════════════════════════════════════════════════╝\n");



    char complete_api_link[256];

    snprintf(complete_api_link, sizeof(complete_api_link), "https://api.openweathermap.org/data/2.5/weather?q=Karachi,PK&appid=%s", api_key);


    getWeatherData(complete_api_link, &weather_data);
    //displayAlert(&weather_data);

    getCurrentDateTime(date_time);
    
    
    // greetings for our system
    printf("\nWelcome to Our Environmental Monitoring System! 🌍");
    printf("\nStay informed with real-time environment information. Explore the dashboard for insights and make eco-friendly decisions.\n\n"); 



    printToTerminalAndFile(dashboardfile, "* LOCATION : KARACHI, PAKISTAN *\n");
    printToTerminalAndFile(dashboardfile, "Province : Sindh\n");
    printToTerminalAndFile(dashboardfile, "Time zone in Karachi (GMT+5): %d\n", weather_data.timezone);
    printToTerminalAndFile(dashboardfile, "Latitude: %.4f \n", weather_data.latitude);
    printToTerminalAndFile(dashboardfile, "Longitude: %.4f \n", weather_data.longitude);
    

    
    // pretty printing to print day,date and time
    printToTerminalAndFile(dashboardfile, "+--------------------------------------------------------------------+\n");
    
    printToTerminalAndFile(dashboardfile, "|          CURRENT STATS ||  %s    |\n", date_time);
    
    printToTerminalAndFile(dashboardfile, "+--------------------------------------------------------------------+\n");

    
    
    // printing and storing retreival values from API
    
    printf("\nDay Period: %s\n", timeOfDay);
    
    printToTerminalAndFile(dashboardfile, "Temperature: %.2f°C\n", weather_data.temperature - 273.15);

    printToTerminalAndFile(dashboardfile, "Weather: %s\n", weather_data.description);

    printToTerminalAndFile(dashboardfile, "Humidity: %d%%\n", weather_data.humidity);

    printToTerminalAndFile(dashboardfile, "Wind Speed: %.2f m/s\n", weather_data.wind_speed);

    printToTerminalAndFile(dashboardfile, "Wind Direction: %.2f° %s\n", weather_data.wind_direction, getWindDirection(weather_data.wind_direction));

    printToTerminalAndFile(dashboardfile, "Atmospheric Pressure: %.2f hPa\n", weather_data.atmospheric_pressure);
    
    
    
    // for printing realtime visibilty value
    double visibility_km = weather_data.visibility / 1000.0; // Convert visibility to kilometers
    
    if (visibility_km >= 60.0) {
        printToTerminalAndFile(dashboardfile, "Visibility: Excellent (%.2f km)\n", visibility_km);
    }
    
    else if (visibility_km >= 10.0 && visibility_km <= 50.0) {
            printToTerminalAndFile(dashboardfile, "Visibility: Clear (%.2f km)\n", visibility_km);
    }
    
    else {
        printToTerminalAndFile(dashboardfile, "Visibility: Poor (%.2f km)\n", visibility_km);
    }
    
    
    
    // pretty printing for goodbye
    printf("\n\nThank you for using our Environemtal monitoring Application! \n");
     
    printToTerminalAndFile(dashboardfile, "------------------------- *********** ------------------------\n");

    printToTerminalAndFile(dashboardfile, "\n\n");
    
    
    // close the file
    fclose(dashboardfile);
    
   
   
      
   // Alert file for mail attachment
   // file is in append mode to save all previous data with current one's
   
   
   
   
    // Alert file for mail attachment
   // file is in append mode to save all previous data with current one's
   
   
   
   
   FILE *info_mailfile = fopen("info_condition.txt", "w");

    if (info_mailfile == NULL) {
       fprintf(stderr, "Failed to open info mail file.\n");
       return EXIT_FAILURE;
    }



    // pretty printing for alert file
    fprintf(info_mailfile, "********************************************************************\n");
    fprintf(info_mailfile, "*                INFORMATION ABOUT LATEST CONDITION                *\n");
    fprintf(info_mailfile, "********************************************************************\n");
    fflush(info_mailfile);



   
    // CONDITIONS FOR MAIL   
    // condition: If temperature is greater than 20, send an email with attachment
    // attachment is "alert_log.txt"
    
    if (weather_data.temperature - 273.15 > 20) {
         fprintf(info_mailfile, "\nBE CAUTIOUS AND STAY SAFE!\n");
         fprintf(info_mailfile, "High temperature detected! Temperature is %.2f°C\n", weather_data.temperature - 273.15);
         fflush(info_mailfile);
    }
   
   
    if (weather_data.humidity > 20) {
         fprintf(info_mailfile, "\nBE CAUTIOUS AND STAY SAFE!\n");
         fprintf(info_mailfile, "High humidity detected! Humidity is %d%%\n", weather_data.humidity);
         fflush(info_mailfile);

     }    
   
   
   

   
    FILE *alertlogfile = fopen("alert_log.txt", "a");

    if (alertlogfile == NULL) {
       fprintf(stderr, "Failed to open alert log file.\n");
       return EXIT_FAILURE;
    }



    // pretty printing for alert file
    fprintf(alertlogfile, "********************************************\n");
    fprintf(alertlogfile, "*              AlertLogs                   *\n");
    fprintf(alertlogfile, "********************************************\n");
    fflush(alertlogfile);



   
    // CONDITIONS FOR MAIL   
    // condition: If temperature is greater than 20, send an email with attachment
    // attachment is "alert_log.txt"
    
    if (weather_data.temperature - 273.15 > 20) {
         fprintf(alertlogfile, "\nAlert generated on: %s\n", date_time);
         fprintf(alertlogfile, "High temperature detected! Temperature is %.2f°C\n", weather_data.temperature - 273.15);
         fflush(alertlogfile);


         // temperature mail body message
         const char *body = "Dear Citizen,\n\nWe hope this message finds you well. Our environmental monitoring system has detected a significant change in temperature that requires your attention.\nFor your well-being, it is advised to stay hydrated and consider staying indoors to stay cool and safe.\nIn case of any emergency, please contact our Emergency Hotline: 123-456-7890.\n\nSincerly,\nMeteorological Department Of Pakistan";
         
         
         // main mail function which is defined in realtimedata.c file
        sendEmailAlert("anoosharaza95@gmail.com", "Urgent: Temperature Alert - Stay Informed and Stay Safe!", body, "info_condition.txt");
        printf("Email sent successfully!\n");
     }




     // condition: If humidity is greater than 20, send another email with attachment
     // attachment is "alert_log.txt"
     if (weather_data.humidity > 20) {
         fprintf(alertlogfile, "\nAlert generated on: %s\n", date_time);
         fprintf(alertlogfile, "High humidity detected! Humidity is %d%%\n", weather_data.humidity);
         fflush(alertlogfile);

         
         // humidity mail body message
         const char *body = "Dear Citizen,\n\nWe hope this message finds you well. Our environmental monitoring system has detected high humidity levels that require your attention.\nIn case of any discomfort or health concerns related to humidity, please take necessary precautions.\nFor your well-being, it is advised not to stay outside for extended periods during high humidity conditions.\n\nSincerely,\nMeteorological Department of Pakistan";
         
         
         // main mail function which is defined in realtimedata.c file
         sendEmailAlert("anoosharaza95@gmail.com", "Urgent: Humidity Alert - Stay Informed and Stay Safe!", body, "info_condition.txt");
         printf("Email sent successfully!\n");
      }






   // Close the file
   fclose(alertlogfile);
   fclose(info_mailfile);
   
   //Calling function for Report Generation
   GeneratingReport("raw_data.txt", "environmental_report.txt");


    return 0;

}

