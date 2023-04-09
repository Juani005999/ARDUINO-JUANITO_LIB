/// ---------------------------------------------------------------------
///
/// Objet           : JUANITO_LCD_I2C
/// Auteur          : Juanito del Pepito
/// Date            : 02/04/2023
/// Version         : 1.1.0.5
/// Description     : Objet permettant la gestion d'un écran LCD (LCD_COLS X LCD_ROWS) sur bus I2C
///                     - Cet objet nécessite l'ajout de la bibliothèque <LiquidCrystal_I2C.h> depuis l'IDE Arduino
///                     - Cet objet est un wrapper de l'objet LiquidCrystal_I2C, avec ajout de quelques fonctionnalités supplémentaire
///                     - Cet objet est destiné aux écrans avec caractères de type Pixels en 5 colonnes sur 8 lignes
///                     - Par défaut, l'écran LCD est positionné sur l'adresse LCD_DEFAULT_ADDRESS du bus I2C
///                     - Par défaut, l'écran LCD est de dimmensions LCD_DEFAULT_COLS X LCD_DEFAULT_ROWS
///
/// ---------------------------------------------------------------------
// Librairies
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

// Déclaration des constantes pour l'association des caractères aux glyphes
#define CARACT_ARROW_UP                         1                           // Caractère associé au glyphe : Flèche haut
#define CARACT_ARROW_DOWN                       2                           // Caractère associé au glyphe : Flèche bas
#define CARACT_ARROW_RIGHT                      3                           // Caractère associé au glyphe : Flèche droite
#define CARACT_ARROW_LEFT                       4                           // Caractère associé au glyphe : Flèche gauche
#define CARACT_SMILEY                           5                           // Caractère associé au glyphe : Smiley
#define CARACT_WINK                             6                           // Caractère associé au glyphe : Clin d'oeil
#define CARACT_SMILEY_SAD                       10                         // Caractère associé au glyphe : Smiley triste
#define CARACT_DEGREE                           7                           // Caractère associé au glyphe : Degrés

// Déclaration des constantes de settings de l'écran LCD
#define LCD_DEFAULT_ADDRESS                     0x27                        // Adresse par défaut du LCD sur le bus I2C
#define LCD_DEFAULT_COLS                        16                          // Nombre de colonnes par défaut du LCD
#define LCD_DEFAULT_ROWS                        2                           // Nombre de lignes par défaut du LCD

#define LCD_DISPLAY_DEFAULT_INTERVAL            150                         // Taux de rafraichissement (ms) par défaut pour l'affichage par la méthode DisplayText

#define LCD_DISPLAY_DEFAULT_SCROLL_INTERVAL     500                         // Mise à jour du scrolling texte      

/// <summary>
/// JUANITO_LCD_I2C : Objet permettant la gestion d'un écran LCD (LCD_COLS X LCD_ROWS) sur bus I2C
/// </summary>
class JUANITO_LCD_I2C
{
    public:
        JUANITO_LCD_I2C();
        ~JUANITO_LCD_I2C();
        Init(int address = LCD_DEFAULT_ADDRESS, int cols = LCD_DEFAULT_COLS, int rows = LCD_DEFAULT_ROWS);
        clear();
        setCursor(int col, int row);
        print(char* text);
        println(char* text);
        PrintRightJustify(char* text, int row);
        DisplayText(char* text, int row = 0, int startIndex = 0, int interval = LCD_DISPLAY_DEFAULT_INTERVAL);
        DisplayScrollingText(char* text, int row = 0, int startIndex = 0, int width = -1, int interval = LCD_DISPLAY_DEFAULT_SCROLL_INTERVAL);
        UpArrow();
        DownArrow();
        RightArrow();
        LeftArrow();
        Smiley();
        Wink();
        Degree();
        Percent();

    private:
        // Objet interne
        LiquidCrystal_I2C * _lcd;                                           // Ecran LCD I2C

        // Champs valorisés à l'initialisation de l'objet
        int _address = NAN;
        int _cols = NAN;
        int _rows = NAN;

        // Chrono
        long* _chronoDisplayText;
        long _chronoScrolliongText;
        String _currentScrollingText;
        int _currentScrollingIndex;

        // Caractères spéciaux : Pixels en colonnes (masque de 5 bits) sur 8 lignes
        byte _upArrow[8] = {                                                // Caractère spécial : flèche haut
            B00100,
            B01110,
            B10101,
            B00100,
            B00100,
            B00100,
            B00100,
            B00000
        };
        byte _downArrow[8] = {                                              // Caractère spécial : flèche bas
            B00100,
            B00100,
            B00100,
            B00100,
            B10101,
            B01110,
            B00100,
            B00000
        };
        byte _rightArrow[8] = {                                             // Caractère spécial : flèche droite
            B00000,
            B00100,
            B00010,
            B11111,
            B00010,
            B00100,
            B00000,
            B00000
        };
        byte _leftArrow[8] = {                                              // Caractère spécial : flèche gauche
            B00000,
            B00100,
            B01000,
            B11111,
            B01000,
            B00100,
            B00000,
            B00000
        };
        byte _smiley[8] = {                                                 // Caractère spécial : Smiley
            B00000,
            B11011,
            B11011,
            B00000,
            B00000,
            B10001,
            B01110,
            B00000
        };
        byte _wink[8] = {                                                   // Caractère spécial : Smiley clin d'oeil
            B00000,
            B00011,
            B11011,
            B00000,
            B00000,
            B10001,
            B01110,
            B00000
        };
        byte _smileySad[8] = {                                              // Caractère spécial : Smiley triste
            B00000,
            B11011,
            B11011,
            B00000,
            B00000,
            B01110,
            B10001,
            B00000
        };
        byte _degree[8] = {                                                 // Caractère spécial : Degrés
            B00110,
            B01001,
            B01001,
            B00110,
            B00000,
            B00000,
            B00000,
            B00000
        };
};