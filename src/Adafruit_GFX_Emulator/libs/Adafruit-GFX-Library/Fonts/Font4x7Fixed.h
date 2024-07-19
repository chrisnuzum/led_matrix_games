/**
** 4x7 font
*
*
* Author Rob Jennings
*/
const uint8_t Font4x7FixedBitmaps[] PROGMEM = {
  0xFA, 0xB4, 0x55, 0xF5, 0xF5, 0x50, 0x5F, 0x77, 0xD0, 0x00, 0x94, 0xA9,
  0x48, 0x00, 0x4A, 0xA5, 0xAB, 0x60, 0xD8, 0x00, 0x6A, 0xA4, 0x00, 0x95,
  0x58, 0x00, 0xAA, 0x80, 0x5D, 0x00, 0xD0, 0xE0, 0xF0, 0x25, 0x25, 0x20,
  0x00, 0x76, 0xDB, 0x70, 0x00, 0x59, 0x24, 0xB8, 0x00, 0xE4, 0xF9, 0x38,
  0x00, 0xE4, 0xB2, 0x78, 0x00, 0xB6, 0xF2, 0x48, 0x00, 0xF2, 0x72, 0x78,
  0x00, 0xF2, 0x7B, 0x78, 0x00, 0xE4, 0x92, 0x48, 0x00, 0xF6, 0xFB, 0x78,
  0x00, 0xF6, 0xF2, 0x78, 0x00, 0xD8, 0x00, 0xDC, 0x00, 0x2A, 0x22, 0x00,
  0xE3, 0x80, 0x88, 0xA8, 0x00, 0x54, 0xA4, 0x10, 0x00, 0x69, 0x9B, 0xB8,
  0x70, 0x56, 0xFB, 0x68, 0x00, 0xD6, 0xFB, 0x70, 0x00, 0x56, 0x49, 0x50,
  0x00, 0xD6, 0xDB, 0x70, 0x00, 0xF2, 0x69, 0x38, 0x00, 0xF2, 0x69, 0x20,
  0x00, 0x69, 0x8B, 0x99, 0x60, 0xB6, 0xFB, 0x68, 0x00, 0xE9, 0x24, 0xB8,
  0x00, 0x24, 0x93, 0x78, 0x00, 0x96, 0xEB, 0x68, 0x00, 0x92, 0x49, 0x38,
  0x00, 0x9F, 0xF9, 0x99, 0x90, 0x99, 0xDF, 0xB9, 0x90, 0x56, 0xDB, 0x50,
  0x00, 0xD6, 0xE9, 0x20, 0x00, 0x56, 0xDB, 0xD8, 0x00, 0xD6, 0xED, 0x68,
  0x00, 0x72, 0x22, 0x70, 0x00, 0xE9, 0x24, 0x90, 0x00, 0xB6, 0xDB, 0x78,
  0x00, 0xB6, 0xDB, 0x50, 0x00, 0x99, 0x99, 0xFF, 0x90, 0xB6, 0xAB, 0x68,
  0x00, 0xB6, 0xA4, 0x90, 0x00, 0xE4, 0xA9, 0x38, 0x00, 0xF2, 0x49, 0x38,
  0x00, 0x91, 0x24, 0x48, 0x00, 0xE4, 0x92, 0x78, 0x00, 0x54, 0x00, 0xE0,
  0x90, 0x65, 0xD6, 0x00, 0x93, 0x5B, 0x70, 0x00, 0x72, 0x46, 0x00, 0x25,
  0xDB, 0x58, 0x00, 0x57, 0xC6, 0x00, 0x69, 0x74, 0x90, 0x00, 0x75, 0x9E,
  0x00, 0x93, 0x5B, 0x68, 0x00, 0xBE, 0x45, 0x5C, 0x00, 0x92, 0xDD, 0x68,
  0x00, 0xFE, 0x00, 0xFF, 0x99, 0x90, 0xD6, 0xDA, 0x00, 0x56, 0xD4, 0x00,
  0xD7, 0x48, 0x00, 0x75, 0x92, 0x00, 0xD6, 0x48, 0x00, 0x71, 0x1C, 0x00,
  0x4B, 0xA4, 0x98, 0x00, 0xB6, 0xDE, 0x00, 0xB6, 0xD4, 0x00, 0x99, 0x9F,
  0x90, 0xB5, 0x5A, 0x00, 0xB5, 0x9C, 0x00, 0xE5, 0x4E, 0x00, 0x29, 0x44,
  0x88, 0x00, 0xFE, 0x00, 0x89, 0x14, 0xA0, 0x00, 0xCC, 0x00
};

