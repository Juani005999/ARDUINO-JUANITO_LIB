/// ---------------------------------------------------------------------
///
/// Objet           : JUANITO_HCSR04
/// Auteur          : Juanito del Pepito
/// Date            : 04/04/2023
/// Version         : 1.1.0.5
/// Description     : Objet permettant la gestion du module Ultra-Sons de type HCSR04
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"

// Déclaration des constantes propres à l'objet
#define HCSR04_MEASURE_TIMEOUT						25000UL					// 25ms = ~8m à 340m/s
#define HCSR04_DEFAULT_MEASURE_INTERVAL				400						// Temps (ms) minimum entre 2 mesures de distance
#define HCSR04_MIN_MEASURE_INTERVAL					300						// Temps (ms) minimum entre 2 mesures de distance (seuil mini)
#define HCSR04_MEASURE_BORNE_MIN					20						// Borne minimale acceptée pour les mesure (limites constructeur)
#define HCSR04_MEASURE_BORNE_MAX					4000					// Borne maximale acceptée pour les mesure (limites constructeur)

// Constantes servant aux calculs
const float SOUND_SPEED = 340.0 / 1000;									    // Vitesse du son dans l'air en mm/us

/// <summary>
/// JUANITO_HCSR04 : Objet permettant la gestion du module Ultra-Sons de type HCSR04
/// </summary>
class JUANITO_HCSR04
{
	public:
		JUANITO_HCSR04();
		Init(int pinEcho, int triggerPin, int measureInterval = HCSR04_DEFAULT_MEASURE_INTERVAL);
		float Distance(float temperature = NAN, float humidity = NAN);

	private:
		// Champs valorisés à l'initialisation de l'objet
		int _pinEcho = NAN;
		int _pinTrigger = NAN;
		int _measureInterval = NAN;

		float _distance = NAN;
		float _distanceCorrigee = NAN;
		long _chronoLastDistance;
};