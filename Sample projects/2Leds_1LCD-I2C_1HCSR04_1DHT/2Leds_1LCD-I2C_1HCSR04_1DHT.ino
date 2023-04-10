/// ---------------------------------------------------------------------
///
/// Projet          : 2Leds_1LCD-I2C_1HCSR04_1DHT
/// Auteur          : Juanito del Pepito
/// Version         : 0.0.0.1
/// Date            : 10/04/2023
/// Description     : Projet témoin
///                     - Tuto montrant l'utilisation des objets JUANITO_LED, JUANITO_LCD_I2C, JUANITO_HCSR04, JUANITO_DHT
///                     - Ce projet nécessite l'ajout de la bibliothèque "JUANITO_LIB" dans vos librairies Arduino
///                     - GitHub : https://github.com/Juani005999/ARDUINO-JUANITO_LIB
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <JUANITO_LED.h>
#include <JUANITO_LCD_I2C.h>
#include <JUANITO_HCSR04.h>
#include <JUANITO_DHT.h>

// Déclaration des constantes correspondant aux PIN
#define PIN_LED_YELLOW                    3                   // LED Jaune sur PIN ~PWM
#define PIN_LED_BLUE                      5                   // LED Bleue sur PIN ~PWM
#define PIN_PUSH_ACTION                   2                   // Push button pour changement d'action : Mesure ON/OFF
#define PIN_HCSR04_TRIGGER                9                   // TRIGGER du Module Ultra Son HC-SR04
#define PIN_HCSR04_ECHO                   10                  // ECHO du Module Ultra Son HC-SR04
#define PIN_DHT_SENSOR                    8                   // PIN du Sensor de température et d'humidité

// Déclaration des constantes propres à l'application
#define LED_YELLOW_MIN_VALUE              1                   // Valeur minimale de la LED jaune
#define LED_YELLOW_MAX_VALUE              50                  // Valeur maximale de la LED jaune
#define LED_BLUE_MIN_VALUE                1                   // Valeur minimale de la LED bleue
#define LED_BLUE_MAX_VALUE                5                   // Valeur maximale de la LED bleue
#define LCD_COMPARE_GLYPHE_INTERVAL       350                 // Tx de rafraichissement (ms) du glyphe de comparaison de distance

// Déclarations des constantes nécessaires à la mesure de distance
#define HCSR04_MEASURE_INTERVAL           350                 // Taux de rafraichissement des mesures de distance (ms)

// Déclaration des constantes relatives au Sensor DHT (température et humidité)
#define DHT_SENSOR_TYPE                   DHT11               // Type de Sensor [DHT11|DHT12|DHT21|DHT22|AM2301]
#define DHT_SENSOR_INTERVAL               4000                // Taux de rafraichissement des mesures de température et d'humidité

// Déclaration des objets interne
JUANITO_LCD_I2C lcd = JUANITO_LCD_I2C();                      // Ecran LCD I2C
JUANITO_LED yellowLed = JUANITO_LED();                        // LED jaune
JUANITO_LED blueLed = JUANITO_LED();                          // LED bleue
JUANITO_HCSR04 hcsr04 = JUANITO_HCSR04();                     // Module Ultra-Son HCSR04
JUANITO_DHT dht = JUANITO_DHT();                              // Sensor DHT11 de température et d'humidité

// Flags interne d'action en cours
volatile bool mesureOn = false;                               // volatile car son état est modifié par le biais d'une interruption

// Déclaration des variables nécessaires à la mesure des variables d'environnement (température et humidité)
long lastMesure = -1;                                         // Dernière mesure effectuée
long lastCompareMesure = -1;                                  // Sauvegarde de la dernière mesure effectuée à titre de comparaison
long chronoMesure;
long chronoCompareGlyphe;
char mesureTexte[50] = "";                                    // Chaine de caractère servant à stocker le texte relatif à le mesure de distance
                                                              // La manipulation d'objet String est couteuse en ressources et performances,
                                                              // cette chaine est donc actualisée sur Interval

// Déclaration des variables nécessaires à la mesure des variables d'environnement (température et humidité)
float temperature = NAN;
float lastTemperature = NAN;                                  // Flag permettant de connaitre l'évolution de la température par rapport à la dernière mesure
float humidite = NAN;
long chronoEnvironment;
char environementTexte[50] = "";                              // Chaine de caractère servant à stocker le texte relatif aux conditions d'environnement
                                                              // La manipulation d'objet String est couteuse en ressources et performances,
                                                              // cette chaine est donc actualisée sur Interval

