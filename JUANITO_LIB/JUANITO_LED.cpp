#include "JUANITO_LED.h"

/// <summary>
/// Constructeur
/// </summary>
JUANITO_LED::JUANITO_LED ()
{
}

/// <summary>
/// Initialisation du composant
/// </summary>
/// <param name="pin"></param>
/// <param name="isPWM"></param>
/// <param name="minValue"></param>
/// <param name="maxValue"></param>
JUANITO_LED::Init (int pin, bool isPWM, int minValue, int maxValue)
{
  _pin = pin;
  _minValue = minValue;
  _maxValue = maxValue;
  _isPWM = isPWM;
  _state = LED_STATE_OFF;

  // Sécurité sur les bornes min et max
  if (_minValue < 0)
    _minValue = 0;
  if (_maxValue > 255)
    _maxValue = 255;

  // Définition des PIN
  pinMode(_pin, OUTPUT);
}

/// <summary>
/// Positionne la LED à HIGH ou a _maxValue
/// </summary>
/// <param name="setToMaxValue"></param>
/// <param name="interval"></param>
JUANITO_LED::On (bool setToMaxValue, int interval)
{
  // Si auncun intervalle est passé en paramètre, on éxecute direct la commande
  //  Sinon, traitement si plus dans l'intervalle défini
  if ((interval == 0 || millis() > _chronoOnLoop + interval)
      && ((setToMaxValue && _state != LED_STATE_MAX)
          || (!setToMaxValue && _state != LED_STATE_ON)))
  {
    if (setToMaxValue && _isPWM)
    {
      _state = LED_STATE_MAX;
      analogWrite(_pin, _maxValue);
    }
    else
    {
      _state = LED_STATE_ON;
      digitalWrite(_pin, HIGH);
    }

    // Actualisation du chrono
    _chronoOnLoop = millis();
  }
}

/// <summary>
/// Positionne la LED à LOW ou a _minValue
/// </summary>
/// <param name="setToMinValue"></param>
/// <param name="interval"></param>
JUANITO_LED::Off (bool setToMinValue, int interval)
{
  // Si auncun intervalle est passé en paramètre, on éxecute direct la commande
  //  Sinon, traitement si plus dans l'intervalle défini
  if ((interval == 0 || millis() > _chronoOffLoop + interval)
      && ((setToMinValue && _state != LED_STATE_MIN)
          || (!setToMinValue && _state != LED_STATE_OFF)))
  {
    if (setToMinValue && _isPWM)
    {
      _state = LED_STATE_MIN;
      analogWrite(_pin, _minValue);
    }
    else
    {
      _state = LED_STATE_OFF;
      digitalWrite(_pin, LOW);
    }

    // Actualisation du chrono
    _chronoOffLoop = millis();
  }
}

/// <summary>
/// Positionne la LED à la valeur en paramètre
/// </summary>
/// <param name="value"></param>
JUANITO_LED::SetValue (int value)
{
  // Méthode disponible uniquement en mode PWN
  if (_isPWM)
  {
    // On valide les bornes min et max
    int newValue = value;
    // Validation des bornes min et max
    if (newValue < _minValue)
      newValue = _minValue;
    if (newValue > _maxValue)
      newValue = _maxValue;

    _state = LED_STATE_RANDOM;
    _ledDoubleBlinkStage = 0;

    // On envoi la commande
    analogWrite(_pin, newValue);
  }
}

/// <summary>
/// Clignotement simple de la LED
/// </summary>
/// <param name="interval"></param>
JUANITO_LED::Blink(int interval)
{
  // Traitement si plus dans l'intervalle défini
  if (millis() > _chronoBlinkLoop + interval)
  {
    // On change le sens et on positionne la LED
    // En mode PWM, on analogWrite min et max, sinon on digitalWrite 0 et 255
    if (_state == LED_STATE_MAX || _state == LED_STATE_ON)
    {
      if (_isPWM)
      {
        _state = LED_STATE_MIN;
        analogWrite(_pin, _minValue);
      }
      else
      {
        _state = LED_STATE_OFF;
        digitalWrite(_pin, LOW);
      }
    }
    else
    {
      if (_isPWM)
      {
        _state = LED_STATE_MAX;
        analogWrite(_pin, _maxValue);
      }
      else
      {
        _state = LED_STATE_ON;
        digitalWrite(_pin, HIGH);
      }
    }

    // Actualisation du chrono
    _chronoBlinkLoop = millis();
  }
}

