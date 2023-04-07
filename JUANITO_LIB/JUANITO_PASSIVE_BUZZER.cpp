#include "JUANITO_PASSIVE_BUZZER.h"

JUANITO_PASSIVE_BUZZER::JUANITO_PASSIVE_BUZZER()
{
}

JUANITO_PASSIVE_BUZZER::Init(int pin)
{
  _pin = pin;
}

JUANITO_PASSIVE_BUZZER::SoundOn(int freq)
{
  // Envoi de l'impulsion
  tone(_pin, freq, PASSIVE_BUZZER_SOUND_ON_DELAY);  

  // Reset du flag de la note en cours de lecture
  _indexCurrentNote = -1;
}

JUANITO_PASSIVE_BUZZER::PlaySong(int* morceau, int tempo, int nbNotes)
{
  if (morceau != NULL && nbNotes > 0)
  {
    _morceau = morceau;
    _tempo = tempo;
    _nbNotes = nbNotes;

    //Serial.println("PlayCurrentNote");
    PlayCurrentNote();
  }
  else
    _indexCurrentNote = -1;
}

JUANITO_PASSIVE_BUZZER::StopSong()
{
  noTone(_pin);
  _morceau = NULL;
  _indexCurrentNote = -1;
}

JUANITO_PASSIVE_BUZZER::PlayCurrentNote()
{
  long currentDuration = 0;
  if (_indexCurrentNote != -1)
    currentDuration = 60000 / _tempo * ((float)((float)(_morceau[(_indexCurrentNote * 3) + 2])) / 4);

  if (_indexCurrentNote == -1 || millis() > _chronoCurrentNote + currentDuration)
  {
    _indexCurrentNote++;
    if (_indexCurrentNote >= _nbNotes)
      _indexCurrentNote = 0;

    int freq = _morceau[_indexCurrentNote * 3] * _morceau[(_indexCurrentNote * 3) + 1];
    long newDuration = 60000 / _tempo * ((float)(_morceau[(_indexCurrentNote * 3) + 2]) / 4);
    if (freq == 0)
      newDuration = 0;

    if (freq != 0)
    {
      // if (_indexCurrentNote != 0)
      //   tone(_pin, 0, 0);

      noTone(_pin);
      //delayMicroseconds(50);
      tone(_pin, freq, newDuration);
    }

    _chronoCurrentNote = millis();
  }
}
