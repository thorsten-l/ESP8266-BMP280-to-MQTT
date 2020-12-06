#include <App.hpp>
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <OtaHandler.hpp>
#include <Util.hpp>
#include <WifiHandler.hpp>
#include <MqttHandler.hpp>
#include <TelnetStream.h>

#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

ADC_MODE(ADC_VCC);

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP280 bmp;
float temperature_C;
float pressure_hPa;
float altitude;

void readSensor()
{
  temperature_C = bmp.readTemperature();
  pressure_hPa = bmp.readPressure() / 100.0f;
  altitude = bmp.readAltitude();

  sprintf(buffer,
          "{"
          "\"temperature_C\":%.2f,"
          "\"pressure_hPa\":%.3f,"
          "\"altitude\":%.3f"
          "}",
          temperature_C, pressure_hPa, altitude);
}

void setup()
{
  app.setup();

  unsigned status;

  logMessage("I2C SCL=%d", SCL);
  logMessage("    SDA=%d", SDA);

  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);       // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0) // Receive 0 = success (ACK response)
    {
      logMessage("Found address: %d (0x%02X)", i, i);
      count++;
    }
  }
  logMessage("Found %d device(s).", count);
  logMessage("I2C sensor address setting : %d (0x%02X)",
             appcfg.i2c_sensor_address, appcfg.i2c_sensor_address);
  status = bmp.begin(appcfg.i2c_sensor_address);
  if (!status)
  {
    logMessage("Could not find a valid BMP280 sensor, check wiring, "
               "address, sensor ID!");
  }

  LOG0("initialized\n");
}

void loop()
{
  static time_t lastMqttMessageTimestamp = 0;

  app.loop();

  if (otaHandler.inProgress == false)
  {

    if (appcfg.mqtt_sending_interval > 0 && ((millis() - lastMqttMessageTimestamp >= (appcfg.mqtt_sending_interval * 1000))))
    {
      static char logbuffer[MAX_MESSAGE_LENGTH + 1];
      readSensor();

      if (appcfg.mqtt_enabled == true)
      {
        mqttHandler.sendValue(appcfg.mqtt_topic_json, buffer);
      }
      lastMqttMessageTimestamp = millis();

      sprintf(logbuffer,
              "temperature_C:%.2f, "
              "pressure_hPa:%.3f, "
              "altitude:%.3f",
              temperature_C, pressure_hPa, altitude);

      logMessage(logbuffer);
    }

    mqttHandler.handle(millis());
  }

  delay(20);
}