/// <summary>
/// Clignotement double de la LED
/// </summary>
/// <param name="interval"></param>
JUANITO_LED::DoubleBlink(int interval)
{
  // Au delà de 5X l'interval, on considère qu'on est sur un nouveau cycle et on repart à l'étape 1
  if (millis() > _chronoDoubleBlinkLoop + (interval * 5))
    _ledDoubleBlinkStage = 0;

  // Traitement si plus dans l'intervalle défini
  if (millis() > _chronoDoubleBlinkLoop + interval)
  {
    // Actualisation du Stage
    if (_ledDoubleBlinkStage >= 8)
      _ledDoubleBlinkStage = 1;
    else
      _ledDoubleBlinkStage++;

    // On positionne la LED
    if (_ledDoubleBlinkStage  == 1 || _ledDoubleBlinkStage == 3)
    {
      if (_isPWM)
      {
        _state = LED_STATE_MAX;
        analogWrite(_pin, _maxValue);
      }
      else
      {
        _state = LED_STATE_ON;
        digitalWrite(_pin, HIGH);
      }
    }    
    else
    {
      if (_isPWM)
      {
        _state = LED_STATE_MIN;
        analogWrite(_pin, _minValue);
      }
      else
      {
        _state = LED_STATE_OFF;
        digitalWrite(_pin, LOW);
      }
    }

    // Actualisation du chrono
    _chronoDoubleBlinkLoop = millis();
  }
}

/// <summary>
/// Oscillation de la LED
/// </summary>
/// <param name="coefInterval"></param>
/// <param name="reverseOscillation"></param>
JUANITO_LED::Oscillate(float coefInterval, bool reverseOscillation)
{
  // Méthode disponible uniquement en mode PWM
  if (_isPWM)
  {
    int currentInterval = LED_DEFAULT_OSCILLATE_CHRONO_INTERVAL * LED_DEFAULT_OSCILLATE_COEF_STD;
    // Si présence d'un coef en paramètre, on applique le nouveau coef d'intervalle
    if (coefInterval != -1)
      currentInterval = LED_DEFAULT_OSCILLATE_CHRONO_INTERVAL + (LED_DEFAULT_OSCILLATE_CHRONO_INTERVAL * coefInterval * 50);

    // Actualisation des flags internes
    _state = LED_STATE_RANDOM;
    _ledDoubleBlinkStage = 0;

    // Traitement si plus dans l'intervalle défini
    if (millis() > _chronoOscillateLoop + currentInterval)
    {
      // Validation des bornes min et max
      if (_ledOscillateCurrentLevel < _minValue)
        _ledOscillateCurrentLevel = _minValue;
      if (_ledOscillateCurrentLevel > _maxValue)
        _ledOscillateCurrentLevel = _maxValue;

      // On positionne la valeur de la LED
      analogWrite(_pin, _ledOscillateCurrentLevel);
      //analogWrite(_pinLedCardLeft, _ledCardLevelMax - _ledCardLevel + _ledCardLevelMin);

      // On update le niveau courant pour le prochain affichage
      if (reverseOscillation)
      {      
        _ledOscillateCurrentLevel = _ledOscillateSensUp ? _ledOscillateCurrentLevel - LED_DEFAULT_OSCILLATE_LEVEL_INTERVAL
                                                          : _ledOscillateCurrentLevel + LED_DEFAULT_OSCILLATE_LEVEL_INTERVAL;
      }
      else
      {      
        _ledOscillateCurrentLevel = _ledOscillateSensUp ? _ledOscillateCurrentLevel + LED_DEFAULT_OSCILLATE_LEVEL_INTERVAL
                                                          : _ledOscillateCurrentLevel - LED_DEFAULT_OSCILLATE_LEVEL_INTERVAL;
      }
      
      // On change le sens si on a atteint les bornes
      if (_ledOscillateCurrentLevel >= _maxValue || _ledOscillateCurrentLevel <= _minValue)
        _ledOscillateSensUp = !_ledOscillateSensUp;

      // Actualisation du chrono
      _chronoOscillateLoop = millis();
    }
  }
}
