/*
 REACTION TIMER - Jogo de Precisão Temporal
 Miniprojeto: Parar cronômetro exatamente em 5000ms
 Autor: Anderson de Souza Santos
 Data: 16/10/2025
 Controles: 1 clique = Inicia | 2º clique = Para | LED pisca = VITÓRIA!
 Alvo: 5000ms (5.000)
*/

#define REST 0
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_C5 523
#define NOTE_F5 698

unsigned char unidades = 0;
unsigned char dezenas = 0;
unsigned char centenas = 0;
unsigned char milhares = 0;

// Tabela para displays de 7 segmentos (catodo comum)
unsigned char tabela[10] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F  // 9
};

// Pinos e Configurações Gerais
#define BOTAO_PIN 8
#define LED_PIN 13
#define BUZZER_PIN 12

// Tempo-alvo (em ms)
const unsigned long ALVO = 5000;

// Margem de erro permitida (em ms)
const unsigned long TOLERANCIA = 50;

// Controle de estados
unsigned char estado = 0;

// Controle de tempo
unsigned long startTime = 0;
unsigned long elapsedTime = 0;

// Controle de debounce
unsigned long ultimoTempoBotao = 0;
unsigned char botaoAnterior = HIGH;

// Melodia "Parabéns pra Você"
int tempo = 140;
int melody[] = {
  NOTE_C4,4, NOTE_C4,8, 
  NOTE_D4,-4, NOTE_C4,-4, NOTE_F4,-4,
  NOTE_E4,-2, NOTE_C4,4, NOTE_C4,8, 
  NOTE_D4,-4, NOTE_C4,-4, NOTE_G4,-4,
  NOTE_F4,-2, NOTE_C4,4, NOTE_C4,8,
  NOTE_C5,-4, NOTE_A4,-4, NOTE_F4,-4, 
  NOTE_E4,-4, NOTE_D4,-4, NOTE_AS4,4, NOTE_AS4,8,
  NOTE_A4,-4, NOTE_F4,-4, NOTE_G4,-4,
  NOTE_F4,-2,
};
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

void setup() {
  DDRD = 0xFF; // Porta D = saída (segmentos)
  DDRC = 0x0F; // 4 bits inferiores da Porta C = seleção dos displays

  pinMode(BOTAO_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  resetDisplays();
}

void loop() {
  verificaBotao();

  if (estado == 1) {
    atualizaCronometro();
  }

  if (estado == 2) {
    if (verificaVitoria()) {
      sinalizaVitoria();
    } else {
      for (int i = 0; i < 300; i++) { // Mostra "FAIL" por ~1 segundo
        mostraFail();
      }
      tone(BUZZER_PIN, 200, 400); // Som grave de erro
      delay(500);
      noTone(BUZZER_PIN);
    }
    resetCronometro();
    estado = 0;
  }

  mostra();
}

// --------------------------------------------------
// Leitura e tratamento do botão (com debounce)
// --------------------------------------------------
void verificaBotao() {
  unsigned char botaoAtual = digitalRead(BOTAO_PIN);

  if (botaoAtual == LOW && botaoAnterior == HIGH && millis() - ultimoTempoBotao > 80) {
    if (estado == 0 || estado == 2) {
      // Inicia novo jogo
      startTime = millis();
      elapsedTime = 0;
      estado = 1;
      digitalWrite(LED_PIN, LOW);
      tone(BUZZER_PIN, 1000, 150); // som curto de início
    } else if (estado == 1) {
      // Para o cronômetro
      elapsedTime = millis() - startTime;
      atualizaDigitos();
      estado = 2;
    }
    ultimoTempoBotao = millis();
  }

  botaoAnterior = botaoAtual;
}

// --------------------------------------------------
// Atualização do cronômetro
// --------------------------------------------------
void atualizaCronometro() {
  elapsedTime = millis() - startTime;
  if (elapsedTime > 9999) elapsedTime = 9999;
  atualizaDigitos();
}

// --------------------------------------------------
// Atualização dos dígitos para exibir no display
// --------------------------------------------------
void atualizaDigitos() {
  milhares = (elapsedTime / 1000) % 10;
  centenas = (elapsedTime / 100) % 10;
  dezenas = (elapsedTime / 10) % 10;
  unidades = elapsedTime % 10;
}

// --------------------------------------------------
// Verifica se o tempo atingido está dentro da margem
// --------------------------------------------------
bool verificaVitoria() {
  long diferenca = (long)elapsedTime - (long)ALVO;
  if (diferenca < 0) diferenca = -diferenca;
  return (diferenca <= TOLERANCIA);
}

// --------------------------------------------------
// Efeito sonoro e visual de vitória (Parabéns pra Você)
// --------------------------------------------------
void sinalizaVitoria() {
  for (int thisNote = 0; thisNote < notes * 2; thisNote += 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }

    tone(BUZZER_PIN, melody[thisNote], noteDuration * 0.9);
    digitalWrite(LED_PIN, HIGH);
    delay(noteDuration / 2);
    digitalWrite(LED_PIN, LOW);
    delay(noteDuration / 2);
    noTone(BUZZER_PIN);
  }
}

// --------------------------------------------------
// Mostra "FAIL" no display em caso de erro
// --------------------------------------------------
void mostraFail() {
  // Letras F, A, I, L (catodo comum)
  unsigned char F = 0x71;
  unsigned char A = 0x77;
  unsigned char I = 0x06;
  unsigned char L = 0x38;

  PORTD = L;   // Unidades
  PORTC = 0x0E;
  delay(3);
  PORTC = 0x0F;

  PORTD = I;   // Dezenas
  PORTC = 0x0D;
  delay(3);
  PORTC = 0x0F;

  PORTD = A;   // Centenas
  PORTC = 0x0B;
  delay(3);
  PORTC = 0x0F;

  PORTD = F;   // Milhares
  PORTC = 0x07;
  delay(3);
  PORTC = 0x0F;
}

// --------------------------------------------------
// Reset do cronômetro e displays
// --------------------------------------------------
void resetCronometro() {
  unidades = dezenas = centenas = milhares = 0;
  elapsedTime = 0;
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
}

// --------------------------------------------------
// Funções de controle de displays
// --------------------------------------------------
void resetDisplays() {
  PORTD = 0x00;
  PORTC = 0x0F;
  delay(5);
}

void mostra() {
  PORTD = tabela[unidades];
  PORTC = 0x0E;
  delay(3);
  PORTC = 0x0F;

  PORTD = tabela[dezenas];
  PORTC = 0x0D;
  delay(3);
  PORTC = 0x0F;

  PORTD = tabela[centenas];
  PORTC = 0x0B;
  delay(3);
  PORTC = 0x0F;

  PORTD = tabela[milhares];
  PORTC = 0x07;
  delay(3);
  PORTC = 0x0F;
}
