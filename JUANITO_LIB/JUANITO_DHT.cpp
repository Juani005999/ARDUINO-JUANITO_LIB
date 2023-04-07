#include "JUANITO_DHT.h"

/// <summary>
/// Constructeur
/// </summary>
JUANITO_DHT::JUANITO_DHT()
{
}

/// <summary>
/// Destructeur
/// </summary>
JUANITO_DHT::~JUANITO_DHT()
{
    // Free de la mémoire en sortant
    free(_dht);
}

/// <summary>
/// Initialisation du composant
/// </summary>
/// <param name="pin"></param>
/// <param name="sensorType"></param>
/// <param name="measureInterval"></param>
JUANITO_DHT::Init (int pin, int sensorType, int measureInterval)
{
    // Valorisations des membres interne
    _pin = pin;
    _sensorType = sensorType;
    _measureInterval = measureInterval;

    // Sécurité sur seuil mini
    if (_measureInterval < DHT_SENSOR_MIN_MEASURE_INTERVAL)
        _measureInterval = DHT_SENSOR_MIN_MEASURE_INTERVAL;

    // Initialisation objet DHT
    _dht = new DHT(_pin, _sensorType);
}

/// <summary>
/// Renvoi l'humidité mesurée lors du dernier intervalle
/// </summary>
/// <returns></returns>
float JUANITO_DHT::Humidite()
{
    // Actualisation des membres interne si nécessaire
    readEnvironment();

    // Retour
    return _humidite;
}

/// <summary>
/// Renvoi la température mesurée lors du dernier intervalle
/// </summary>
/// <returns></returns>
float JUANITO_DHT::Temperature()
{
    // Actualisation des membres interne si nécessaire
    readEnvironment();
    
    // Retour
    return _temperature;
}

/// <summary>
/// Lecture des conditions d'environnement (température et humidité)
/// Lecture des données du Sensor avec une fréquence défini par l'intervalle
/// </summary>
JUANITO_DHT::readEnvironment()
{
    // Actualisation des données d'environnement si en dessous de l'intervalle de lecture
    if (millis() > _chronoLastMesure + _measureInterval)
    {
        // Actualisation des membres internes
        _humidite = _dht->readHumidity();
        _temperature = _dht->readTemperature();

        // Actualisation du chrono
        _chronoLastMesure = millis();
    }
}
