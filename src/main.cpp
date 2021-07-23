#include <Arduino.h>
#include <Tinker_DEBUG.h>
extern char DEBUGtxt[48];

#include "config.h"
#include "TopSecret.h"

#include <Tinker_WiFi.h>
#include <Tinker_HTTP.h>
#include <Tinker_MQTT.h>

#include <Tinker_UT61EWIFI.h>
#include <HTML/Parts/Content_Squirrel.htm>

/* OTA Stuff */
bool restartRequired = false;
int deviceREBOOTED = true;
void OTA_Restart();

void setup()
{
  setup_DEBUG();
  DEBUG_Title();
  DEBUG_Reset();

  setup_RGB_StatusLED();
  RGB_StatusLED(100, 0, 0); // Red

  setup_WiFi();

  sprintf(DEBUGtxt, "WiFi Strength: %d dBm", WiFi_strength());
  DEBUG_LineOut(DEBUGtxt);

  RGB_StatusLED(100, 100, 0); // Yellow(ish)

  setup_HTTP();

  RGB_StatusLED(100, 0, 100); // Pink(ish)

  MQTT_init();

  RGB_StatusLED(0, 0, 100); // Blue

  setup_UT61EWIFI();

  DEBUG_Separator();
  sprintf(DEBUGtxt, "Sketch Size: %s", info_memsketch().c_str());
  DEBUG_LineOut(DEBUGtxt);

  DEBUG_Ready();
}

void loop()
{
  if (restartRequired)
    OTA_Restart();

  if (!WiFi_Test())
  {
    Serial.println("Oh Poop! WiFi lost...");
    delay(2000);
  }

  MQTT_handler();

  UT61EWIFI_loop();

}

void OTA_Restart()
{
  yield();
  delay(1000);
  yield();
  DEBUG_SectionTitle("Rebooting!");
  ESP.restart();
}

/*********************************************/
/* Development copy of MQTT_HandleMessages() */
/*********************************************/
#if defined(TestCode)
void MQTT_HandleMessages(const char *Topic, const char *Message)
{
  char debugTEXT[46];

  // sprintf(debugTEXT, "  Topic: %s", Topic);
  // DEBUG_Success(debugTEXT);
  // sprintf(debugTEXT, "Message: %s", Message);
  // DEBUG_Success(debugTEXT);

  if (strcmp(Topic, "/Power") == 0)
  {
    // MQTT_SendTELE(Topic, Topic);
    MQTT_SendNOTI("triggered", "Power!!!");
    DEBUG_LineOut("Power Triggered");
  }
  else if (strcmp(Topic, "/Button") == 0)
  {
    // MQTT_SendTELE(Topic, Topic);
    MQTT_SendNOTI("triggered", "Button!!!");
    sprintf(debugTEXT, "Button %s", Message);
    DEBUG_Success(debugTEXT);
  }
  else if (strcmp(Topic, "/Status") == 0)
  {
    MQTT_SendSTAT("triggered", "Status!!!");
    DEBUG_LineOut("Status Requested");
  }
  else
  {
    DEBUG_Trouble("Dunno Whatcha want...");
    MQTT_SendNOTI("Error", "Dunno Whatcha want...");
  }
}
#endif
