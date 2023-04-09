/// ---------------------------------------------------------------------
///
/// Objet           : JUANITO_LED
/// Auteur          : Juanito del Pepito
/// Date            : 04/04/2023
/// Version         : 1.1.0.5
/// Description     : Objet permettant la gestion d'une LED
///                   - Branchez la LED sur une PIN "~PWM" afin de pouvoir piloter le niveau
///                   - Si vous branchez la LED sur une PIN "~PWM", positionner le boolean "isPWM" à true à la construction de l'objet
///                     et précisez des valeurs min et max
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"

// Déclaration des constantes propres à l'objet
#define LED_DEFAULT_SIMPLE_BLINK_INTERVAL		500					// Intervalle de temps (ms) d'un clignotement simple
#define LED_DEFAULT_DOUBLE_BLINK_INTERVAL		100					// Intervalle de temps (ms) d'un clignotement double
#define LED_DEFAULT_ON_INTERVAL					100					// Intervalle de temps (ms) sans "renvoi" de la commande ON
#define LED_DEFAULT_OFF_INTERVAL				100					// Intervalle de temps (ms) sans "renvoi" de la commande OFF
#define LED_DEFAULT_OSCILLATE_CHRONO_INTERVAL	10					// Intervalle de temps (ms) pour l'oscillation minimum
#define LED_DEFAULT_OSCILLATE_COEF_STD			20					// Coef standard d'adaptation de l'oscillation
#define LED_DEFAULT_OSCILLATE_LEVEL_INTERVAL	5					// Intervalle de niveau entre chaque oscillation

// Enum définissant les états de la LED
enum LedState
{
  LED_STATE_OFF     = 0,											// Led éteinte (LOW)
  LED_STATE_ON      = 1,											// Led allumée (HIGH)
  LED_STATE_MIN     = 2,											// Led à _minValue
  LED_STATE_MAX     = 3,											// Led à _maxValue
  LED_STATE_RANDOM  = 4												// Led à valeur non définie
};

/// <summary>
/// JUANITO_LED : Objet permettant la gestion d'une LED
/// </summary>
class JUANITO_LED
{
	public:
		JUANITO_LED();
		Init(int pin, bool isPWM = false, int minValue = 0, int maxValue = 255);
		On(bool setToMaxValue = false, int interval = LED_DEFAULT_ON_INTERVAL);
		Off(bool setToMinValue = false, int interval = LED_DEFAULT_OFF_INTERVAL);
		Blink(int interval = LED_DEFAULT_SIMPLE_BLINK_INTERVAL);
		DoubleBlink(int interval = LED_DEFAULT_DOUBLE_BLINK_INTERVAL);
		Oscillate(double coefInterval = -1, bool reverseOscillation = false);
		SetValue (int value);

	private:
		// Champs valorisés à l'initialisation de l'objet
		int _pin;
		int _minValue;
		int _maxValue;
		bool _isPWM;

		// Champs internes
		LedState _state = LED_STATE_OFF;
		bool _ledOscillateSensUp = true;
		int _ledDoubleBlinkStage = 0;
		int _ledOscillateCurrentLevel;

		// Chronos
		long _chronoOnLoop;
		long _chronoOffLoop;
		long _chronoBlinkLoop;
		long _chronoDoubleBlinkLoop;
		long _chronoOscillateLoop;
};