#include "JUANITO_PASSIVE_BUZZER.h"

/// <summary>
/// Constructeur
/// </summary>
JUANITO_PASSIVE_BUZZER::JUANITO_PASSIVE_BUZZER()
{
}

/// <summary>
/// Initialisation du composant
/// </summary>
/// <param name="pin"></param>
JUANITO_PASSIVE_BUZZER::Init(int pin)
{
  _pin = pin;
}

/// <summary>
/// Déclenche un son à une fréquence
/// Durée du son : PASSIVE_BUZZER_SOUND_ON_DELAY
/// </summary>
/// <param name="freq"></param>
JUANITO_PASSIVE_BUZZER::SoundOn(int freq)
{
  // Envoi de l'impulsion
  tone(_pin, freq, PASSIVE_BUZZER_SOUND_ON_DELAY);

  // Reset du flag de la note en cours de lecture
  _indexCurrentNote = -1;
}

/// <summary>
/// Joue unbe note
/// Durée de la note : PASSIVE_BUZZER_SOUND_ON_DELAY
/// </summary>
/// <param name="note"></param>
/// <param name="octave"></param>
JUANITO_PASSIVE_BUZZER::PlayNote(int note, int octave)
{
    // On calcul la fréquence en fonction de la note et de l'octave
    int freq = note * octave;

    // On joue la note
    if (freq > 0)
        tone(_pin, freq, PASSIVE_BUZZER_SOUND_ON_DELAY);
}

/// <summary>
/// Joue un morceau de façon asynchrone
/// </summary>
/// <param name="morceau"></param>
/// <param name="tempo"></param>
/// <param name="nbNotes"></param>
JUANITO_PASSIVE_BUZZER::PlaySong(int* morceau, int tempo, int nbNotes)
{
  if (morceau != NULL && nbNotes > 0)
  {
    // Si on remarque que _nbNotes ou *_morceau[0] a changé, on reset le morceau
    if ((_nbNotes != 0 && _nbNotes != nbNotes)
        || _morceau[0] != morceau[0])
        _indexCurrentNote = -1;
        _morceau = morceau;
        _tempo = tempo;
        _nbNotes = nbNotes;

    //Serial.println("PlayCurrentNote");
    PlayCurrentNote();
  }
  else
    _indexCurrentNote = -1;
}

/// <summary>
/// Réindexation du morceau en cours sur arret
/// </summary>
JUANITO_PASSIVE_BUZZER::StopSong()
{
  noTone(_pin);
  _morceau = NULL;
  _indexCurrentNote = -1;
}

/// <summary>
/// Joue la note en cours sur le morceau
/// </summary>
JUANITO_PASSIVE_BUZZER::PlayCurrentNote()
{
    // Sécurité sur _indexCurrentNote
    if (_indexCurrentNote >= _nbNotes)
        _indexCurrentNote = 0;

    // Positionnement de l'interval
    long currentInterval = 0;
    if (_indexCurrentNote != -1)
        currentInterval = 60000 / _tempo * ((float)((float)(_morceau[(_indexCurrentNote * 3) + 2])) / 4);

    // On applique un coef d'acceptation
    currentInterval *= 0.95;

    if (_indexCurrentNote == -1 || millis() > _chronoCurrentNote + currentInterval)
    {
        _indexCurrentNote++;
        if (_indexCurrentNote >= _nbNotes)
        _indexCurrentNote = 0;

        // Lecture de la note et de la duration
        int freq = _morceau[_indexCurrentNote * 3] * _morceau[(_indexCurrentNote * 3) + 1];
        long newDuration = 60000 / _tempo * ((float)(_morceau[(_indexCurrentNote * 3) + 2]) / 4);
        if (freq == 0)
            newDuration = 0;

        if (freq != 0)
        {
            // On arrête la précédente note si besoin
            noTone(_pin);
            // On joue la note
            tone(_pin, freq, newDuration);
        }

        // Actualisation du chrono
        _chronoCurrentNote = millis();
    }
}
