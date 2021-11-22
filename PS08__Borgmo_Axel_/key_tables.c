#include <stdbool.h>

#include "freq.h"           /* freq_table[] = {f0, f1, f2, ... } */
static int key2index[128];  /* valid keyboard characters to freq_table index */

bool valid_key(int key)
{
    return(key2index[key] >= 0 ? true : false);
}

double key2freq(int key)
{
    double f0;
    if (valid_key(key)) {
        f0 = freq_table[key2index[key]];
    }
    else {
        f0 = -1;
    }
    return(f0);
}

void init_key2index(void)
{
    /* Middle three octaves of piano keyboard
     *
     * qwertyQWERTY -> C3 to B3
     * asdfghASDFGH -> C4 to B4
     * zxcvbnZXCVBN -> C5 to B5
     *
     * all other keys are unrecognized
     */
    int i;
    for (i=0; i<128; i++) {
        /* there are 128 possible ASCII characters
         * array index values are ASCII value
         * initialize to make all entries invalid (-1)
         */
        key2index[i] = -1;
    }
    /* Overwrite valid keys with a key index that is >= 0 
     * The index of key2index[] is the ASCII value of the key qwerty... 
     * The value of key2index[] is the index into the freq[] table of 
     * piano key frequencies.
     */
    i = 0;
    /* octave 3 */
    key2index['q'] = i++;
    key2index['w'] = i++;
    key2index['e'] = i++;
    key2index['r'] = i++;
    key2index['t'] = i++;
    key2index['y'] = i++;

    key2index['Q'] = i++;
    key2index['W'] = i++;
    key2index['E'] = i++;
    key2index['R'] = i++;
    key2index['T'] = i++;
    key2index['Y'] = i++;

    /* octave 4 */
    key2index['a'] = i++;
    key2index['s'] = i++;
    key2index['d'] = i++;
    key2index['f'] = i++;
    key2index['g'] = i++;
    key2index['h'] = i++;

    key2index['A'] = i++;
    key2index['S'] = i++;
    key2index['D'] = i++;
    key2index['F'] = i++;
    key2index['G'] = i++;
    key2index['H'] = i++;

    /* octave 5 */
    key2index['z'] = i++;
    key2index['x'] = i++;
    key2index['c'] = i++;
    key2index['v'] = i++;
    key2index['b'] = i++;
    key2index['n'] = i++;

    key2index['Z'] = i++;
    key2index['X'] = i++;
    key2index['C'] = i++;
    key2index['V'] = i++;
    key2index['B'] = i++;
    key2index['N'] = i++;
}
