/* PS09 Axel Borgmo */

#include <stdio.h>
#include <stdlib.h> //for exit()
#include <math.h>   //for sin()
#include "synth.h"



Synth::Synth() {;} //Constructor

Synth::~Synth() {;} //Deconstructor

double Synth::set_mod(double in_freq) {
    double fs, f0;
    mod.freq = in_freq;
    mod.phase = 0;
    fs = this->samp_rate;
    f0 = mod.freq;

    if (in_freq == 0) {
        mod.phase_inc = -1; 
    }

    else {
        mod.phase_inc = 2*PI*f0/fs;
    }

    return 0;
}


void Synth::init_synth(int num_chan, int in_samp_rate) 
{
   Tone *pt = &this->tone[0]; /* Tone structure */
    /* initialize class data */
    this->samp_rate = in_samp_rate;
    this->num_keys = 0; //number of keys voiced
    for (int i=0; i<KEYS_VOICED; i++) {
        //space, which is printed in Ncurses loop
        pt[i].key = ' '; 
    }
}

void Synth::execute_cmd(int in_cmd)
{
    switch (in_cmd) {
        case (CMD_ADD_KEY):
        add_key(this->new_key, this->new_freq);
        break;
        case (CMD_RM_KEY):
        rm_key();
        break;
    }
}

double *Synth::synth_block(int framesPerBuffer)
{
    Tone *pt = &this->tone[0];

    if (this->cmd) {
        int cmd = this->cmd; // local variable
        execute_cmd(cmd); 
        this->cmd = 0;
    }

    int n = 0;

    for (int i=0; i<framesPerBuffer; i++) { // Loop where each frame is interleaved samples of all channels

        double v = 0; // Initialize the output value to zero. 
        int num_keys = this->num_keys;   


        for (n=0; n<num_keys; n++) { 

            /* Synthesize the tones */
            if (pt[n].phase_inc != -1) { 
                v += sin(pt[n].phase) * (1 - pt[n].attack_amp) * (pt[n].decay_amp); // Compute next tone sample value
                pt[n].phase += pt[n].phase_inc; // Phase incremented by the phase_inc
                pt[n].attack_amp *= pt[n].attack_factor;
                pt[n].decay_amp *= pt[n].decay_factor;
            }

            if (pt[n].decay_amp < DROP_LEVEL) { // Stops playout if level is lower than -60 dB
                pt[n].phase_inc = -1;
            }


            /* Perform the modulation */
            if (this->mod.phase_inc > 0) { 
                v *= sin(this->mod.phase);
                this->mod.phase += this->mod.phase_inc;
            }


        }

        this->output[i] = v * FS_AMPL;

    }

    return(&this->output[0] );
}



void Synth::init_key(int new_key, double new_freq)
{

    int n;
    double fs, f0;

    n = this->num_keys-1; /* from number to index */  
    if (n < 0) {
        /* since init_key() is called after ++num_keys, this should not occur */
        fprintf(stderr, "ERROR in init_key(): num_keys %d\n", this->num_keys);
        exit(-1);
    }

   
    f0 = new_freq;  // Settings values for the synth structure elements
    fs = this->samp_rate;


    /* Initialize values at Tone array index (new_key-1) pt: */
    Tone *pt = &this->tone[this->num_keys-1];

    pt->key = new_key;
    pt->f0 = new_freq;
    pt->phase_inc = 2*PI*f0/fs;
    pt->phase = 0.0;
    pt->attack_factor = ATTACK_FACTOR;
    pt->decay_factor = DECAY_FACTOR;
    pt->attack_amp = 1.0;
    pt->decay_amp = 1.0;

}



void Synth::add_key(int in_new_key, double in_new_freq)
{
    this->new_key = in_new_key;
    this->new_freq = in_new_freq;
    /* increment number of keys */
    if (++(this->num_keys) > KEYS_VOICED) {
        /* list is full, so remove oldest key by shifting key list */
        shift_keys();
        this->num_keys = KEYS_VOICED;
    }
    /* enter new key info */
    init_key(in_new_key, in_new_freq);
}

void Synth::rm_key(void)
{
    /* remove oldest key by shifting key list down */
    shift_keys();
    /* decreast number of keys voicing */
    /* number of keys can never be less than zero */
    if (--(this->num_keys) < 0) {
        this->num_keys = 0;
    }
}

void Synth::shift_keys(void)
{
    Tone *pt = &this->tone[0];
    /* shift tone array values down by one place */
    for (int i=0; i<KEYS_VOICED-1; i++) {
        pt[i].key = pt[i+1].key;
        pt[i].f0 = pt[i+1].f0;
        pt[i].phase_inc = pt[i+1].phase_inc;
        pt[i].phase = pt[i+1].phase;
        pt[i].attack_factor = pt[i+1].attack_factor;
        pt[i].decay_factor = pt[i+1].decay_factor;
        pt[i].attack_amp = pt[i+1].attack_amp;
        pt[i].decay_amp = pt[i+1].decay_amp;
    }
    /* top array entry is initialized to inactive */
    int n = KEYS_VOICED-1;
    pt[n].key = ' ';
    pt[n].phase_inc = -1;
}





























