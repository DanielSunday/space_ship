#ifndef SOUND_H
#define SOUND_H

#define SAMPLE_RATE 44100
#define BUFFER_SIZE  1024

void init_sound_system(void);
void init_sound(const char *addr);
void play_sound(void);
void stop_sound(void);
void exit_sound(void);
void exit_sound_system(void);

#endif
