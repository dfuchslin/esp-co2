#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

#include <Wire.h>
#include <Adafruit_SCD30.h>

const int32_t SCREEN_WIDTH = 240;
const int32_t SCREEN_HEIGHT = 135;
const int32_t mid_x = 120;
const int32_t mid_y = 67;
TFT_eSPI tft = TFT_eSPI(SCREEN_HEIGHT, SCREEN_WIDTH);
TFT_eSprite spr = TFT_eSprite(&tft);
Adafruit_SCD30 scd30;

void showError(String error)
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(error, mid_x, mid_y, 4);
}

void setup()
{
  Serial.begin(115200);

  tft.init();
  tft.setRotation(3);
  spr.setColorDepth(16);
  spr.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
  spr.fillSprite(TFT_BLACK);

  if (TFT_BL > 0)
  {                                         // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
  }

  tft.fillScreen(TFT_BLACK);

  Wire.begin(21, 22);
  if (!scd30.begin())
  {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    showError("No sensor!");
    while (1)
      ;
  }

  Serial.println("Found MCP9808!");

  Serial.print("Measurement Interval: ");
  Serial.print(scd30.getMeasurementInterval());
  Serial.println(" seconds");
}

void updateValues(float temp, float humidity, int co2)
{
  tft.fillScreen(TFT_BLACK);

  /*


    < 1000 = Good air. Your body will be happy!
    1000 - 2000 = Poor air. See if there is any way to improve.
    2000 - 5000 =  Warning levels. Good idea to investigate why.
    > 5000 = Dang, you are above the OSHA Permissible Exposure Limit for 8 hour exposure. Something should be done.


  */

  uint8_t co2Font = 8;
  if (co2 > 9999)
  {
    co2Font = 7;
  }
  uint16_t co2Color = TFT_DARKGREEN;
  uint16_t co2ColorBackground = TFT_BLACK;
  if (co2 > 1000)
  {
    co2Color = TFT_ORANGE;
  }
  if (co2 > 2000)
  {
    co2Color = TFT_RED;
  }
  if (co2 > 5000)
  {
    co2Color = TFT_WHITE;
    co2ColorBackground = TFT_RED;
  }
  spr.fillSprite(co2ColorBackground);
  spr.setTextColor(co2Color);
  spr.setTextDatum(BC_DATUM);
  spr.drawNumber(co2, mid_x, mid_y + 10, co2Font);
  spr.setTextDatum(TC_DATUM);
  spr.drawString("ppm CO2", mid_x, mid_y + 10, 2);

  spr.setTextColor(TFT_LIGHTGREY);
  spr.setTextDatum(BC_DATUM);
  char buf[10];
  sprintf(buf, "%2.1f C", temp);
  spr.drawString(buf, mid_x / 2, 135, 4);
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "%2.0f %%", humidity);
  spr.drawString(buf, mid_x * 3 / 2, 135, 4);

  spr.pushSprite(0, 0);
}

void loop()
{
  if (scd30.dataReady())
  {
    if (!scd30.read())
    {
      Serial.println("Error reading sensor data");
      return;
    }
    updateValues(scd30.temperature, scd30.relative_humidity, scd30.CO2);
  }
  delay(1000);
}
