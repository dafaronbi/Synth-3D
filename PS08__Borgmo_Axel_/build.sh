gcc -Wall -o synth main.c synth.c key_tables.c paUtils.c \
	-I/usr/local/include \
	-L/usr/local/lib -lsndfile -lportaudio -lncurses
