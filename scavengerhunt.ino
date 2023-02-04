#if (defined(__AVR__))
#include <avr\pgmspace.h>
#else
#include <pgmspace.h>
#endif

#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
//#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include <FastLED.h>



/*

 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#define NUM_LEDS    200

#define BRIGHTNESS  30
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define LED_PIN     7

#define NUM_NUIDS 62

// All those LEDs
CRGB leds[NUM_LEDS];

#define DISP_COLOR_BLACK             0
#define DISP_COLOR_BLUE              1
#define DISP_COLOR_ORANGE            2
#define DISP_COLOR_YELLOW            3
#define DISP_COLOR_RED               4
#define DISP_COLOR_GREEN             5
#define DISP_COLOR_CYAN              6
#define DISP_COLOR_PURPLE            7
#define DISP_COLOR_WHITE             8
#define DISP_COLOR_PINK              9
#define DISP_COLOR_CRIMSON          10
#define DISP_COLOR_CORAL            11
#define DISP_COLOR_DARK_BLUE        12
#define DISP_COLOR_GOLD             13
#define DISP_COLOR_DARK_CYAN        14 
#define DISP_COLOR_DARK_GREEN       15
#define DISP_COLOR_DARK_RED         16
#define DISP_COLOR_DARK_MAGENTA     17
#define DISP_COLOR_DARK_ORANGE      18
#define DISP_COLOR_DARK_VIOLET      19
#define DISP_COLOR_DEEP_PINK        20
#define DISP_COLOR_DARK_TURQ        21
#define DISP_COLOR_GOLDENROD        22
#define DISP_COLOR_DEEP_SKY_BLUE    23
#define DISP_COLOR_FUCHSIA          24
#define DISP_COLOR_LIGHT_SEA_GREEN  25
#define DISP_COLOR_INDIGO           26
#define DISP_COLOR_LAWN_GREEN       27
#define DISP_COLOR_MAGENTA          28
#define DISP_COLOR_LIME             29
#define DISP_COLOR_MAROON           30
#define DISP_COLOR_MED_VIOLET_RED   31
#define DISP_COLOR_MED_BLUE         32
#define DISP_COLOR_MED_SPRING_GREEN 33
#define DISP_COLOR_ORANGE_RED       34
#define DISP_COLOR_NAVY             35
#define DISP_COLOR_OLIVE            36
#define DISP_COLOR_SALMON           37
#define DISP_COLOR_PERU             38
#define DISP_COLOR_ROYAL_BLUE       39
#define DISP_COLOR_TEAL             40
#define DISP_COLOR_SEA_GREEN        41
#define DISP_COLOR_SPRING_GREEN     42
#define DISP_COLOR_FAIRY_LIGHT      43
#define DISP_COLOR_VIOLET           44
#define DISP_COLOR_MINT             45
#define DISP_COLOR_BROWN            46
#define NUM_DISP_COLORS             47

const CRGB numToColor[NUM_DISP_COLORS] = 
{CRGB::Black, CRGB::Blue, CRGB::Orange, CRGB::Yellow, CRGB::Red, CRGB::Green, CRGB::Cyan, CRGB::Purple,
 CRGB::White, 0xFF1493, CRGB::Crimson, CRGB::Coral, CRGB::DarkBlue, CRGB::Gold, CRGB::DarkCyan, CRGB::DarkGreen,
 CRGB::DarkRed, CRGB::DarkMagenta, CRGB::DarkOrange, CRGB::DarkViolet, CRGB::DeepPink, CRGB::DarkTurquoise, CRGB::Goldenrod, CRGB::DeepSkyBlue, 
 CRGB::Fuchsia, CRGB::LightSeaGreen, CRGB::Indigo, CRGB::LawnGreen, CRGB::Magenta, CRGB::Lime, CRGB::Maroon, CRGB::MediumVioletRed, 
 CRGB::MediumBlue, CRGB::MediumSpringGreen, CRGB::OrangeRed, CRGB::Navy, CRGB::Olive, CRGB::Salmon, CRGB::Peru, CRGB::RoyalBlue,
 CRGB::Teal, CRGB::SeaGreen, CRGB::SpringGreen, CRGB::FairyLight, CRGB::Violet, 0x28a828, CRGB::Brown};


const unsigned int PROGMEM character_nuid_one[NUM_NUIDS] =
{224, // Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81
 192, // Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81
 192, // Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81
 54, // Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51
 192, // Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81
 192, // Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81
 192, // Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81
 64, // Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81
 48, // Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81
 224, // Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81
 160, // Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81
 64, // Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81
 64, // Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81
 160, // Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81
 32, // Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81
 32, // Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81
 192, // Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81
 240, // Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81
 176, // Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81
 128, // Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81
 144, // Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81
 32, // Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81
 160, // Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81
 24, // Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81
 48, // Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81
 48, // Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81
 48, // Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81
 48, // Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81
 48, // Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81
 80, // Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81
 192, // Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81
 208, // Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81
 208, // Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81
 208, // Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81
 224, // Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81
 224, // Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81
 224, // Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81
 80, // Aurora (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81
 128, // Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81
 214, // Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51
 06, // Rapunzel 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51
 118, // Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51
 230, // Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51
 166, // Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51
 150, // Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51
 150, // Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51
 38, // Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51
 22, // Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51
 214, // Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51
 198, // Elsa Frozen 2 In hex:  C6 72 3F 33 In dec:  198 114 63 51
 214, // Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51
 54, // PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51
 198, // Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51
 246, // Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51
 192, // Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81
 214, // Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51
 38, // Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51
 160, // Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81
 160, // Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81
 160,  // Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81
 28, // Fia In hex:  1C 37 72 17 In dec:  28 55 114 23
 60 // Kaia In hex:  3C 79 71 17 In dec:  60 121 113 23
 };

const unsigned int PROGMEM character_nuid_two[NUM_NUIDS] =
{122, // Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81
 201, // Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81
 138, // Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81
 192, // Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51
 154, // Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81
 170, // Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81
 186, // Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81
 215, // Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81
 231, // Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81
 246, // Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81
 04, // Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81
 88, // Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81
 102, // Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81
 196, // Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81
 210, // Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81
 225, // Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81
 205, // Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81
 189, // Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81
 221, // Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81
 05, // Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81
 05, // Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81
 248, // Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81
 232, // Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81
 221, // Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81
 196, // Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81
 180, // Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81
 166, // Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81
 150, // Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81
 134, // Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81
 118, // Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81
 97, // Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81
 81, // Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81
 65, // Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81
 49, // Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81
 33, // Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81
 17, // Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81
 252, // Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81
 237, // Aurora (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81
 221, // Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81
 73, // Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51
 179, // Rapunzel 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51
 31, // Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51
 26, // Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51
 144, // Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51
 22, // Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51
 26, // Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51
 176, // Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51
 70, // Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51
 228, // Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51
 114, // Elsa Frozen 2 In hex:  C6 72 3F 33 In dec:  198 114 63 51
 188, // Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51
 42, // PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51
 78, // Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51
 221, // Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51
 116, // Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81
 242, // Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51
 142, // Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51
 180, // Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81
 148, // Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81
 164,  // Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81
 55, // Fia In hex:  1C 37 72 17 In dec:  28 55 114 23
 121 // Kaia In hex:  3C 79 71 17 In dec:  60 121 113 23
 };

const unsigned int PROGMEM character_nuid_three[NUM_NUIDS] =
{183, // Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81
 183, // Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81
 183, // Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81
 65, // Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51
 183, // Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81
 183, // Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81
 183, // Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81
 183, // Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81
 183, // Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81
 183, // Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81
 184, // Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81
 183, // Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81
 183, // Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81
 183, // Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81
 183, // Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81
 183, // Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81
 182, // Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81
 182, // Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81
 182, // Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81
 184, // Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81
 184, // Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81
 183, // Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81
 183, // Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81
 183, // Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81
 183, // Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81
 183, // Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81
 183, // Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81
 183, // Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81
 183, // Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81
 183, // Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81
 183, // Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81
 183, // Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81
 183, // Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81
 183, // Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81
 183, // Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81
 183, // Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81
 182, // Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81
 182, // Aurora (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81
 182, // Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81
 57, // Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51
 56, // Rapunzel 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51
 56, // Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51
 55, // Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51
 54, // Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51
 55, // Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51
 56, // Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51
 56, // Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51
 57, // Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51
 57, // Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51
 63, // Elsa Frozen 2 In hex:  C6 72 3F 33 In dec:  198 114 63 51
 65, // Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51
 65, // PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51
 66, // Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51
 66, // Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51
 183, // Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81
 63, // Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51
 64, // Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51
 183, // Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81
 183, // Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81
 183,  // Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81
 114, // Fia In hex:  1C 37 72 17 In dec:  28 55 114 23
 113 // Kaia In hex:  3C 79 71 17 In dec:  60 121 113 23
 };

const unsigned int PROGMEM character_nuid_four[NUM_NUIDS] =
{81, // Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81
 81, // Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81
 81, // Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81
 51, // Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51
 81, // Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81
 81, // Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81
 81, // Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81
 81, // Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81
 81, // Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81
 81, // Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81
 81, // Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81
 81, // Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81
 81, // Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81
 81, // Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81
 81, // Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81
 81, // Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81
 81, // Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81
 81, // Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81
 81, // Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81
 81, // Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81
 81, // Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81
 81, // Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81
 81, // Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81
 81, // Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81
 81, // Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81
 81, // Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81
 81, // Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81
 81, // Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81
 81, // Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81
 81, // Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81
 81, // Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81
 81, // Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81
 81, // Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81
 81, // Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81
 81, // Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81
 81, // Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81
 81, // Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81
 81, // Aurora (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81
 81, // Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81
 51, // Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51
 51, // Rapunzel 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51
 51, // Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51
 51, // Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51
 51, // Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51
 51, // Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51
 51, // Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51
 51, // Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51
 51, // Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51
 51, // Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51
 51, // Elsa Frozen 2 In hex:  C6 72 3F 33 In dec:  198 114 63 51
 51, // Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51
 51, // PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51
 51, // Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51
 51, // Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51
 81, // Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81
 51, // Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51
 51, // Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51
 81, // Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81
 81, // Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81
 81,  // Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81
 23, // Fia In hex:  1C 37 72 17 In dec:  28 55 114 23
 23 // Kaia In hex:  3C 79 71 17 In dec:  60 121 113 23
 };

 const unsigned int PROGMEM character_colors[NUM_NUIDS + 8][3] =
{{DISP_COLOR_BLUE, DISP_COLOR_YELLOW, DISP_COLOR_BLUE},                      // aa Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81
 {DISP_COLOR_BLUE, DISP_COLOR_YELLOW, DISP_COLOR_BLUE},                      // ab Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81
 {DISP_COLOR_DARK_RED, DISP_COLOR_WHITE, DISP_COLOR_YELLOW},                 // ac Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81
 {DISP_COLOR_FAIRY_LIGHT, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},     // ad Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // ae Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81
 {DISP_COLOR_MED_BLUE, DISP_COLOR_PINK, DISP_COLOR_WHITE},                   // af Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81
 {DISP_COLOR_SPRING_GREEN, DISP_COLOR_SPRING_GREEN, DISP_COLOR_ORANGE_RED},  // ag Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81
 {DISP_COLOR_TEAL, DISP_COLOR_TEAL, DISP_COLOR_TEAL},                        // ah Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81
 {DISP_COLOR_DEEP_SKY_BLUE, DISP_COLOR_DARK_BLUE, DISP_COLOR_RED},           // ai Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_WHITE},                   // aj Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81
 {DISP_COLOR_DARK_RED, DISP_COLOR_YELLOW, DISP_COLOR_GREEN},                 // ak Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81
 {DISP_COLOR_PURPLE, DISP_COLOR_YELLOW, DISP_COLOR_PURPLE},                  // al Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81
 {DISP_COLOR_PINK, DISP_COLOR_YELLOW, DISP_COLOR_PINK},                      // am Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81
 {DISP_COLOR_WHITE, DISP_COLOR_ORANGE, DISP_COLOR_WHITE},                    // an Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81
 {DISP_COLOR_MINT, DISP_COLOR_MINT, DISP_COLOR_SEA_GREEN},                   // ao Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81
 {DISP_COLOR_DARK_RED, DISP_COLOR_BLUE, DISP_COLOR_YELLOW},                  // ap Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81
 {DISP_COLOR_DARK_GREEN, DISP_COLOR_YELLOW, DISP_COLOR_DARK_RED},            // aq Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81
 {DISP_COLOR_WHITE, DISP_COLOR_PINK, DISP_COLOR_WHITE},                      // ar Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81
 {DISP_COLOR_BLUE, DISP_COLOR_BLUE, DISP_COLOR_BLUE},                        // as Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81
 {DISP_COLOR_YELLOW, DISP_COLOR_RED, DISP_COLOR_WHITE},                      // at Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81
 {DISP_COLOR_PURPLE, DISP_COLOR_YELLOW, DISP_COLOR_PINK},                    // au Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81
 {DISP_COLOR_YELLOW, DISP_COLOR_WHITE, DISP_COLOR_BLUE},                     // av Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81
 {DISP_COLOR_DARK_BLUE,DISP_COLOR_ORANGE, DISP_COLOR_DARK_GREEN},            // aw Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81
 {DISP_COLOR_PINK, DISP_COLOR_WHITE, DISP_COLOR_PINK},                       // ax Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81
 {DISP_COLOR_MINT, DISP_COLOR_WHITE, DISP_COLOR_PINK},                       // ay Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81
 {DISP_COLOR_GREEN, DISP_COLOR_PURPLE, DISP_COLOR_DARK_RED},                 // az Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},       // ba Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81
 {DISP_COLOR_DARK_RED, DISP_COLOR_BLUE, DISP_COLOR_YELLOW},                  // bb Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81
 {DISP_COLOR_DEEP_SKY_BLUE, DISP_COLOR_DARK_BLUE, DISP_COLOR_RED},           // bc Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81
 {DISP_COLOR_BLUE, DISP_COLOR_WHITE, DISP_COLOR_BLUE},                       // bd Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81
 {DISP_COLOR_PURPLE, DISP_COLOR_YELLOW, DISP_COLOR_PURPLE},                  // be Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81
 {DISP_COLOR_FAIRY_LIGHT, DISP_COLOR_ORANGE_RED, DISP_COLOR_TEAL},           // bf Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81
 {DISP_COLOR_TEAL, DISP_COLOR_TEAL, DISP_COLOR_TEAL},                        // bg Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81
 {DISP_COLOR_SPRING_GREEN, DISP_COLOR_SPRING_GREEN, DISP_COLOR_ORANGE_RED},  // bh Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_CYAN, DISP_COLOR_DARK_VIOLET},       // bi Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81
 {DISP_COLOR_BLUE, DISP_COLOR_YELLOW, DISP_COLOR_BLUE},                      // bj Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81
 {DISP_COLOR_MINT, DISP_COLOR_MINT, DISP_COLOR_SEA_GREEN},                   // bk Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81
 {DISP_COLOR_PINK, DISP_COLOR_YELLOW, DISP_COLOR_PINK},                      // bl Aurora 2 (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81
 {DISP_COLOR_FAIRY_LIGHT, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},     // bm Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81
 {DISP_COLOR_RED, DISP_COLOR_GREEN, DISP_COLOR_BLUE},                        // bn Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51
 {DISP_COLOR_DARK_VIOLET, DISP_COLOR_WHITE, DISP_COLOR_YELLOW},              // bo Aurora 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51
 {DISP_COLOR_PURPLE, DISP_COLOR_MINT, DISP_COLOR_PURPLE},                    // bp Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51
 {DISP_COLOR_PURPLE, DISP_COLOR_MINT, DISP_COLOR_PURPLE},                    // bq Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51
 {DISP_COLOR_PINK, DISP_COLOR_YELLOW, DISP_COLOR_PINK},                      // br Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51
 {DISP_COLOR_BROWN, DISP_COLOR_YELLOW, DISP_COLOR_RED},                      // bs Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},       // bt Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_CYAN, DISP_COLOR_DARK_VIOLET},       // bu Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51
 {DISP_COLOR_TEAL, DISP_COLOR_BLACK, DISP_COLOR_FAIRY_LIGHT},                // bv Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51
 {DISP_COLOR_WHITE, DISP_COLOR_ORANGE, DISP_COLOR_WHITE},                    // bw Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},       // bx Elsa Frozen 2 (49) In hex:  C6 72 3F 33 In dec:  198 114 63 51
 {DISP_COLOR_RED, DISP_COLOR_GREEN, DISP_COLOR_RED},                         // by Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51
 {DISP_COLOR_RED, DISP_COLOR_GREEN, DISP_COLOR_BLUE},                        // bz PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51
 {DISP_COLOR_GREEN, DISP_COLOR_GREEN, DISP_COLOR_GREEN},                     // ca Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51
 {DISP_COLOR_GREEN, DISP_COLOR_GREEN, DISP_COLOR_GREEN},                     // cb Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51
 {DISP_COLOR_BLUE, DISP_COLOR_WHITE, DISP_COLOR_RED},                        // cc Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81
 {DISP_COLOR_BLUE, DISP_COLOR_WHITE, DISP_COLOR_RED},                        // cd Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51
 {DISP_COLOR_BLUE, DISP_COLOR_WHITE, DISP_COLOR_RED},                        // ce Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51
 {DISP_COLOR_PINK, DISP_COLOR_BLUE, DISP_COLOR_GREEN},                       // cf Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81
 {DISP_COLOR_PINK, DISP_COLOR_BLUE, DISP_COLOR_GREEN},                       // cg Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81
 {DISP_COLOR_PINK, DISP_COLOR_BLUE, DISP_COLOR_GREEN},                       // ch Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81
 {DISP_COLOR_WHITE, DISP_COLOR_RED, DISP_COLOR_WHITE},                       // ci Fia 1 In hex:  1C 37 72 17 In dec:  28 55 114 23
 {DISP_COLOR_PURPLE, DISP_COLOR_PURPLE, DISP_COLOR_PINK},                    // cj Kaia 1 In hex:  3C 79 71 17 In dec:  60 121 113 23
 {DISP_COLOR_PURPLE, DISP_COLOR_ORANGE, DISP_COLOR_GREEN},                   // ck Fia 2 In hex:  1C 37 72 17 In dec:  28 55 114 23
 {DISP_COLOR_DARK_RED, DISP_COLOR_BLUE, DISP_COLOR_YELLOW},                  // cl Kaia 2 In hex:  3C 79 71 17 In dec:  60 121 113 23
 {DISP_COLOR_PINK, DISP_COLOR_PINK, DISP_COLOR_PINK},                        // cm Fia 3 In hex:  1C 37 72 17 In dec:  28 55 114 23
 {DISP_COLOR_YELLOW, DISP_COLOR_PINK, DISP_COLOR_DARK_TURQ},                 // cn Kaia 3 In hex:  3C 79 71 17 In dec:  60 121 113 23
 {DISP_COLOR_PINK, DISP_COLOR_PINK, DISP_COLOR_PINK},                        // co Fia 4 In hex:  1C 37 72 17 In dec:  28 55 114 23
 {DISP_COLOR_PURPLE, DISP_COLOR_PURPLE, DISP_COLOR_PURPLE},                  // cp Kaia 4 In hex:  3C 79 71 17 In dec:  60 121 113 23
 {DISP_COLOR_GREEN, DISP_COLOR_GREEN, DISP_COLOR_GREEN},                     // cq Fia 5 In hex:  1C 37 72 17 In dec:  28 55 114 23
 {DISP_COLOR_NAVY, DISP_COLOR_DEEP_SKY_BLUE, DISP_COLOR_PINK}                // cr Kaia 5 In hex:  3C 79 71 17 In dec:  60 121 113 23
 };

 

#define TIME_STEP 500
#define SONG_LENGTH 24

#define KNIGHT_PIN     8

#define SS_PIN 10
#define RST_PIN 9

/* Song data */
//const unsigned char PROGMEM songData[SONG_LENGTH_16] =
//  {0x8F,0x00,0x34,0xC3,0x28,0x00,0xEC,0x24};

