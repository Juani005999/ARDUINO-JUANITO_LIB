/// ---------------------------------------------------------------------
///
/// Projet          : 2Leds_1LCD-I2C_1DHT
/// Auteur          : Juanito del Pepito
/// Version         : 0.0.0.1
/// Date            : 09/04/2023
/// Description     : Projet témoin
///                     - Tuto montrant l'utilisation des objets JUANITO_LED, JUANITO_LCD_I2C, JUANITO_DHT
///                     - Ce projet nécessite l'ajout de la bibliothèque "JUANITO_LIB" dans vos librairies Arduino
///                     - GitHub : https://github.com/Juani005999/ARDUINO-JUANITO_LIB
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <JUANITO_LED.h>
#include <JUANITO_LCD_I2C.h>
#include <JUANITO_DHT.h>

// Déclaration des constantes correspondant aux PIN
#define PIN_LED_YELLOW                    3                   // LED Jaune sur PIN ~PWM
#define PIN_LED_BLUE                      5                   // LED Bleue sur PIN ~PWM
#define PIN_PUSH_ACTION                   7                   // Push button pour activation des Leds

// Déclaration des constantes propres à l'application
#define LED_YELLOW_MIN_VALUE              1                   // Valeur minimale de la LED jaune
#define LED_YELLOW_MAX_VALUE              50                  // Valeur maximale de la LED jaune
#define LED_BLUE_MIN_VALUE                1                   // Valeur minimale de la LED bleue
#define LED_BLUE_MAX_VALUE                5                   // Valeur maximale de la LED bleue

// Déclarations des constantes nécessaire à l'affichage (LCD) : Ces valeurs sont identiques aux valeurs par défaut donc leur utilisation est optionnelle
#define LCD_ADDRESS                       0x27                // Adresse du LCD sur le bus I2C
#define LCD_COLS                          16                  // Nombre de colonnes du LCD
#define LCD_ROWS                          2                   // Nombre de lignes du LCD

// Déclaration des constantes relatives au Sensor DHT (température et humidité)
#define DHT_SENSOR_TYPE                   DHT11               // Type de Sensor [DHT11|DHT12|DHT21|DHT22|AM2301]
#define DHT_SENSOR_PIN                    8                   // PIN du Sensor de température et d'humidité
#define DHT_SENSOR_INTERVAL               4000                // Taux de rafraichissement des mesures de température et d'humidité

// Déclaration des objets interne
JUANITO_LED yellowLed = JUANITO_LED();                        // LED jaune
JUANITO_LED blueLed = JUANITO_LED();                          // LED bleue
JUANITO_LCD_I2C lcd = JUANITO_LCD_I2C();                      // Ecran LCD I2C
JUANITO_DHT dht = JUANITO_DHT ();                             // Sensor DHT11 de température et d'humidité

// Flags interne d'action en cours
bool ledsOn;

