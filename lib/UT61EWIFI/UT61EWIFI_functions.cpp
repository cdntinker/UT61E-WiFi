#include <Tinker_DEBUG.h>
extern char DEBUGtxt[];

#include <Tinker_UT61EWIFI.h>

// General
uint32_t g_device_id; // Unique ID from ESP chip ID

///////////////////////////////////////////////////////////////////////////////////////////

// MQTT
char g_mqtt_message_buffer[150]; // General purpose buffer for MQTT messages
uint8_t g_buffer_position = 0;
char g_command_topic[50];   // MQTT topic for receiving commands
char g_mqtt_raw_topic[50];  // MQTT topic for reporting the raw data packet
char g_mqtt_json_topic[50]; // MQTT topic for reporting the decoded reading

///////////////////////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h> // Must be the EspSoftwareSerial library

/* ----------------- Hardware-specific config ---------------------- */
/* Multimeter interface */
#define UT61E_RX_PIN D5       // Rx from UT61e (== UT61e Tx)
#define UT61E_BAUD_RATE 19200 // PMS5003 uses 9600bps

SoftwareSerial ut61e(UT61E_RX_PIN, -1); // RX, TX

void setup_UT61EWIFI()
{

  sprintf(DEBUGtxt, "UT61e multimeter WiFi/USB interface v0.0");
  DEBUG_Init(DEBUGtxt);
  sprintf(DEBUGtxt, "RX pin: GPIO%d, @ %d baud", UT61E_RX_PIN, UT61E_BAUD_RATE);
  DEBUG_Init(DEBUGtxt);

  // Open a connection to the PMS and put it into passive mode
  ut61e.begin(UT61E_BAUD_RATE, SWSERIAL_7O1, UT61E_RX_PIN, -1); // Connection for multimeter

  // We need a unique device ID for our MQTT client connection
  g_device_id = ESP.getChipId(); // Get the unique ID of the ESP8266 chip
  Serial.print("Device ID: ");
  Serial.println(g_device_id, HEX);

  // Set up the topics for publishing sensor readings. By inserting the unique ID,
  // the result is of the form: "device/d9616f/PM1P0" etc
  sprintf(g_command_topic, "cmnd/%X/COMMAND", g_device_id); // For receiving commands
  sprintf(g_mqtt_raw_topic, "tele/%X/RAW", g_device_id);    // Data from multimeter
  sprintf(g_mqtt_json_topic, "tele/%X/JSON", g_device_id);  // Data from multimeter

  // Report the MQTT topics to the serial console
  DEBUG_SectionTitle("MQTT Topics");
  sprintf(DEBUGtxt, " Command: %s", g_command_topic);
  DEBUG_LineOut(DEBUGtxt);
  sprintf(DEBUGtxt, " Raw Out: %s", g_mqtt_raw_topic);
  DEBUG_LineOut(DEBUGtxt);
  sprintf(DEBUGtxt, "JSON Out: %s", g_mqtt_json_topic);
  DEBUG_LineOut(DEBUGtxt);
}

void UT61EWIFI_loop()
{
  /* Report value */
  String message_string;
  if (ut61e.available())
  {
    byte this_character = ut61e.read();
    //Serial.write(this_character);
    if (10 == this_character || 13 == this_character)
    {
      if (10 == this_character)
      {
        RGB_StatusLED(0, 255, 0); // Green
        g_mqtt_message_buffer[g_buffer_position] = 0;
        g_buffer_position = 0;
        ///////////////////////////////////////////////////////////
        // client.publish(g_mqtt_raw_topic, g_mqtt_message_buffer);
        ///////////////////////////////////////////////////////////
        DEBUG_LineOut(g_mqtt_message_buffer);
        RGB_StatusLED(0, 0, 0); // off
      }
    }
    else
    {
      g_mqtt_message_buffer[g_buffer_position] = this_character;
      g_buffer_position++;
    }
  }
}