// Déclaration des variables nécessaires à la gestion des interruptions
long chronoLastInterrupt;                                     // Chrono servant à détecter les interruptions successives et immédiates
const int intervalInterrupt = 500;                            // Interval en dessous duquel une intervention immédiate n'est pas prise en compte (dbl-click ?)

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
  lcd.Init();                                                                           // Ecran LCD I2C [par défaut, Adresse 0x27, 16 colonnes, 2 lignes]
  yellowLed.Init(PIN_LED_YELLOW, true, LED_YELLOW_MIN_VALUE, LED_YELLOW_MAX_VALUE);     // LED jaune : PIN en mode ~PWM
  blueLed.Init(PIN_LED_BLUE, true, LED_YELLOW_MIN_VALUE, LED_YELLOW_MAX_VALUE);         // LED bleue : PIN en mode ~PWM
  hcsr04.Init(PIN_HCSR04_ECHO, PIN_HCSR04_TRIGGER, HCSR04_MEASURE_INTERVAL);            // Module de lecture de distance par Ultra-Sons HCSR04
  dht.Init(PIN_DHT_SENSOR, DHT_SENSOR_TYPE, DHT_SENSOR_INTERVAL);                       // Sensor DHT11 de température et d'humidité

  // Définition du mode PIN du bouton Push
  pinMode(PIN_PUSH_ACTION, INPUT_PULLUP);

  // On attache une interruption sur le bouton Mesure ON/OFF : plus facile de passer par la gestion de l'event "CHANGE"
  attachInterrupt(digitalPinToInterrupt(PIN_PUSH_ACTION), ISR_ChangeMesureState, CHANGE);

  // On affiche un petit message de bienvenue et on attend 4s :)
  lcd.setCursor(0, 0);                                        // Positionnement curseur
  lcd.Smiley();                                               // Ecriture glyphe Smiley
  lcd.PrintRightJustify("Hello World", 0);                    // Ecriture synchrone alignée à droite
  lcd.setCursor(0, 1);                                        // Positionnement curseur
  lcd.print("Chargement ...");                                // Ecriture synchrone de texte
  delay(4000);                                                // Le Sensor DHT a besoin d'un peu de temps avant de renvoyer des valeurs cohérentes

  // On clear l'affichage
  lcd.clear();

  // Initialisation du flag d'action en cours
  mesureOn = false;

  // Initialisation des chronos interne
  lastLoopTime = millis();
}

/// ------------------------
/// ARDUINO : Loop
///
void loop() {

  // DEBUG : On mesure les temps de Loop Mini, Maxi et AVG
  UpdateLoopMinMax();

  // Actualisation des mesures d'environnement
  humidite = dht.Humidity();                                      // Lecture asynchrone du taux d'humidité (mesure effectuée toute les DHT_SENSOR_INTERVAL ms)
  temperature = dht.Temperature();                                // Lecture asynchrone de la température (mesure effectuée toute les DHT_SENSOR_INTERVAL ms)

  // Actualisation de la mesure de distance si mesure en cours
  if (mesureOn)                                                   // Lecture asynchrone de la distance mesurée (mesure effectuée toute les HCSR04_MEASURE_INTERVAL ms)
    lastMesure = hcsr04.Distance(temperature, humidite);          // Le calcul de la distance est corrigé en fonction des conditions de température et d'humidité

  // Actualisation du texte relatif à la mesure de distance et d'environnement
  UpdateDistanceText();
  UpdateEnvironmentText();
  
  // Affichage sur LCD des infos de température et d'humidité si on est plus sur l'écran de démarrage et qu'une mesure a déjà été effectuée
  if (lastMesure != -1)
    lcd.DisplayScrollingText(environementTexte, 1, 11, -1, 1000); // Texte défilant
                                                                  // => sur ligne indice 1, StartIndex indice 11, Longueur restante de l'écran, avec un interval de scroll de 1s

  // Positionnement des LEDs et de l'affichage LCD en fonction de l'action en cours
  if (mesureOn)
  {
    // Clignotement des LEDs
    yellowLed.Blink(lastMesure);                                  // Clignotement de la LED jaune sur un rythme en fonction de la distance
    if (lastMesure < 300)                                         // Si distance < 30cm, on double clignote la LED bleue
      blueLed.DoubleBlink();
    else if (lastMesure < 500)                                    // Sinon, si distance < 50cm, on clignote la LED bleue
      blueLed.Blink();
    else
      blueLed.Oscillate(0.1);                                     // Sinon, Oscillation de la LED bleue avec un facteur 10 de l'oscillation de référence

    // Actualisation de l'affichage
    lcd.DisplayText("Mesure en cours ", 0);                       // Titre ligne 1
    lcd.DisplayText(mesureTexte, 1);                              // Distance mesurée ligne 2
  }
  else
  {
    // On éteint les LEDs
    yellowLed.Off();
    blueLed.Off();

    // Actualisation de l'affichage
    if (lastMesure != -1)
    {
      // Si une mesure a déjà été effectuée
      lcd.DisplayText("Derniere mesure ", 0);                       // Titre ligne 1
      lcd.DisplayText(mesureTexte, 1);                              // Dernière distance mesurée ligne 2
    }
    else
    {
      // Si aucne mesure n'a déjà été effectuée
      lcd.DisplayText("Mesure de dist. ", 0);                       // Titre ligne 1
      lcd.DisplayScrollingText("Cliquez pour commencer \5", 1);     // Texte défilant ligne 2
    }    
  }

  // Actualisation du glyphe de progression des mesure
  UpdateCompareGlyphe();
}

