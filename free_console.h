#ifndef FREE_CONSOLE
#define FREE_CONSOLE

#include<termios.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

volatile __sig_atomic_t should_exit = 0;

void handle_sigint( int sig ) { should_exit = 1; }

void set_nonblocking_terminal (){
    struct termios ttst;
    tcgetattr(STDIN_FILENO, &ttst );
    ttst.c_lflag &= ~( ICANON | ECHO );
    ttst.c_cc[VMIN] = 0;
    ttst.c_cc[VTIME] = 0;
    tcsetattr( STDIN_FILENO , TCSANOW , &ttst );
}

void restore_terminal () {
    struct termios ttst;
    tcgetattr( STDIN_FILENO , &ttst );
    ttst.c_lflag |= ( ICANON | ECHO );
    tcsetattr( STDIN_FILENO , TCSANOW , &ttst );
}

#endif