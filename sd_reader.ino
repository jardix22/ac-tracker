#include <SD.h>
#include "EmonLib.h"

EnergyMonitor energyMonitor;
File file;

const float VREF = 5.0;
const int ADC_RESOLUTION = 1023;
// const float SENSOR_RATIO = 30.0;  // 30A por cada 1V RMS

void setup() {
  Serial.begin(115200);
  // constante de calibración: Es  el valor que se desea leer cuando hay 1V a la salida del sensor
  // energyMonitor.current(0, 25.65);  // current (pin de entrada, constante de calibración)

  if (!SD.begin(4)) {
    Serial.println("SD: No se pudo inicializar");
    return;
  }
}

void loop() {
  double signal = getSignal();
  // double meanSignal = getMeanSignal(10);
  double scaledSignal = getScaledSignal();
  // double signalCurrent = getSignalCurrent();
  // double current = getCurrent();
  // double iRms = energyMonitor.calcIrms(1480);  // Calculate Irms only
  // double iRms = energyMonitor.calcIrms(2240);  // Calculate Irms only

  // Open file
  file = SD.open("archivo.txt", FILE_WRITE);

  if (file) {
    file.print(millis());
    file.print(",");
    file.print(signal);
    file.print(",");
    file.println(scaledSignal);

    file.close();

    Serial.print("Signal:");
    Serial.print(signal, 3);

    // Serial.print(", meanSignal:");
    // Serial.print(meanSignal, 3);

    Serial.print(", scaledSignal:");
    // Serial.print(scaledSignal, 3);
    Serial.println(scaledSignal, 3);

    // Serial.print(", signalCurrent:");
    // Serial.print(signalCurrent, 3);

    // Serial.print(", current:");
    // Serial.print(current, 3);

    // Serial.print(", iRms:");
    // Serial.println(iRms, 3);
  }
}

double getMeanSignal(int samples) {
  long sum = 0;

  for (int i = 0; i < samples; i++) {
    sum = sum + analogRead(0);
  }

  return sum / samples;
}

double getSignal() {
  long analogSignal = analogRead(0);
  // double signal = analogSignal * (VREF / ADC_RESOLUTION) - 2.5 + 0.046;
  double signal = analogSignal * (VREF / ADC_RESOLUTION) - 2.5;

  return signal;
}

double getSignalCurrent() {
  double current = 0;
  double sum = 0;
  double iRms = 0;
  long time = millis();
  long N = 0;

  while (millis() - time < 500) {  // 500ms -> 0.5s (30 ciclos 60Hz)
    current = getSignal();
    sum = sum + sq(current);
    N = N + 1;
    // delay(1);  // 1 second of delay
  }

  iRms = sqrt(sum / N);

  return iRms;
}

double getScaledSignal() {
  double signal = getSignal();
  double scaleFactor = 3.87 / 0.143;

  return signal * scaleFactor;
}

double getCurrent() {
  double current = 0;
  double sum = 0;
  double iRms = 0;
  long time = millis();
  long N = 0;

  while (millis() - time < 500) {  // 500ms -> 0.5s (30 ciclos 60Hz)
    current = getScaledSignal();
    sum = sum + sq(current);
    N = N + 1;
    // delay(1);  // 1 second of delay
  }

  iRms = sqrt(sum / N);

  return iRms;
}
