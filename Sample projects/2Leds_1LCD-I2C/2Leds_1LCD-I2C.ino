/// ---------------------------------------------------------------------
///
/// Projet          : 2Leds_1LCD-I2C
/// Auteur          : Juanito del Pepito
/// Version         : 0.0.0.1
/// Date            : 09/04/2023
/// Description     : Projet témoin
///                     - Tuto montrant l'utilisation de l'objet JUANITO_LED et de l'objet JUANITO_LCD_I2C
///                     - Cet application nécessite l'ajout de la bibliothèque "JUANITO_LIB"
///                     - GitHub : https://github.com/Juani005999/ARDUINO-JUANITO_LIB
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <JUANITO_LED.h>
#include <JUANITO_LCD_I2C.h>

// Déclaration des constantes correspondant aux PIN
#define PIN_LED_YELLOW            3                           // LED Jaune sur PIN ~PWM => on pourra piloter sa valeur
#define PIN_LED_BLUE              4                           // LED Bleue sur PIN NON ~PWM => uniquement 2 valeurs possible HIGH et LOW
#define PIN_PUSH_ACTION           7                           // Push button pour activation des Leds

// Déclaration des constantes propres à l'application
#define LED_YELLOW_MIN_VALUE      1                           // Valeur minimale de la LED jaune
#define LED_YELLOW_MAX_VALUE      50                          // Valeur maximale de la LED jaune

// Déclarations des constantes nécessaire à l'affichage (LCD) : Ces valeurs sont identiques aux valeurs par défaut donc leur utilisation est optionnelle
#define LCD_ADDRESS                       0x27                // Adresse du LCD sur le bus I2C
#define LCD_COLS                          16                  // Nombre de colonnes du LCD
#define LCD_ROWS                          2                   // Nombre de lignes du LCD

// Déclaration des objets interne
JUANITO_LED yellowLed = JUANITO_LED();                        // LED jaune
JUANITO_LED blueLed = JUANITO_LED();                          // LED bleue
JUANITO_LCD_I2C lcd = JUANITO_LCD_I2C();                      // Ecran LCD I2C

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
  lcd.Init(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
  yellowLed.Init(PIN_LED_YELLOW, true, LED_YELLOW_MIN_VALUE, LED_YELLOW_MAX_VALUE);     // PIN en mode ~PWM : on valorise isPwm à true afin de piloter les valeurs min et max de la LED
  blueLed.Init(PIN_LED_BLUE);                                                           // PIN en mode NON ~PWM : initialisation simple avec les valeurs par défaut
  
  // Définition du mode PIN du bouton Push
  pinMode(PIN_PUSH_ACTION, INPUT_PULLUP);

  // On affiche un petit message de bienvenue et on attend 2s :)
  lcd.setCursor(0, 0);                                        // Positionnement curseur
  lcd.print("Hello");                                         // Ecriture synchrone de texte
  lcd.PrintRightJustify("World  ", 0);                        // Ecriture synchrone alignée à droite
  lcd.setCursor(15, 0);                                       // Positionnement curseur
  lcd.Smiley();                                               // Ecriture glyphe Smiley
  lcd.setCursor(0, 1);                                        // Positionnement curseur
  lcd.print("Chargement ...");                                // Ecriture synchrone de texte
  delay(2000);

  // On clear l'affichage
  lcd.clear();

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

  // Affichage d'un texte défilant Asynchrone (par défaut sur la première ligne dans sa totalité)
  lcd.DisplayScrollingText("Projet example montrant l'utilisation des objets JUANITO_LED et JUANITO_LCD_I2C de la JUANITO_LIB \5");
  
  // Positionnement des LEDs et de l'affichage LCD en fonction de l'action en cours
  if (ledsOn)
  {
    // Clignotement des LEDs
    yellowLed.Blink();                                            // On fait clignoter la LED jaune
    blueLed.DoubleBlink();                                        // On fait "double" clignoter la LED bleu

    // Actualisation de l'affichage
    lcd.DisplayText("LED STATE : ON ", 1);                          // Ecriture en Asynchrone d'un texte sur la éème ligne
  }
  else
  {
    // On éteint les LEDs
    yellowLed.Off();
    blueLed.Off();

    // Actualisation de l'affichage
    lcd.DisplayText("LED STATE : OFF", 1);                          // Ecriture en Asynchrone d'un texte sur la éème ligne
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