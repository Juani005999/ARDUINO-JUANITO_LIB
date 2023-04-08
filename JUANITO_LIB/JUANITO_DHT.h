/// ---------------------------------------------------------------------
///
/// Objet           : JUANITO_DHT
/// Auteur          : Juanito del Pepito
/// Date            : 04/04/2023
/// Version         : 1.1.0.2
/// Description     : Objet permettant la lecture des variables d'environement de température et d'humidité avec un sensor de type Adafruit DHT
///                     - Cet objet nécessite l'ajout des bibliothèques "DHT_sensor_library" et "Adafruit Unified Sensor Library" depuis l'IDE Arduino
///                     - Cet objet est un wrapper de l'objet DHT, avec ajout de quelques fonctionnalités supplémentaire
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <DHT.h>

// Déclaration des constantes propres à l'objet
#define DHT_SENSOR_DEFAULT_MEASURE_INTERVAL			4000                // Taux de rafraichissement par défaut des mesures de température et d'humidité
#define DHT_SENSOR_MIN_MEASURE_INTERVAL				  1000                // Taux de rafraichissement par défaut des mesures de température et d'humidité

/// <summary>
/// JUANITO_DHT : Objet permettant la lecture des variables d'environement de température et d'humidité avec un sensor de type Adafruit DHT
/// </summary>
class JUANITO_DHT
{
	public:
		JUANITO_DHT();
		~JUANITO_DHT();
		Init(int pin, int sensorType = DHT11, int measureInterval = DHT_SENSOR_DEFAULT_MEASURE_INTERVAL);
		float Humidity();
		float Temperature();

	private:
		readEnvironment();

		// Objet interne
		DHT * _dht;														                          // Sensor Adafruit DHT

		// Champs valorisés à l'initialisation de l'objet
		int _pin = -1;
		int _sensorType;
		int _measureInterval = -1;

		// Champs internes permettant de retenir les dernières valeures de mesure de température et d'humidité
		float _temperature;
		float _humidity;
		long _chronoLastMesure;
};