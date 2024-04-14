//Light tracker, urządzenie służące do śledzenia światła
//wykorzystywane porty analogowe od 1 do 4 oraz cyfrowe od 2 do 13
//kod powstał 13.01.2024r
//Autorzy: Wiktoria Golinowska, Jakub Wrona, Wiktor Równanek

//Zaimportowanie biblioteki do sterowania silnikami

#include <Stepper.h>

// Definicje pinów dla fotorezystorów 
int LG = A1;
int PG = A2;
int PD = A3;
int LD = A4;

// Definicje pinów  dla przycisków krańcowych
int PionPanel = 9;
int PionCzujniki = 8;
int PrawyMax = 10;
int LewyMax = 11;

// Zmienne przechowujące informacje o kierunku ruchu silników
int H, V;

// Stała definiująca ilość kroków na jeden pełny obrót silnika 
const int kroki_na_obrot = 2048;

// Obiekty dla silników krokowych
Stepper Poziom(kroki_na_obrot, 13, 2, 12, 3);
Stepper Pion(kroki_na_obrot, 4, 6, 5, 7);

// Funkcja kalibracji dla silnika pionowego
void kalibracjaPion() {
  while (digitalRead(PionCzujniki) == HIGH) {
    Pion.step(-1);
  }
}

// Funkcja kalibracji dla silnika poziomego
void kalibracjaPoziom() {
  while (digitalRead(PrawyMax) == HIGH) {
    Poziom.step(-1);
  }
   Poziom.step(400);
  
}

// Funkcja wyłączająca silnik pionowy
int wylaczPion() {
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  return 0;
}

// Funkcja wyłączająca silnik poziomy
int wylaczPoziom() {
  digitalWrite(13, LOW);
  digitalWrite(2, LOW);
  digitalWrite(12, LOW);
  digitalWrite(3, LOW);
  return 0;
}

void setup() {
  // Konfiguracja pinów jako wejścia
  pinMode(LG, INPUT);
  pinMode(PG, INPUT);
  pinMode(PD, INPUT);
  pinMode(LD, INPUT);
  pinMode(PionPanel, INPUT_PULLUP);
  pinMode(PionCzujniki, INPUT_PULLUP);
  pinMode(PrawyMax, INPUT_PULLUP);
  pinMode(LewyMax, INPUT_PULLUP);

  // Ustawienie prędkości obrotowej silników
  Poziom.setSpeed(8);
  Pion.setSpeed(8);

  // Rozpoczęcie komunikacji szeregowej
  Serial.begin(9600);

  // Wywołanie funkcji kalibracji dla silników pionowego i poziomego
  kalibracjaPion();
  kalibracjaPoziom();
}

void loop() {
  // Odczyt wartości z fotorezystorów i obliczenie średniej wartości stron
  int gora = (analogRead(LG) + analogRead(PG)) / 2;
  int dol = (analogRead(LD) + analogRead(PD)) / 2;
  int prawo = (analogRead(PD) + analogRead(PG)) / 2;
  int lewo = (analogRead(LG) + analogRead(LD)) / 2;

  // Obliczenie wartości składowych ruchu (pionowy i poziomy)
  V = gora - dol;
  H = prawo - lewo;

  // Kontrola ruchu silnika pionowego uwzględniająca przyciski krańcowe
  if (digitalRead(PionPanel) == LOW) {
    Pion.step(V > 10 ? -2 : wylaczPion());
  } else if ((digitalRead(PionCzujniki) == LOW) and (V < -100)) {
    Poziom.step(300);
  } else if (digitalRead(PionCzujniki) == LOW) {
    Pion.step(V < -10 ? 2 : wylaczPion());
  } else {
    Pion.step(V > 10 ? -1 : (V < -10 ? 1 : wylaczPion()));
  }

  // Kontrola ruchu silnika poziomego uwzględniająca przyciski krańcowe
  if ((digitalRead(PrawyMax) == LOW) and (H > 100)) {
    Poziom.step(1500);
  } else if (digitalRead(PrawyMax) == LOW) {
    Poziom.step(H > 10 ? 2 : wylaczPoziom());
  } else if ((digitalRead(LewyMax) == LOW) and (H < -100)) {
    Poziom.step(-1500);
  } else if (digitalRead(LewyMax) == LOW) {
    Poziom.step(H < -10 ? -2 : wylaczPoziom());
  } else {
    Poziom.step(H > 10 ? -2 : (H < -10 ? 2 : wylaczPoziom()));
  }
}


 