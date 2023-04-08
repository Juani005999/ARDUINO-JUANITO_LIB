#include "JUANITO_HCSR04.h"

/// <summary>
/// Constructeur
/// </summary>
JUANITO_HCSR04::JUANITO_HCSR04 ()
{
}

/// <summary>
/// Initialisation du composant
/// </summary>
/// <param name="pinEcho"></param>
/// <param name="pinTrigger"></param>
/// <param name="measureInterval"></param>
JUANITO_HCSR04::Init (int pinEcho, int pinTrigger, int measureInterval)
{
    _pinEcho = pinEcho;
    _pinTrigger = pinTrigger;
    _measureInterval = measureInterval;

    // Sécurité sur seuil mini
    if (_measureInterval < HCSR04_MIN_MEASURE_INTERVAL)
        _measureInterval = HCSR04_MIN_MEASURE_INTERVAL;

    // Définition des PIN
    pinMode(_pinEcho, INPUT);
    pinMode(_pinTrigger, OUTPUT);
}

/// <summary>
/// Renvoi la distance (mm) mesurée par le HCSR04
/// </summary>
/// <param name="temperature"></param>
/// <param name="humidite"></param>
/// <returns></returns>
float JUANITO_HCSR04::Distance(float temperature, float humidite)
{
    // On effectue la mesure si aucune mesure déjà effectuée ou si temps écoulé > interval
    if (isnan(_distance)
        || millis() > _chronoLastDistance + _measureInterval)
    {
        // Sécurité sur l'initialisation des PIN
        if (isnan(_pinEcho) || isnan(_pinTrigger))
            return 0;

        // Pas d'interruptions pendant la mesure
        noInterrupts();

        // Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER
        digitalWrite(_pinTrigger, LOW);
        delayMicroseconds(2);
        digitalWrite(_pinTrigger, HIGH);
        delayMicroseconds(10);
        digitalWrite(_pinTrigger, LOW);
        delayMicroseconds(2);

        // Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe)
        long mesure = pulseIn(_pinEcho, HIGH, HCSR04_MEASURE_TIMEOUT);

        // On rétabli les interruptions
        interrupts();

        // Calcul de la distance
        _distance = mesure / 2.0 * SOUND_SPEED;
        // Calcul de la distance ajustée en fonction de la température et humidité fournis
        if (!isnan(temperature) && !isnan(humidite))
        {
            float speedOfSoundAdjusted = (331.4 + (0.6 * temperature) + (0.0124 * humidite)) / 1000;
            _distanceCorrigee = mesure / 2.0 * speedOfSoundAdjusted;
        }

        // Actualisation du chrono
        _chronoLastDistance = millis();
    }
    // Retour
    if (!isnan(temperature) && !isnan(humidite))
        return _distanceCorrigee;
    return _distance;
}
