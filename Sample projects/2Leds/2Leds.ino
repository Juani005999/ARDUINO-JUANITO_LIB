/// ---------------------------------------------------------------------
///
/// Projet          : 2Leds
/// Auteur          : Juanito del Pepito
/// Version         : 0.0.0.1
/// Date            : 09/04/2023
/// Description     : Projet témoin
///                     - Tuto montrant l'utilisation de l'objet JUANITO_LED en mode ~PWM et non ~PWM
///                     - En mode ~PWM, vous pouvez piloter la valeur de la LED.
///                     - En mode NON ~PWM, certaines fonctionnalités ne sont pas disponible comme led.SetValue ou led.oscillate
///                     - Cet application nécessite l'ajout de la bibliothèque "JUANITO_LIB"
///                     - GitHub : https://github.com/Juani005999/ARDUINO-JUANITO_LIB
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <JUANITO_LED.h>

// Déclaration des constantes correspondant aux PIN
#define PIN_LED_YELLOW            3                           // LED Jaune sur PIN ~PWM => on pourra piloter sa valeur
#define PIN_LED_BLUE              4                           // LED Bleue sur PIN NON ~PWM => uniquement 2 valeurs possible HIGH et LOW
#define PIN_PUSH_ACTION           7                           // Push button pour activation des Leds

// Déclaration des constantes propres à l'application
#define LED_YELLOW_MIN_VALUE      1                           // Valeur minimale de la LED jaune
#define LED_YELLOW_MAX_VALUE      50                          // Valeur maximale de la LED jaune

// Déclaration des objets interne
JUANITO_LED yellowLed = JUANITO_LED();                        // LED jaune
JUANITO_LED blueLed = JUANITO_LED();                          // LED bleue

// Flags interne d'action en cours
bool ledsOn;

// Déclaration des variables nécessaire aux mesures des temps de Loop
long lastLoopTime = 0;                                        // DEBUG : Flag de lecture de l'interval mini
long minLoopTime = 0;                                         // DEBUG : Interval mini de la fonction loop
long maxLoopTime = 0;                                         // DEBUG : Interval mini de la fonction loop
float avgLoopTime = 0;                                        // DEBUG : Moyenne du temps d'un Interval
long nbTotalLoops = 0;                                        // DEBUG : Nombre total d'intervalles

/// ------------------------
/// ARDUINO : Setup
///
void setup() {

  // Démarrage SerialPort pour Debug
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Program started");

  // Initialisation des objets internes
  yellowLed.Init(PIN_LED_YELLOW, true, LED_YELLOW_MIN_VALUE, LED_YELLOW_MAX_VALUE);     // PIN en mode ~PWM : on valorise isPwm à true afin de piloter les valeurs min et max de la LED
  blueLed.Init(PIN_LED_BLUE);                                                           // PIN en mode NON ~PWM : initialisation simple avec les valeurs par défaut
  
  // Définition du mode PIN du bouton Push
  pinMode(PIN_PUSH_ACTION, INPUT_PULLUP);

  // Initialisation du flag d'action en cours
  ledsOn = false;

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
  
  // Positionnement des LEDs en fonction de l'action en cours
  if (ledsOn)
  {
    yellowLed.Blink(800);                                         // On fait clignoter la LED jaune en forcant l'intervalle par défaut (500 ms) à 800 ms
    blueLed.DoubleBlink();                                        // On fait "double" clignoter la LED bleu
  }
  else
  {
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
  ledsOn = digitalRead(PIN_PUSH_ACTION) == LOW;
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