/// ------------------------------------
/// Actualisation de l'état du flag Mesure ON/OFF sur Interruption (CHANGE) pour faciliter la gestion
///
void ISR_ChangeMesureState()
{
  // On ne gère pas les interruptions en dessous de l'intervalle afin d'éviter les interruptions successives
  if (millis() > chronoLastInterrupt + intervalInterrupt)
  {
    // On lit l'état du bouton
    bool buttonMesurePressed = digitalRead(PIN_PUSH_ACTION) == LOW;

    // Si bouton pressé, on inverse l'état de mesure en cours
    if (buttonMesurePressed)
      mesureOn = !mesureOn;

    // Actualisation du chrono
    chronoLastInterrupt = millis();
  }
}

/// --------------------------------
/// Mise à jour du texte relatif à la distance mesurée
/// La manipulation de l'objet String est couteuse en ressources et en performances
/// => L'actualisation de cette chaine est effectuée par intervalle
/// Les mesures sont actualisées toute les 350ms => on utilise le même interval pour actualiser la chaine
///
void UpdateDistanceText()
{
  // On effectue aucun traitement si on est dessous de l'interval
  if (millis() > chronoMesure + HCSR04_MEASURE_INTERVAL)
  {
    // Si lastMesure est défini
    if (lastMesure != -1)
    {
      String mesureString;
      // Si la mesure vaut 0, c'est une erreur remontée par le module => On traite la mesure comme "hors limites"
      // Gestion des valeurs limites constructeur : 2cm -> 4m
      if (lastMesure < HCSR04_MEASURE_BORNE_MIN || lastMesure > HCSR04_MEASURE_BORNE_MAX)
      {
        mesureString = "Hors limites";
      }
      else
      {
        // On dissocie les mètres des centimètres
        int metres = lastMesure / 1000;
        long centimetres = lastMesure - (metres * 1000);
        if (metres > 0)
        {
          mesureString = String(metres) + "m";
        }
        mesureString += String(((float)centimetres / 10), 1) + "cm";

        // Si nécessaire, on rajoute des espaces jusqu'a atteindre 9 caractères
        while (mesureString.length() < 9)
          mesureString += " ";
      }

      // On recopie le résultat dans la chaine finale
      mesureString.toCharArray(mesureTexte, mesureString.length() >= 50 ? 50 : mesureString.length() + 1);
    }
    else
    {
      // Pas de données d'env. à afficher => message par défaut
      strcpy(mesureTexte, "Aucune mesure   ");
    }

    // Actualisation du chrono
    chronoMesure = millis();
  }
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
    // Si température et humidité sont définies
    if (!isnan(temperature) && !isnan(humidite))
    {
      // On formate la phrase souhaitée dans un objet String (manipulation plus aisée)
      String envText = "Temp.:" + String(temperature, 1) + "\7C/Hum.:" + String(humidite, 0) + "% ";

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

/// --------------------------------
/// Mise à jour du glyphe relatif à la progression de distance mesurée
/// Le glyphe s'écrit de manière synchrone sur le LCD
/// => L'actualisation de ce glyphe est effectuée par intervalle
///
void UpdateCompareGlyphe()
{
  if (millis() > chronoCompareGlyphe + LCD_COMPARE_GLYPHE_INTERVAL)
  {
    // Ecriture du glyphe en fonction de l'évolution par rapport à la dernière mesure
    lcd.setCursor(9, 1);                              // Positionnement du curseur
    if (lastMesure != -1 && lastCompareMesure != -1)
    {
      if (lastMesure > lastCompareMesure)
        lcd.UpArrow();                                // Ecriture flèche haut
      else if (lastMesure < lastCompareMesure)
        lcd.DownArrow();                              // Ecriture flèche bas
      else
        lcd.RightArrow();                             // Ecriture flèche droite
      lcd.print(" ");                                 // On rajoute 2 espaces afin d'enlever tout résidu éventuel du texte défilant de démarrage
    }
    else if (lastMesure != -1)
    {
      lcd.print(" ");                                 // On efface les caractères existant éventuel si une première mesure a déjà été effectuée
    }

    // Actualisation des flags si mesure en cours
    if (mesureOn)
      lastCompareMesure = lastMesure;

    // Actualisation du chrono
    chronoCompareGlyphe = millis();
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