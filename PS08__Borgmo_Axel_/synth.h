#ifndef _SYNTH_H_
#define _SYNTH_H_

#include <stdbool.h>
#include <stdatomic.h>  /* permits write/read of "cmd" to be atomic */

/* write output to wav file for debugging */
#define DB_WAV_OUT          0   

/* other defines */
#define KEYS_VOICED         4   /* number of simultaneouse keys voiced */
#define NUM_KEYS		    12  /* for 12 semitones in an octave */
#define NUM_OCTAVES         3   /* three octaves */
#define SAMP_RATE           48000
#define NUM_CHAN	        2
#define FRAMES_PER_BUFFER   1024
#define FS_AMPL             0.5 /* -6 dB FS */
//#define ATTACK_FACTOR       0.99980 /* attack time constant of 100 ms */
//#define ATTACK_FACTOR       0.99960 /* attack time constant of 50 ms */
#define ATTACK_FACTOR       0.99800 /* attack time constant of 10 ms */
#define DECAY_FACTOR        0.99998 /* decay time constant of 1.0 sec */
#define DROP_LEVEL          0.001  /* -60 dBFS */
#define PI                  3.14159265358979323846

/* commands */
enum {
    CMD_ADD_KEY = 1,
    CMD_RM_KEY
};

/* for each tone */
typedef struct {
    atomic_int cmd; /* command from user interface thread */
    int key; /* keyboard key */  
    double f0; /* frequency associated with key */ 
    double phase_inc; /* phase increment per sample to realize freq */
    double phase; /* save phase value for next sample */
    double attack_factor;
    double decay_factor;
    double attack_amp; /* save attack amplitude for next sample */
    double decay_amp; /* save decay amplitude for next sample */
} Tone;

typedef struct {
    atomic_int cmd; //command for synth
    int new_key;    //key pressed
    double new_freq;//freq associated with key
    int num_keys;   //number of keys voiced
    int samp_rate;  //sampling rate of output
    Tone tone[KEYS_VOICED];
    double output[FRAMES_PER_BUFFER];
} Synth;

    /* function prototypes */
    void init_synth(Synth *ps, int num_chan, int samp_rate);
    void execute_cmd(Synth *ps, int cmd);
    double *synth_block(Synth *ps, int len);
    void add_key(Synth *ps, int new_key, double new_freq);
    void rm_key(Synth *ps);
    void shift_keys(Synth *p);
    void init_key(Synth *ps, int new_key, double new_freq);

#endif /* _SYNTH_H_ */








