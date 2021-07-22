#include <Arduino.h>
#include <Tinker_DEBUG.h>
extern char DEBUGtxt[48];

#include "config.h"
#include "TopSecret.h"

#include <Tinker_WiFi.h>
#include <Tinker_HTTP.h>

#if defined(SiniLink)
#include <Tinker_SiniLink.h>
#else
const char *Content_SiniLink PROGMEM = "";
#endif

#if defined(TFT_enabled)
#include "Tinker_TFT.h"
#include <SPI.h>
#endif

#if defined(MQTT_enabled)
#include <Tinker_MQTT.h>
#endif

#if defined(RGB_enabled)
#include <Tinker_RGB.h>
int RGBping_CTR;
#endif

#if defined(UT61EWIFI)
#include <Tinker_UT61EWIFI.h>
#include <HTML/Parts/Content_Squirrel.htm>
#else
const char *Content_Squirrel PROGMEM = "";
#endif

/* OTA Stuff */
bool restartRequired = false;
int deviceREBOOTED = true;
void OTA_Restart();

void setup()
{
  setup_DEBUG();
  DEBUG_Title();
  DEBUG_Reset();

#if defined(UT61EWIFI)
  setup_RGB_StatusLED();
  RGB_StatusLED(100, 0, 0); // Red
#endif

  setup_WiFi();

  sprintf(DEBUGtxt, "WiFi Strength: %d dBm", WiFi_strength());
  DEBUG_LineOut(DEBUGtxt);

#if defined(UT61EWIFI)
  RGB_StatusLED(100, 100, 0); // Yellow(ish)
#endif

  setup_HTTP();

#if defined(UT61EWIFI)
  RGB_StatusLED(100, 0, 100); // Pink(ish)
#endif

#if defined(TFT_enabled)
  TFT_Setup();
#endif

#if defined(SiniLink)
  SiniLink_init();
#endif

#if defined(MQTT_enabled)
  MQTT_init();
#endif
  RGB_StatusLED(0, 0, 100); // Blue

#if defined(RGB_enabled)
  setup_RGBping();
#endif

#if defined(UT61EWIFI)
  setup_UT61EWIFI();
#endif

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

#if defined(MQTT_enabled)
  MQTT_handler();
#endif

#if defined(TFT)
  TFT_loop();
#endif

#if defined(RGB_enabled)
  loop_RGBping(); // borks HTTP stuff...  :(
#endif

#if defined(UT61EWIFI)
  UT61EWIFI_loop();
#endif

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
