/// ---------------------------------------------------------------------
///
/// Objet           : JUANITO_LCD_I2C
/// Auteur          : Juanito del Pepito
/// Date            : 02/04/2023
/// Version         : 1.1.0.2
/// Description     : Objet permettant la gestion d'un écran LCD (LCD_COLS X LCD_ROWS) sur bus I2C
///                     - Cet objet nécessite l'ajout de la bibliothèque <LiquidCrystal_I2C.h> depuis l'IDE Arduino
///                     - Cet objet est un wrapper de l'objet LiquidCrystal_I2C, avec ajout de quelques fonctionnalités supplémentaire
///                     - Cet objet est destiné aux écrans avec caractères de type Pixels en 5 colonnes sur 8 lignes
///                     - Par défaut, l'écran LCD est positionné sur l'adresse LCD_DEFAULT_ADRESS du bus I2C
///                     - Par défaut, l'écran LCD est de dimmensions LCD_COLS X LCD_ROWS
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

// Déclaration des constantes pour l'association des caractères aux glyphes
#define CARACT_ARROW_UP             0                                       // Caractère associé au glyphe : Flèche haut
#define CARACT_ARROW_DOWN           1                                       // Caractère associé au glyphe : Flèche bas
#define CARACT_ARROW_RIGHT          2                                       // Caractère associé au glyphe : Flèche droite
#define CARACT_ARROW_LEFT           3                                       // Caractère associé au glyphe : Flèche gauche
#define CARACT_SMILEY               4                                       // Caractère associé au glyphe : Smiley
#define CARACT_WINK                 5                                       // Caractère associé au glyphe : Clin d'oeil

// Déclaration des constantes de settings de l'écran LCD
#define LCD_DEFAULT_ADRESS          0x27                                    // Adresse par défaut du LCD sur le bus I2C
#define LCD_DEFAULT_COLS            16                                      // Nombre de colonnes par défaut du LCD
#define LCD_DEFAULT_ROWS            2                                       // Nombre de lignes par défaut du LCD

#define SCROLLING_TEXT_UPDATE       150                                     // Mise à jour du scrolling texte      

/// <summary>
/// JUANITO_LCD_I2C : Objet permettant la gestion d'un écran LCD (LCD_COLS X LCD_ROWS) sur bus I2C
/// </summary>
class JUANITO_LCD_I2C
{
    public:
        JUANITO_LCD_I2C();
        ~JUANITO_LCD_I2C();
        Init(int address = LCD_DEFAULT_ADRESS, int cols = LCD_DEFAULT_COLS, int rows = LCD_DEFAULT_ROWS);
        clear();
        setCursor(int col, int row);
        print(String text);
        println(String text);
        PrintRightJustify (String text, int row);
        PrintScrolling (String text, int row, int startIndex, int width = -1);
        UpArrow();
        DownArrow();
        RightArrow();
        LeftArrow();
        Smiley();
        Wink();

    private:
        // Objet interne
        LiquidCrystal_I2C * _lcd;                                           // Ecran LCD I2C

        // Champs valorisés à l'initialisation de l'objet
        int _address = NAN;
        int _cols = NAN;
        int _rows = NAN;

        // Chrono
        long chronoScrolliongText;

        // Caractères spéciaux : Pixels en colonnes (masque de 5 bits) sur 8 lignes
        byte _upArrow[8] = {                                                // Caractère spécial : flèche haut
            B00100, B01110, B10101, B00100,
            B00100, B00100, B00100, B00000
        };
        byte _downArrow[8] = {                                              // Caractère spécial : flèche bas
            B00100, B00100, B00100, B00100,
            B10101, B01110, B00100, B00000
        };
        byte _rightArrow[8] = {                                             // Caractère spécial : flèche droite
            B00000, B00100, B00010, B11111,
            B00010, B00100, B00000, B00000
        };
        byte _leftArrow[8] = {                                              // Caractère spécial : flèche gauche
            B00000, B00100, B01000, B11111,
            B01000, B00100, B00000, B00000
        };
        byte _smiley[8] = {                                                 // Caractère spécial : Smiley
            B00000, B11011, B11011, B00000,
            B00000, B10001, B01110, B00000
        };
        byte _wink[8] = {                                                   // Caractère spécial : Smiley clin d'oeil
            B00000, B00011, B11011, B00000,
            B00000, B10001, B01110, B00000
        };
};