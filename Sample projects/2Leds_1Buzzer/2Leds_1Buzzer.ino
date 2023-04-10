/// ---------------------------------------------------------------------
///
/// Projet          : 2Leds_1Buzzer
/// Auteur          : Juanito del Pepito
/// Version         : 0.0.0.1
/// Date            : 10/04/2023
/// Description     : Projet témoin
///                     - Tuto montrant l'utilisation des objets JUANITO_LED, JUANITO_PASSIVE_BUZZER
///                     - Ce projet nécessite l'ajout de la bibliothèque "JUANITO_LIB" dans vos librairies Arduino
///                     - GitHub : https://github.com/Juani005999/ARDUINO-JUANITO_LIB
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <JUANITO_LED.h>
#include <JUANITO_PASSIVE_BUZZER.h>

// Déclaration des constantes correspondant aux PIN
#define PIN_LED_YELLOW                      6                   // LED Jaune sur PIN ~PWM
#define PIN_LED_BLUE                        5                   // LED Bleue sur PIN ~PWM
#define PIN_PUSH_ACTION                     2                   // Push button pour activation LED et son
#define PIN_BUZZER                          11                  // Buzzer

// Déclaration des constantes propres à l'application
#define LED_YELLOW_MIN_VALUE                1                   // Valeur minimale de la LED jaune
#define LED_YELLOW_MAX_VALUE                100                 // Valeur maximale de la LED jaune
#define LED_BLUE_MIN_VALUE                  1                   // Valeur minimale de la LED bleue
#define LED_BLUE_MAX_VALUE                  25                  // Valeur maximale de la LED bleue

// Déclaration des objets interne
JUANITO_PASSIVE_BUZZER buzzer = JUANITO_PASSIVE_BUZZER();       // Buzzer passif
JUANITO_LED yellowLed = JUANITO_LED();                          // LED jaune
JUANITO_LED blueLed = JUANITO_LED();                            // LED bleue

// Flags interne d'action en cours
bool actionOn;

// Déclaration des variables nécessaire aux mesures des temps de Loop
long lastLoopTime = 0;                                          // DEBUG : Flag de lecture de l'interval mini
long minLoopTime = 0;                                           // DEBUG : Interval mini de la fonction loop
long maxLoopTime = 0;                                           // DEBUG : Interval mini de la fonction loop
float avgLoopTime = 0;                                          // DEBUG : Moyenne du temps d'un Interval
long nbTotalLoops = 0;                                          // DEBUG : Nombre total d'intervalles

/// ------------------------
/// ARDUINO : Setup
///
void setup() {

  // Démarrage SerialPort pour Debug
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Program started");

  // Initialisation des objets internes
  yellowLed.Init(PIN_LED_YELLOW, true, LED_YELLOW_MIN_VALUE, LED_YELLOW_MAX_VALUE);     // LED jaune : PIN en mode ~PWM
  blueLed.Init(PIN_LED_BLUE, true, LED_YELLOW_MIN_VALUE, LED_YELLOW_MAX_VALUE);         // LED bleue : PIN en mode ~PWM
  buzzer.Init(PIN_BUZZER);                                                              // Buzzer passif

  // Définition du mode PIN du bouton Push
  pinMode(PIN_PUSH_ACTION, INPUT_PULLUP);

  // Initialisation du flag d'action en cours
  actionOn = false;

  // Initialisation des chronos interne
  lastLoopTime = millis();
}

/// ------------------------
/// ARDUINO : Loop
///
void loop() {

  // DEBUG : On mesure les temps de Loop Mini, Maxi et AVG
  UpdateLoopMinMax();

  // Mise à jour de l'état d'action en cours
  SetActionState();
  
  // Positionnement des LEDs et du Buzzer en fonction de l'action en cours
  if (actionOn)
  {
    // On active le son
    buzzer.PlayNote(PASSIVE_BUZZER_NOTE_LA, PASSIVE_BUZZER_OCTAVE_4);

    // On positionne les LED
    yellowLed.Blink();
    blueLed.SetValue(50);                 // SetValue non disponible en mode PWM OFF
  }
  else
  {
    // On éteint les LEDs
    yellowLed.Off();
    blueLed.Off();
  }
}

/// --------------------------------
/// Mise à jour de l'état d'Action en cours sur action du Push button
///
void SetActionState()
{
  // Bouton Pressé
  actionOn = digitalRead(PIN_PUSH_ACTION) == LOW;
}

/// -------------------------
/// DEBUG : On mesure les temps de Loop Mini, Maxi et AVG
///
void UpdateLoopMinMax()
{
  // TimeSpan actuel
  long currentDif = millis() - lastLoopTime;

  // Temps min
  if (minLoopTime == 0
      || (currentDif != 0 && currentDif < minLoopTime))
  {
    minLoopTime = currentDif;
    if (minLoopTime != 0)
      Serial.println("LoopTime : min = " + String(minLoopTime) + " ms");
  }

  // Temps MAX
  if (maxLoopTime == 0 || currentDif > maxLoopTime)
  {
    maxLoopTime = currentDif;
    if (maxLoopTime != 0)
      Serial.println("LoopTime : MAX = " + String(maxLoopTime) + " ms");
  }

  // Actualisation moyenne
  avgLoopTime = avgLoopTime + ((currentDif - avgLoopTime) / (nbTotalLoops + 1));
  // On trace la moyenne tous les XX Loop
  if (nbTotalLoops % 100000 == 0)
    Serial.println("LoopTime : AVG = " + String(avgLoopTime, 5) + " ms / Nb. LOOPS = " + String(nbTotalLoops));
  
  // Actualisation du chrono et flag
  lastLoopTime = millis();
  nbTotalLoops++;
}