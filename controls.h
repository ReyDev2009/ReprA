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
        if ( ch == ' ' ) return 3;
        if ( ch == 'q' ) return 4;
        if ( ch == 'a' ) return 5;
        if ( ch == '+' ) return 6;
        if ( ch == '-') return 7;
    }

    return 0;
}

void play_aleatorie(int* avis, int* idx , const int* count, int* order , int* posi , int* sig ) { 
    srand(time(NULL));
    if ( !*sig ) *idx = order [ *posi = *posi - 1 < 0 ? 0 : *posi - 1  ];
    else if ( *sig == 1 ) {
        if ( order [ *posi + 1 > *count ? *count : *posi + 1  ]  ) {
            *idx = order [ ++*posi ];
        }
        else {
            while ( (*idx) == -1 || ( avis [ (*idx) ] ) ) { (*idx) = rand() % (*count); } 
            order [ ++*posi ] = *idx;
        }
    }
}
#endif