// Déclaration des variables nécessaires à la mesure des variables d'envirronement (température et humidité)
float temperature = NAN;
float lastTemperature = NAN;                                  // Flag permettant de connaitre l'évolution de la température par rapport à la dernière mesure
float humidite = NAN;
long chronoEnvironment;
char environementTexte[50] = "";                              // Chaine de caractère servant à stocker le texte relatif aux conditions d'envirorronement
                                                              // La manipulation d'objet String est couteuse en ressources et performances,
                                                              // cette chaine est donc actualisée sur Interval

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
  lcd.Init(LCD_ADDRESS, LCD_COLS, LCD_ROWS);                                            // Ecran LCD I2C 16 X 2
  yellowLed.Init(PIN_LED_YELLOW, true, LED_YELLOW_MIN_VALUE, LED_YELLOW_MAX_VALUE);     // PIN en mode ~PWM
  blueLed.Init(PIN_LED_BLUE, true, LED_YELLOW_MIN_VALUE, LED_YELLOW_MAX_VALUE);         // PIN en mode ~PWM
  dht.Init(DHT_SENSOR_PIN, DHT_SENSOR_TYPE, DHT_SENSOR_INTERVAL);                       // Sensor DHT11 de température et d'humidité

  // Définition du mode PIN du bouton Push
  pinMode(PIN_PUSH_ACTION, INPUT_PULLUP);

  // On affiche un petit message de bienvenue et on attend 2s :)
  lcd.setCursor(0, 0);                                        // Positionnement curseur
  lcd.Smiley();                                               // Ecriture glyphe Smiley
  lcd.PrintRightJustify("Hello World", 0);                    // Ecriture synchrone alignée à droite
  lcd.setCursor(0, 1);                                        // Positionnement curseur
  lcd.print("Chargement ...");                                // Ecriture synchrone de texte
  delay(4000);                                                // Le Sensor DHT a besoin d'un peu de temps avant de renvoyer des valeurs cohérentes

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

  // Actualisation des mesures d'environnement
  humidite = dht.Humidity();                                      // Lecture asynchrone du taux d'humidité (mesure effectuée toute les DHT_SENSOR_INTERVAL ms)
  temperature = dht.Temperature();                                // Lecture asynchrone de la température (mesure effectuée toute les DHT_SENSOR_INTERVAL ms)

  // Actualisation du texte relatif aux conditions de température et d'humidité et affichage du texte sur l'écran LCD
  UpdateEnvironmentText();
  lcd.DisplayScrollingText(environementTexte, 0, 0, -1, 1000);    // Texte défilant : sur ligne indice 0, StartIndex indice 0, Longueur max de l'écran, avecun interval de scroll de 1s

  // Positionnement des LEDs et de l'affichage LCD en fonction de l'action en cours
  if (ledsOn)
  {
    // Clignotement des LEDs
    yellowLed.Blink();                                            // Clignotement de la LED jaune
    blueLed.Oscillate(0.1);                                       // Oscillation de la LED bleue avec un facteur 10 de l'oscillation de référence

    // Actualisation de l'affichage
    lcd.DisplayText("LED STATE : ON ", 1);                        // Ecriture en Asynchrone d'un texte sur la 2ème ligne
  }
  else
  {
    // On éteint les LEDs
    yellowLed.Off();
    blueLed.Off();

    // Actualisation de l'affichage
    lcd.DisplayText("LED STATE : OFF", 1);                        // Ecriture en Asynchrone d'un texte sur la 2ème ligne
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

/// --------------------------------
/// Mise à jour du texte relatif aux conditions d'environnement
/// La manipulation de l'objet String est couteuse en ressources et en performances
/// => L'actualisation de cette chaine est effectuée par intervalle
/// Les mesures sont actualisées toute les 4s => on utilise le même interval pour actualiser la chaine
///
void UpdateEnvironmentText()
{
  // On effectue aucun traitement si on est dessous de l'interval
  if (millis() > chronoEnvironment + DHT_SENSOR_INTERVAL)
  {
    // Si température et humidité ne sont pas défini
    if (!isnan(temperature) && !isnan(humidite))
    {
      // On formate la phrase souhaitée dans un objet String (manipulation plus aisée des string)
      String envText = "Temp.:" + String(temperature, 1) + "\7C / Hum.:" + String(humidite, 0) + "% ";

      // Détermination du glyphe en fonction de l'évolution par rapport à la dernière mesure
      String glypheEvolution = "";
      if (!isnan(lastTemperature))
      {
        if (temperature > lastTemperature)
          glypheEvolution = "\0";
        else if (temperature < lastTemperature)
          glypheEvolution = "\1";
        else
          glypheEvolution = "\2";
      }
      envText += glypheEvolution;

      // Actualisation des flags
      lastTemperature = temperature;
      
      // On recopie le résultat dans la chaine finale
      envText.toCharArray(environementTexte, envText.length() >= 50 ? 50 : envText.length() + 1);
    }
    else
    {
      // Pas de données d'env. à afficher => message par défaut
      strcpy(environementTexte, "Aucune donnee   ");
    }

    // Actualisation du chrono
    chronoEnvironment = millis();
  }
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