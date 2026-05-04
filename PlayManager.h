#ifndef PLAYMANAGER
#define PLAYMANAGER

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<string.h>
#include "stack.h"
#include<stdbool.h>
#include<dirent.h>
#define CHUNK_SIZE 4046

typedef struct AudioFile{
    struct AudioFile *sig;
    struct AudioFile *ant;
    char* path;

} AudioFile;




typedef struct Playlist{
    AudioFile* First;
    AudioFile* Last;
    int size;
} Playlist;

typedef struct CurrentAudio {
    AudioFile* audio;
    char* ( *show ) ( struct CurrentAudio* self );
    void ( *next ) ( struct CurrentAudio* self );
    void ( *prev ) ( struct CurrentAudio* self );

} CurrentAudio;

char* show ( CurrentAudio* self ) { return self -> audio -> path; }
void next ( CurrentAudio* self ) { self -> audio = self-> audio -> sig; }
void prev ( CurrentAudio* self ) { self -> audio = self -> audio -> ant; }

CurrentAudio* create_current_audio_session () {
    CurrentAudio* c = (CurrentAudio*) malloc ( sizeof( CurrentAudio ));
    c -> audio = NULL;
    c -> show = show;
    c -> next = next;
    c -> prev = prev;
    return c;
}

Playlist* create_playlist () {
    Playlist* p = malloc ( sizeof ( Playlist ));
    if (p == NULL) {  
        fprintf(stderr, "Error: no se pudo reservar memoria\n");
        return NULL;
    }
    p -> First = p -> Last = NULL;
    p -> size = 0;
    return p;
}

int add_audio_to_playlist ( Playlist* head , char* path ) {
    AudioFile* audio = (AudioFile* ) malloc( sizeof ( AudioFile ) );
    if ( audio == NULL ) { return 1; }
    audio -> path = strdup(path);
    if (audio->path == NULL) {
        free(audio);
        return 1;
    }
    if (head->First == NULL) {
        head->First = audio;
        head->Last = audio;
    }
    else {
        audio->ant = head->Last;
        head->Last->sig = audio;
        head->Last = audio;
    }
    
    head->size++;

    return 0;

}

void init_play( Playlist* head , CurrentAudio* curr , bool front) {
    if ( head == NULL ||  head -> First == NULL ) {
        printf( "La Playlist se encuentra vacia ");
        return;
    }
    if ( front ) curr -> audio = head -> First;
    else curr -> audio = head -> Last;
}




void add_content( char* DirPath , Playlist* head , stack_char** stack ) {
    DIR *d = opendir(DirPath);
    if ( !d ) return;
    struct dirent *dir;
    struct stat st;
    while ( ( dir = readdir (d) ) != NULL ) {
        if ( strcmp ( dir->d_name , ".") == 0 || strcmp( dir -> d_name , ".." ) == 0 ) continue;

        char full_path[ 2048 ];
        snprintf(full_path , sizeof(full_path) , "%s/%s", DirPath, dir->d_name );
        if (  stat(full_path, &st ) == 0 ) {
            if ( S_ISDIR(st.st_mode ) ) add_element_to_stack(stack , full_path );
            else add_audio_to_playlist(head, full_path );
        }
    }
}

void readDir( Playlist* head , char* DirPath ) {
    stack_char* stack = NULL;
    add_element_to_stack( &stack , DirPath );
    
    while ( stack != NULL ) {
        char* dir = stack -> t;
        stack_char* tmp = stack;
        stack = stack -> sig;
        add_content ( dir , head , &stack );
        free ( tmp );
    }
    
}

char** insert_to_array ( Playlist* pl , int* count ) {
    CurrentAudio* curr = create_current_audio_session();
    
    int sz = 2;
    char** arr = malloc ( sz * sizeof(char* ));

    init_play(pl , curr , true );
    while ( curr -> audio != NULL ) {
        if ( *count >= sz ) {
            sz *= 2;
            arr = realloc( arr , sz * sizeof(char*));
        }
        arr [ (*count) ] = curr ->show(curr);
        (*count)++;
        curr -> next( curr );
    }
    return arr;
}

#endif