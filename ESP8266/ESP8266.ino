// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include <ESP8266WiFi.h>
#include "src/iotc/common/string_buffer.h"
#include "src/iotc/iotc.h"

#define WIFI_SSID "Casa de gatos"
#define WIFI_PASSWORD "0512AnacletO"

const char* SCOPE_ID = "0ne001EA030";
const char* DEVICE_ID = "18vrw79vcwh";
const char* DEVICE_KEY = "RY9LPFIImnKjW/POn2aJZgotDYYGn35B72e97fprXFo=/FNKWjOoT7RveToE8/MCgjN5bZjMZLPCUrll9QeSieIs=";

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo);
#include "src/connection.h"

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo) {
  // ConnectionStatus
  if (strcmp(callbackInfo->eventName, "ConnectionStatus") == 0) {
    LOG_VERBOSE("Is connected ? %s (%d)",
                callbackInfo->statusCode == IOTC_CONNECTION_OK ? "YES" : "NO",
                callbackInfo->statusCode);
    isConnected = callbackInfo->statusCode == IOTC_CONNECTION_OK;
    return;
  }

  // payload buffer doesn't have a null ending.
  // add null ending in another buffer before print
  AzureIOT::StringBuffer buffer;
  if (callbackInfo->payloadLength > 0) {
    buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
  }

  LOG_VERBOSE("- [%s] event was received. Payload => %s\n",
              callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

  if (strcmp(callbackInfo->eventName, "Command") == 0) {
    LOG_VERBOSE("- Command name was => %s\r\n", callbackInfo->tag);
  }
  
  if (strcmp(callbackInfo->eventName, "SettingsUpdated") == 0) {
    LOG_VERBOSE("- Setting name was => %s\r\n", callbackInfo->tag);
  }
  
}

void setup() {
  Serial.begin(9600);

  connect_wifi(WIFI_SSID, WIFI_PASSWORD);
  connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);

  if (context != NULL) {
    lastTick = 0;  // set timer in the past to enable first telemetry a.s.a.p
  }
}

double randomDouble(double minf, double maxf)
{
  return minf + random(1UL << 31) * (maxf - minf) / (1UL << 31);  // use 1ULL<<63 for max double values)
}

void loop() {
  if (isConnected) {
    unsigned long ms = millis();
    if (ms - lastTick > 15000) {  // send telemetry every 15 seconds
      char msg[128] = {0};
      int pos = 0, errorCode = 0;

      lastTick = ms;

      if (loopId++ % 2 == 0) {  // send telemetry
        double tem = randomDouble(21.00,25.99);
        Serial.println(tem);
        double hum = randomDouble(80.00,99.99);
        Serial.println(hum);
        
        pos = snprintf(msg, sizeof(msg) - 1, "{\"Temp\": %d, \"Humidity\":%d}", 20, 80); 
        errorCode = iotc_send_telemetry(context, msg, pos);
        
      } else {  // send property
        double lat = randomDouble(-21.99,-21.00);
        Serial.println(lat);
        double lon = randomDouble(-69.99,-69.00);
        Serial.println(lon);
        
        pos = snprintf(msg, sizeof(msg) - 1, "{\"Location\": {\"lat\": %d,\"lon\": %d,\"alt\": 0}}", -20, -69); 
        errorCode = iotc_send_telemetry(context, msg, pos);
      }
      
      msg[pos] = 0;

      if (errorCode != 0) {
        LOG_ERROR("Sending message has failed with error code %d", errorCode);
      }
    }

    iotc_do_work(context);  // do background work for iotc
  } else {
    iotc_free_context(context);
    context = NULL;
    connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);
  }
}
