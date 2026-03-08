#include <M5StickCPlus.h>

float accX, accY, accZ;
bool recording = false;
float maxG, minG, sumG;
int sampleCount;
unsigned long startTime, recordDuration;
float lastTotal = -1;
int lastSampleCount = -1;
unsigned long lastElapsed = -1;
bool justStopped = false;

void drawResults() {
  M5.Lcd.fillScreen(BLACK);
  unsigned long secs = recordDuration / 1000;
  unsigned long ms = recordDuration % 1000;
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("Time: %lus %lums\n", secs, ms);
  M5.Lcd.printf("Avg: %.2f g\n", sumG / sampleCount);
  M5.Lcd.printf("Max: %.2f g\n", maxG);
  M5.Lcd.printf("Min: %.2f g\n", minG);
}

void setup() {
  M5.begin();
  M5.Imu.Init();
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("Press button to\nstart recording\n");
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    if (!recording) {
      recording = true;
      maxG = -9999;
      minG = 9999;
      sumG = 0;
      sampleCount = 0;
      startTime = millis();
      lastTotal = -1;
      lastSampleCount = -1;
      lastElapsed = -1;
      M5.Lcd.fillScreen(BLACK);
    } else {
      recording = false;
      recordDuration = millis() - startTime;
      justStopped = true;
    }
  }

  M5.Imu.getAccelData(&accX, &accY, &accZ);
  float total = sqrt(accX * accX + accY * accY + accZ * accZ) - 1.0;
  if (total < 0) total = 0;

  if (recording) {
    if (total > maxG) maxG = total;
    if (total < minG) minG = total;
    sumG += total;
    sampleCount++;

    unsigned long elapsed = (millis() - startTime) / 1000;
    bool changed = false;

    if (elapsed != lastElapsed) { lastElapsed = elapsed; changed = true; }
    if (abs(total - lastTotal) > 0.01) { lastTotal = total; changed = true; }
    if (sampleCount != lastSampleCount) { lastSampleCount = sampleCount; changed = true; }

    if (changed) {
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.printf("REC  %02lus\n", elapsed);
      M5.Lcd.printf("Total: %.2f g\n", total);
      M5.Lcd.printf("Samples: %d\n", sampleCount);
    }
  } else if (justStopped) {
    drawResults();
    justStopped = false;
  }

  delay(100);
}