const GFXglyph Font4x7FixedGlyphs[] PROGMEM = {
  {     0,   0,   1,   2,    0,    0 }   // ' '
 ,{     0,   1,   7,   2,    0,   -7 }   // '!'
 ,{     1,   3,   2,   4,    0,   -7 }   // '"'
 ,{     2,   4,   7,   5,    0,   -7 }   // '#'
 ,{     6,   3,   7,   4,    0,   -7 }   // '$'
 ,{    10,   3,   7,   4,    0,   -7 }   // '%'
 ,{    14,   4,   7,   5,    0,   -7 }   // '&'
 ,{    18,   2,   3,   3,    0,   -7 }   // '''
 ,{    20,   2,   7,   3,    0,   -7 }   // '('
 ,{    23,   2,   7,   3,    0,   -7 }   // ')'
 ,{    26,   3,   3,   4,    0,   -5 }   // '*'
 ,{    28,   3,   3,   4,    0,   -5 }   // '+'
 ,{    30,   2,   2,   3,    0,   -2 }   // ','
 ,{    31,   3,   1,   4,    0,   -4 }   // '-'
 ,{    32,   2,   2,   3,    0,   -2 }   // '.'
 ,{    33,   3,   7,   4,    0,   -7 }   // '/'
 ,{    37,   3,   7,   4,    0,   -7 }   // '0'
 ,{    41,   3,   7,   4,    0,   -7 }   // '1'
 ,{    45,   3,   7,   4,    0,   -7 }   // '2'
 ,{    49,   3,   7,   4,    0,   -7 }   // '3'
 ,{    53,   3,   7,   4,    0,   -7 }   // '4'
 ,{    57,   3,   7,   4,    0,   -7 }   // '5'
 ,{    61,   3,   7,   4,    0,   -7 }   // '6'
 ,{    65,   3,   7,   4,    0,   -7 }   // '7'
 ,{    69,   3,   7,   4,    0,   -7 }   // '8'
 ,{    73,   3,   7,   4,    0,   -7 }   // '9'
 ,{    77,   1,   5,   2,    0,   -6 }   // ':'
 ,{    79,   1,   6,   2,    0,   -6 }   // ';'
 ,{    81,   3,   5,   4,    0,   -6 }   // '<'
 ,{    84,   3,   3,   4,    0,   -5 }   // '='
 ,{    86,   3,   5,   4,    0,   -6 }   // '>'
 ,{    89,   3,   7,   4,    0,   -7 }   // '?'
 ,{    93,   4,   7,   5,    0,   -7 }   // '@'
 ,{    97,   3,   7,   4,    0,   -7 }   // 'A'
 ,{   101,   3,   7,   4,    0,   -7 }   // 'B'
 ,{   105,   3,   7,   4,    0,   -7 }   // 'C'
 ,{   109,   3,   7,   4,    0,   -7 }   // 'D'
 ,{   113,   3,   7,   4,    0,   -7 }   // 'E'
 ,{   117,   3,   7,   4,    0,   -7 }   // 'F'
 ,{   121,   4,   7,   5,    0,   -7 }   // 'G'
 ,{   125,   3,   7,   4,    0,   -7 }   // 'H'
 ,{   129,   3,   7,   4,    0,   -7 }   // 'I'
 ,{   133,   3,   7,   4,    0,   -7 }   // 'J'
 ,{   137,   3,   7,   4,    0,   -7 }   // 'K'
 ,{   141,   3,   7,   4,    0,   -7 }   // 'L'
 ,{   145,   4,   7,   5,    0,   -7 }   // 'M'
 ,{   149,   4,   7,   5,    0,   -7 }   // 'N'
 ,{   153,   3,   7,   6,    0,   -7 }   // 'O'
 ,{   157,   3,   7,   4,    0,   -7 }   // 'P'
 ,{   161,   3,   7,   4,    0,   -7 }   // 'Q'
 ,{   165,   3,   7,   4,    0,   -7 }   // 'R'
 ,{   169,   3,   7,   4,    0,   -7 }   // 'S'
 ,{   173,   3,   7,   4,    0,   -7 }   // 'T'
 ,{   177,   3,   7,   4,    0,   -7 }   // 'U'
 ,{   181,   3,   7,   4,    0,   -7 }   // 'V'
 ,{   185,   4,   7,   5,    0,   -7 }   // 'W'
 ,{   189,   3,   7,   4,    0,   -7 }   // 'X'
 ,{   193,   3,   7,   4,    0,   -7 }   // 'Y'
 ,{   197,   3,   7,   4,    0,   -7 }   // 'Z'
 ,{   201,   3,   7,   4,    0,   -7 }   // '['
 ,{   205,   3,   7,   4,    0,   -7 }   // '\'
 ,{   209,   3,   7,   4,    0,   -7 }   // ']'
 ,{   213,   3,   3,   4,    0,   -7 }   // '^'
 ,{   215,   3,   1,   4,    0,   -1 }   // '_'
 ,{   216,   2,   2,   3,    0,   -7 }   // '`'
 ,{   217,   3,   5,   4,    0,   -5 }   // 'a'
 ,{   220,   3,   7,   4,    0,   -7 }   // 'b'
 ,{   224,   3,   5,   4,    0,   -5 }   // 'c'
 ,{   227,   3,   7,   4,    0,   -7 }   // 'd'
 ,{   231,   3,   5,   4,    0,   -5 }   // 'e'
 ,{   234,   3,   7,   4,    0,   -7 }   // 'f'
 ,{   238,   3,   5,   4,    0,   -5 }   // 'g'
 ,{   241,   3,   7,   4,    0,   -7 }   // 'h'
 ,{   245,   1,   7,   2,    0,   -7 }   // 'i'
 ,{   246,   2,   7,   3,    0,   -7 }   // 'j'
 ,{   249,   3,   7,   4,    0,   -7 }   // 'k'
 ,{   253,   1,   7,   2,    0,   -7 }   // 'l'
 ,{   255,   4,   5,   5,    0,   -5 }   // 'm'
 ,{   258,   3,   5,   4,    0,   -5 }   // 'n'
 ,{   261,   3,   5,   4,    0,   -5 }   // 'o'
 ,{   264,   3,   5,   4,    0,   -5 }   // 'p'
 ,{   267,   3,   5,   4,    0,   -5 }   // 'q'
 ,{   270,   3,   5,   4,    0,   -5 }   // 'r'
 ,{   273,   3,   5,   4,    0,   -5 }   // 's'
 ,{   276,   3,   7,   4,    0,   -7 }   // 't'
 ,{   280,   3,   5,   4,    0,   -5 }   // 'u'
 ,{   283,   3,   5,   4,    0,   -5 }   // 'v'
 ,{   286,   4,   5,   5,    0,   -5 }   // 'w'
 ,{   289,   3,   5,   4,    0,   -5 }   // 'x'
 ,{   292,   3,   5,   4,    0,   -5 }   // 'y'
 ,{   295,   3,   5,   4,    0,   -5 }   // 'z'
 ,{   298,   3,   7,   4,    0,   -7 }   // '{'
 ,{   302,   1,   7,   2,    0,   -7 }   // '|'
 ,{   304,   3,   7,   4,    0,   -7 }   // '}'
 ,{   308,   3,   2,   4,    0,   -4 }   // '~'
};

const GFXfont Font4x7Fixed PROGMEM = {
(uint8_t  *)Font4x7FixedBitmaps,
(GFXglyph *)Font4x7FixedGlyphs,
0x20, 0x7E, 7};
