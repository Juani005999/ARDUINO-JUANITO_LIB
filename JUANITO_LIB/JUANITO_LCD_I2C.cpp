#include "JUANITO_LCD_I2C.h"

/// <summary>
/// Constructeur
/// </summary>
JUANITO_LCD_I2C::JUANITO_LCD_I2C ()
{
}

/// <summary>
/// Destructeur
/// </summary>
JUANITO_LCD_I2C::~JUANITO_LCD_I2C()
{
    // Free de la mémoire en sortant
    free(_lcd);
}

/// <summary>
/// Initialisation du composant
/// </summary>
/// <param name="address"></param>
/// <param name="cols"></param>
/// <param name="rows"></param>
JUANITO_LCD_I2C::Init (int address, int cols, int rows)
{
    // Valorisations des membres interne
    _address = address;
    _cols = cols;
    _rows = rows;

    // Initialisation objet LiquidCrystal_I2C
    _lcd = new LiquidCrystal_I2C(_address, _cols, _rows);

    // Initialisation écran
    _lcd->init();
    _lcd->backlight();

    // Association des caractères spéciaux
    _lcd->createChar(CARACT_ARROW_UP, _upArrow);
    _lcd->createChar(CARACT_ARROW_DOWN, _downArrow);
    _lcd->createChar(CARACT_ARROW_RIGHT, _rightArrow);
    _lcd->createChar(CARACT_ARROW_LEFT, _leftArrow);
    _lcd->createChar(CARACT_SMILEY, _smiley);
    _lcd->createChar(CARACT_WINK, _wink);
}

/// <summary>
/// Wrapper de LiquidCrystal_I2C::clear
/// </summary>
JUANITO_LCD_I2C::clear ()
{
    _lcd->clear();
}

/// <summary>
/// Wrapper de LiquidCrystal_I2C::setCursor
/// </summary>
/// <param name="col"></param>
/// <param name="row"></param>
JUANITO_LCD_I2C::setCursor (int col, int row)
{
    _lcd->setCursor(col, row);
}

/// <summary>
/// Wrapper de LiquidCrystal_I2C::print
/// </summary>
/// <param name="text"></param>
JUANITO_LCD_I2C::print (String text)
{
    _lcd->print(text);
}

/// <summary>
/// Wrapper de LiquidCrystal_I2C::println
/// </summary>
/// <param name="text"></param>
JUANITO_LCD_I2C::println (String text)
{
    _lcd->println(text);
}

/// <summary>
/// Ecrit le texte aligné à droite
/// </summary>
/// <param name="text"></param>
/// <param name="row"></param>
JUANITO_LCD_I2C::PrintRightJustify (String text, int row)
{
    // Sécurité sur la ligne
    if (row > _rows)
        row = _rows;
    // Positionnement du texte justifié à droite
    if (text.length() > 0)
    {
        // Détermination de la position colonne
        int col = text.length() > _cols ? 0 : _cols - text.length();

        // Positionnement curseur et écriture du texte
        _lcd->setCursor(col, row);
        _lcd->print(text);
    }
}

/// <summary>
/// Ecrit un texte défilant
/// </summary>
/// <param name="text"></param>
/// <param name="row"></param>
/// <param name="startIndex"></param>
/// <param name="width"></param>
JUANITO_LCD_I2C::PrintScrolling (String text, int row, int startIndex, int width)
{
    // On ne fait rien si on est pas dans l'intervalle
    if (millis() > SCROLLING_TEXT_UPDATE)
    {
        if (text.length() > 0 && startIndex > 0 && startIndex < _cols)
        {
            if (width == -1)
            {
                width = _cols - text.length();
            }
            _lcd->setCursor(startIndex, row);
            if (text.length() <= width)
                _lcd->print(text);
            else
            {
            }
        }

        // Actualisation du chrono
        chronoScrolliongText = millis();
    }
}

/// <summary>
/// Ecrit le caractère flèche haut
/// </summary>
JUANITO_LCD_I2C::UpArrow ()
{
    _lcd->write(byte(CARACT_ARROW_UP));
}

/// <summary>
/// Ecrit le caractère flèche bas
/// </summary>
JUANITO_LCD_I2C::DownArrow ()
{
    _lcd->write(byte(CARACT_ARROW_DOWN));
}

/// <summary>
/// Ecrit le caractère flèche droite
/// </summary>
JUANITO_LCD_I2C::RightArrow ()
{
    _lcd->write(byte(CARACT_ARROW_RIGHT));
}

/// <summary>
/// Ecrit le caractère flèche gauche
/// </summary>
JUANITO_LCD_I2C::LeftArrow ()
{
    _lcd->write(byte(CARACT_ARROW_LEFT));
}

/// <summary>
/// Ecrit le caractère Smiley
/// </summary>
JUANITO_LCD_I2C::Smiley ()
{
    _lcd->write(byte(CARACT_SMILEY));
}

/// <summary>
/// Ecrit le caractère Clin d'oeil
/// </summary>
JUANITO_LCD_I2C::Wink ()
{
    _lcd->write(byte(CARACT_WINK));
}