const unsigned char songData[SONG_LENGTH] =
  {1,1,1,1,1,1,1,0,
   0,0,0,0,0,1,0,1,
   0,0,1,1,0,0,1,1};


//SoftwareSerial mySoftwareSerial(6, 7); // RX, TX
//DFRobotDFPlayerMini myDFPlayer;

byte readCard[4];
String My_ID = "********";  
String ID = "";

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance



void display_leds_three(unsigned int color_num_1, unsigned int color_num_2, unsigned int color_num_3)
{
   unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
  FastLED.show();
  delay(100);
  
  for(i = 0; i < (NUM_LEDS / 2); i++)
  {
    if (i < (NUM_LEDS / 3))
    {
      /* color from each end, one every .1 s */
      leds[i] = numToColor[color_num_1];
      leds[NUM_LEDS - 1 - i] = numToColor[color_num_3];
    }
    else
    {
      /* color from each end, one every .1 s */
      leds[i] = numToColor[color_num_2];
      leds[NUM_LEDS - 1 - i] = numToColor[color_num_2];
    }
    FastLED.show();
    delay(100);
  }
  
}

void setup()
{

  pinMode(KNIGHT_PIN, OUTPUT);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  
  pinMode(LED_PIN, OUTPUT);

  //SPI.begin(); // Init SPI bus

 

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();    // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();

 // mySoftwareSerial.begin(9600);
//  delay(2000);

  //Serial.println();
//  Serial.println(F("DFRobot DFPlayer Mini Demo"));
//  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  //if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
  //  Serial.println(F("Unable to begin:"));
  //  Serial.println(F("1.Please recheck the connection!"));
  //  Serial.println(F("2.Please insert the SD card!"));
  //  while(true);
 // }
 // Serial.println(F("DFPlayer Mini online."));

//  myDFPlayer.volume(15);  //Set volume value. From 0 to 30
}


