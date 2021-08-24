/***************************************************
* Liquid Level Sensor-FS-IR02
* ****************************************************
* This example is to get liquid level

* @author jackli(Jack.li@dfrobot.com)
* @version  V1.0
* @date  2016-1-30

* GNU Lesser General Public License.
* See <http://www.gnu.org/licenses/> for details.
* All above must be included in any redistribution
* ****************************************************/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

#include <Wire.h>
#include "Adafruit_MCP9808.h"

TFT_eSPI tft = TFT_eSPI(135, 240);
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
const int32_t mid_x = 120;
const int32_t mid_y = 67;

void setup()
{
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);

  if (TFT_BL > 0)
  {                                         // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
  }

  tft.fillScreen(TFT_BLACK);

  Wire.begin(21, 22);
  if (!tempsensor.begin(0x18))
  {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    while (1)
      ;
  }

  Serial.println("Found MCP9808!");

  tempsensor.setResolution(3);
}

void updateValues(float temp, float humidity, int co2)
{
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(BC_DATUM);
  int co2Font = 8;
  if (co2 > 9999)
  {
    co2Font = 7;
  }
  tft.drawNumber(co2, mid_x, mid_y + 10, co2Font);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("ppm CO2", mid_x, mid_y + 10, 2);

  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setTextDatum(BC_DATUM);
  char buf[10];
  sprintf(buf, "%2.1f C", temp);
  tft.drawString(buf, mid_x/2, 135, 4);
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "%2.0f %%", humidity);
  tft.drawString(buf, mid_x*3/2, 135, 4);
}

void loop()
{
  Serial.println("wake up MCP9808.... "); // wake up MCP9808 - power consumption ~200 mikro Ampere
  tempsensor.wake();                      // wake up, ready to read!

  // Read and print out the temperature, also shows the resolution mode used for reading.
  Serial.print("Resolution in mode: ");
  Serial.println(tempsensor.getResolution());
  float temp = tempsensor.readTempC();
  Serial.print("Temp: ");
  Serial.print(temp, 4);
  Serial.println("*C");
  updateValues(temp, 42.0, 250);

  delay(2000);
  Serial.println("Shutdown MCP9808.... ");
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
  Serial.println("");
  delay(200);
}
