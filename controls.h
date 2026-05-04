#ifndef CONTROLS
#define CONTROLS

#include<unistd.h>
#include<stdlib.h>
#include<time.h>

int get_action ( ) {
    char ch;
    int n = read (STDIN_FILENO, &ch, 1 );
    if ( n > 0 ) {
        if ( ch == 'n') return 1;
        if ( ch == 'p') return 2;
        if ( ch == 'q' ) return 3;
        if ( ch == ' ' ) return 4;
        if ( ch == 'a' ) return 5;
    }

    return 0;
}

void play_aleatorie(int* avis, int* idx , const int* count) { 
    srand(time(NULL));
    while ( (*idx) == -1 || ( avis [ (*idx) ] ) ) { (*idx) = rand() % (*count); } 
}
#endif