void play_song()
{
  for(int i = 0; i < SONG_LENGTH; i++)
  {
    //Serial.println("I");
    //for(int j = 0; j < 8; j++)
    {
      if (songData[i] > 0)
      {
        digitalWrite(KNIGHT_PIN, HIGH);
        Serial.println("HIGH");
      }
      else
      {
        digitalWrite(KNIGHT_PIN, LOW);
        Serial.println("LOW");
      }
      delay(TIME_STEP);
    }
  }
  digitalWrite(KNIGHT_PIN, LOW);
  Serial.println("LOW");
  
}

boolean getID() 
{
  if ( ! mfrc522.PICC_IsNewCardPresent()) { 
  return false;
  Serial.println("GOT HERE");
  delay(100);
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { 
  return false;
  }
  ID = "";
  for ( uint8_t i = 0; i < 4; i++) { 
  //readCard[i] = mfrc522.uid.uidByte[i];
  ID.concat(String(mfrc522.uid.uidByte[i], HEX)); 
  }
  ID.toUpperCase();
  mfrc522.PICC_HaltA(); 
  return true;
}

//unsigned int track_num_current = 0;

void loop()
{

  while (getID()) 
  {
    play_song();
  }

  display_leds_three(DISP_COLOR_WHITE, DISP_COLOR_PINK, DISP_COLOR_BLUE);
  

  delay(5000);


  
  //if(myDFPlayer.readCurrentFileNumber() == 0) //read current play file number
  //  digitalWrite(RECORD_PIN, LOW);

  // myDFPlayer.playMp3Folder(matched_character);

  //    if (myDFPlayer.available()) 
  //    {
  //      printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  //    } 



  //Serial.println(myDFPlayer.readState()); //read mp3 state
  //Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
  



  
}



/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
