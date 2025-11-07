#include <Wire.h>
#include "SSD1306Wire.h"


SSD1306Wire display(0x3C, 21, 22);

// Variáveis para controle de tempo
unsigned long previousMillis = 0;
unsigned long interval = 1000;  // 1 segundo

int segundos = 0;
int minutos = 18;
int horas = 9;   // Ajuste manual da hora inicial
int dia = 7;     // Ajuste manual da data inicial
int mes = 11;
int ano = 2025;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando display...");

  display.init();
  display.flipScreenVertically();
  display.clear();
  display.display();

  Serial.println("Sistema iniciado.");
}

void loop() {
  unsigned long currentMillis = millis();

  // Atualiza o "relógio" a cada segundo
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    segundos++;

    if (segundos >= 60) {
      segundos = 0;
      minutos++;
      if (minutos >= 60) {
        minutos = 0;
        horas++;
        if (horas >= 24) {
          horas = 0;
          dia++;
          // Aqui você pode adicionar lógica simples de mudança de mês/dia se quiser
        }
      }
    }

    // Exibe no display
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);

    // Título
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 5, "Sistemas");
    display.drawString(64, 25, "Embarcados");

    // Data e hora
    display.setFont(ArialMT_Plain_10);
    char dataStr[20];
    char horaStr[20];
    sprintf(dataStr, "%02d/%02d/%04d", dia, mes, ano);
    sprintf(horaStr, "%02d:%02d:%02d", horas, minutos, segundos);

    display.drawString(64, 48, String(dataStr) + "  " + String(horaStr));
    display.display();

    
    Serial.println(String(dataStr) + "  " + String(horaStr));
  }
}
