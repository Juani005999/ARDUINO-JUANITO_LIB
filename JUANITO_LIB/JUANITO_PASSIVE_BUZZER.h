/// ---------------------------------------------------------------------
///
/// Objet           : JUANITO_PASSIVE_BUZZER
/// Auteur          : Juanito del Pepito
/// Date            : 04/04/2023
/// Version         : 1.1.0.5
/// Description     : Objet permettant la gestion d'un buzzer passif
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"

// Déclaration des constantes relatives aux notes
#define PASSIVE_BUZZER_SILENT               0
#define PASSIVE_BUZZER_NOTE_DO              33
#define PASSIVE_BUZZER_NOTE_DOd             35
#define PASSIVE_BUZZER_NOTE_RE              37
#define PASSIVE_BUZZER_NOTE_REd             39
#define PASSIVE_BUZZER_NOTE_MI              41
#define PASSIVE_BUZZER_NOTE_FA              44
#define PASSIVE_BUZZER_NOTE_FAd             46
#define PASSIVE_BUZZER_NOTE_SOL             49
#define PASSIVE_BUZZER_NOTE_SOLd            52
#define PASSIVE_BUZZER_NOTE_LA              55
#define PASSIVE_BUZZER_NOTE_LAd             58
#define PASSIVE_BUZZER_NOTE_SI              62

// Déclaration des constantes relatives aux octaves
#define PASSIVE_BUZZER_OCTAVE_1             1
#define PASSIVE_BUZZER_OCTAVE_2             4
#define PASSIVE_BUZZER_OCTAVE_3             8
#define PASSIVE_BUZZER_OCTAVE_4             16
#define PASSIVE_BUZZER_OCTAVE_5             32
#define PASSIVE_BUZZER_OCTAVE_6             64

// Déclaration des constantes relatives aux durée des notes
#define PASSIVE_BUZZER_NOTE_RONDE           16
#define PASSIVE_BUZZER_NOTE_BLANCHE         8
#define PASSIVE_BUZZER_NOTE_NOIRE           4
#define PASSIVE_BUZZER_NOTE_CROCHE          2
#define PASSIVE_BUZZER_NOTE_DBL_CROCHE      1

// Déclaration des constantes internes à l'objet
#define PASSIVE_BUZZER_SOUND_ON_DELAY       75                      // Durée (ms) d'une impulsion pour Sound ON

class JUANITO_PASSIVE_BUZZER
{
  public:
    JUANITO_PASSIVE_BUZZER();
    Init(int pin);
    SoundOn(int freq);
    PlayNote(int note = PASSIVE_BUZZER_NOTE_LA, int octave = PASSIVE_BUZZER_OCTAVE_3);
    PlaySong(int* morceau = NULL, int tempo = 120, int nbNotes = 0);
    StopSong();
    PlayCurrentNote();

  private:
    int _pin;

    int* _morceau;
    int _nbNotes;
    int _tempo;

    int _indexCurrentNote = -1;
    long _chronoCurrentNote;
};