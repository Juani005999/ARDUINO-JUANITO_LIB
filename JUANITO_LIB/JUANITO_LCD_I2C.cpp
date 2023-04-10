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
    free(_chronoDisplayText);
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

    // Création en mémoire d'un tableau de chrono pour chaque caractère d'affichage de l'écran (pour la méthode DisplayText)
    _chronoDisplayText = new long[_rows * _cols];
    for (int i = 0; i < _rows * _cols; i++)
    {
        *(_chronoDisplayText + i) = 0;
    }

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
    _lcd->createChar(CARACT_SMILEY_SAD, _smileySad);
    _lcd->createChar(CARACT_DEGREE, _degree);
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
JUANITO_LCD_I2C::print (char* text)
{
    _lcd->print(text);
}

/// <summary>
/// Wrapper de LiquidCrystal_I2C::println
/// </summary>
/// <param name="text"></param>
JUANITO_LCD_I2C::println (char* text)
{
    _lcd->println(text);
}

/// <summary>
/// Ecrit le texte aligné à droite
/// </summary>
/// <param name="text"></param>
/// <param name="row"></param>
JUANITO_LCD_I2C::PrintRightJustify (char* text, int row)
{
    int textLen = strlen(text);
    // Sécurité sur la ligne
    if (row > _rows)
        row = _rows;
    // Positionnement du texte justifié à droite
    if (textLen > 0)
    {
        // Détermination de la position colonne
        int col = textLen > _cols ? 0 : _cols - textLen;

        // Positionnement curseur et écriture du texte
        _lcd->setCursor(col, row);
        _lcd->print(text);
    }
}

/// <summary>
/// Ecrit un texte sur interval de temps
/// </summary>
/// <param name="text"></param>
/// <param name="row"></param>
/// <param name="interval"></param>
JUANITO_LCD_I2C::DisplayText(char* text, int row, int startIndex, int interval)
{
    int textLen = strlen(text);
    if (textLen > 0)
    {
        int currentRow = row;
        // On valide la valeur du paramètre row
        if (currentRow < 0)
            currentRow = 0;
        if (currentRow > _rows)
            currentRow = _rows;

        // Traitement si plus dans l'interval défini
        if (millis() > *(_chronoDisplayText + ((currentRow * (_cols)) + startIndex)) + interval)
        {
            // Validation de startIndex
            if (startIndex < 0)
                startIndex = 0;
            if (startIndex > _cols - 1)
                startIndex = _cols - 1;
            // Recup de stopIndex
            int stopIndex = _cols - 1;

            // On récupère le texte à écrire sur l'écran
            char* finalText = malloc(textLen + 1);
            if (textLen > _cols - startIndex)
                strncpy(finalText, text, _cols - startIndex);
            else
                strcpy(finalText, text);

            // On repositionne le curseur et on écrit finalText
            _lcd->setCursor(startIndex, currentRow);
            _lcd->print(finalText);
            free(finalText);

            // Actualisation du chrono correspondant à la ligne
            *(_chronoDisplayText + ((currentRow * (_cols)) + startIndex)) = millis();
        }
    }
}

/// <summary>
/// Ecrit un texte défilant
/// </summary>
/// <param name="text"></param>
/// <param name="row"></param>
/// <param name="startIndex"></param>
/// <param name="width"></param>
JUANITO_LCD_I2C::DisplayScrollingText(char* text, int row, int startIndex, int width, int interval)
{
    int textLen = strlen(text);
    if (textLen > 0)
    {
        int currentRow = row;
        // On valide la valeur du paramètre row
        if (currentRow < 0)
            currentRow = 0;
        if (currentRow > _rows)
            currentRow = _rows;

        // Traitement si plus dans l'interval défini
        if (millis() > _chronoScrolliongText + interval)
        {
            // Validation de startIndex
            if (startIndex < 0)
                startIndex = 0;
            if (startIndex > _cols - 1)
                startIndex = _cols - 1;
            // Recup de stopIndex en fonction de width et textLen
            int currentWidth = width == -1 ? _cols - startIndex : width;
            if (currentWidth > _cols)
                currentWidth = _cols;
            if (currentWidth > textLen)
                currentWidth = textLen;

            //// On vérifie si on est toujours de scroller la même chaine, sinon, on reset l'index
            //char bufferCompare[_currentScrollingText.length() + 1];
            //_currentScrollingText.toCharArray(bufferCompare, _currentScrollingText.length() + 1);
            //if (strcmp(text, bufferCompare) != 0)
            //    _currentScrollingIndex = 0;
            // Si l'index actuel est supérieur à la taille de la chaine, on reset le scroll au début
            if (_currentScrollingIndex > textLen)
                _currentScrollingIndex = 0;

            // On recopie la chaine à écrire dans une chaine temporaire avec allocation mémoire
            char* currentString = malloc(currentWidth + 1);
            int borneMax = textLen - _currentScrollingIndex;

            // Validation de la borne max. pour la recopie de string et recopie
            if (borneMax > currentWidth)
                borneMax = currentWidth;
            for (int i = 0; i < borneMax + 1; i++)
                *(currentString + i) = *(text + i + _currentScrollingIndex);

            // Si nécessaire, on complète avec des " "
            while (strlen(currentString) < currentWidth + 1)
                strcat(currentString, " ");

            // On force la caractère de fin de string
            currentString[currentWidth + 1] = '\0';

            // On écrit sur le LCD
            _lcd->setCursor(startIndex, currentRow);
            _lcd->print(currentString);

            // On libère la mémoire allouée localement
            free(currentString);

            // Actualisation des Flags
            _currentScrollingIndex++;
            if (_currentScrollingIndex >= textLen)
                _currentScrollingIndex = 0;
            _currentScrollingText = text;

            // Actualisation du chrono
            _chronoScrolliongText = millis();
        }
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

/// <summary>
/// Ecrit le caractère Degrés
/// </summary>
JUANITO_LCD_I2C::Degree()
{
    _lcd->write(byte(CARACT_DEGREE));
}
