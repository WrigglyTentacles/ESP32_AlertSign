#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <cJSON.h>

#include "login_page.h"
#include "update_page.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* host = HOST_NAME;

//variabls to blink without delay:
const int led = 2;
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 1000;           // interval at which to blink (milliseconds)
int ledState = LOW;             // ledState used to set the LED

WebServer server(80);

QueueHandle_t txqueue;
int status = 0;

int enumerate_status(String statusCode){
  if(statusCode == "none")         return 0;
  else if(statusCode == "urgent")  return 1;
  else if(statusCode == "alert")   return 2;
  else if(statusCode == "good")    return 3;
  else if(statusCode == "vibe")    return 4;
  return 0;
}

/*
 * setup function
 */
void ota_setup(QueueHandle_t signal_queue) {
  txqueue=signal_queue;
  pinMode(led, OUTPUT);
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  // upload page
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  //status
  server.on("/status", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    status = (status+1)%3;
    int txBuffer[1];
    txBuffer[0]=status;
    xQueueSend(txqueue,(void*)txBuffer,(TickType_t)0);
    server.send(200, "application/javascript", "{success:'True',value:'"+String(status)+"'}");
    //vTaskDelay(1000/ portTICK_RATE_MS);
  });
  //set status from args
  server.on("/set", HTTP_GET, []() {
    String status_arg = "none";
    int statusCode = -1;
    int txBuffer[1];// ipc buffer

    server.sendHeader("Connection", "close"); // Close the clients waiting connection
    (server.hasArg("status"))?status_arg=server.arg("status"):status_arg="none"; //Pull the status from args if it exists
    statusCode = enumerate_status(status_arg);// Get us an integer code from the string pulled in

    txBuffer[0]=statusCode;//fill buffer with status code
    xQueueSend(txqueue,(void*)txBuffer,(TickType_t)0);//Shove the code into the shared queue for the led core to pick up
    server.send(200, "application/javascript", "{success:'True',value:'"+String(status_arg)+"',code: '"+statusCode+"'}");//let the client know what's up
    //vTaskDelay(500/ portTICK_RATE_MS);//don't let us take another request for a bit
  });
  //Set Status using post
  server.on("/set", HTTP_POST, []() {
    String status_arg = "none";
    int statusCode = -1;
    int txBuffer[1];// ipc buffer

    server.sendHeader("Connection", "close"); // Close the clients waiting connection
    (server.hasArg("status"))?status_arg=server.arg("status"):status_arg="none"; //Pull the status from args if it exists
    statusCode = enumerate_status(status_arg);// Get us an integer code from the string pulled in

    txBuffer[0]=statusCode;//fill buffer with status code
    xQueueSend(txqueue,(void*)txBuffer,(TickType_t)0);//Shove the code into the shared queue for the led core to pick up
    server.send(200, "application/javascript", "{success:'True',value:'"+String(status_arg)+"',code: '"+statusCode+"'}");//let the client know what's up
    //vTaskDelay(500/ portTICK_RATE_MS);//don't let us take another request for a bit
  });
  //Set Status using post
  server.on("/kuma", HTTP_POST, []() {
    String status_arg = "none";
    int statusCode = -1;
    int txBuffer[1];// ipc buffer

    server.sendHeader("Connection", "close"); // Flag to close the clients waiting connection
    (server.hasArg("status"))?status_arg=server.arg("status"):status_arg="none"; //Pull the status from args if it exists
    int statuslen = status_arg.length();
    if(status_arg != "none"){
      if((statuslen >= 28) && status_arg.substring(26,28)== "Up") status_arg="none";
      else if((statuslen >= 30) && status_arg.substring(26, 30) == "Down")status_arg="urgent";
    }
    statusCode = enumerate_status(status_arg);// Get us an integer code from the string pulled in
    txBuffer[0]=statusCode;//fill buffer with status code
    xQueueSend(txqueue,(void*)txBuffer,(TickType_t)0);//Shove the code into the shared queue for the led core to pick up
    server.send(200, "application/javascript", "{success:'True',value:'"+String(status_arg)+"',code: '"+statusCode+"'}");//let the client know what's up
  });

  //Check API Status using post
  server.on("/API/echo", HTTP_POST, []() {
    String returnVal;
    (server.hasArg("status"))?returnVal=server.arg("status"):returnVal="none"; //Pull the status from args if it exists
    server.send(200, "application/javascript", "{'raw': '"+returnVal+"'}");//let the client know what's up
  });

  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void ota_loop(void * pvParameters) {
  for(;;){
    server.handleClient();
    delay(1);
  }
} 