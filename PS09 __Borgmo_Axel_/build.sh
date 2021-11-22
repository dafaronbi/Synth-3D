g++ -Wno-deprecated -o synth \
	main.cpp synth.cpp key_tables.c paUtils.c \
	-I/usr/local/include \
	-L/usr/local/lib -lsndfile -lportaudio -lncurses
