#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "PlayManager.h"
#include "free_console.h"
#include "controls.h"

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>





int main () {
    atexit(restore_terminal);
    signal(SIGINT, handle_sigint);
    ma_result result;
    ma_engine engine;
    
    result = ma_engine_init( NULL , &engine);

    if ( result != MA_SUCCESS ) {
        printf("ERROR AL INICIAR EL MOTOR\n");
        return -1;
    }
    
    printf("Directorio a reproducir: ");
    char in_dir [ 5000 ];
    scanf("%s" , in_dir );
    Playlist* playlist = create_playlist();
    CurrentAudio* curr = create_current_audio_session();
    readDir( playlist, in_dir );
    init_play(playlist, curr , true);

    set_nonblocking_terminal();
    printf(" CONTROLES:\n");
    printf(" [N] SIGUIENTE CANCION\n");
    printf(" [P] Cancion Anterior\n");
    printf(" [A] Activar / Desactivar Aleatorio\n");
    printf(" [Q] SALIR\n");

    int count = 0;
    char** playl = insert_to_array(playlist , &count );
    printf("%d\n", playlist->size );
    int* avis = calloc(count, sizeof( int ));
    if ( !avis ) {
        printf("EROR: No se pudo asignar memoria para avis");
        ma_engine_uninit(&engine);
        return -1;
    }
    int* order = calloc ( count, sizeof( int ) );

    int is_aleatorie = 0;
    
    int idx = 0;
    int posi = 0;
    int vis = 0;
    int ke = 0;
    int p = 1;
    order [ 0 ] = 0;
    while ( ( ( idx < count && idx >= 0 && !is_aleatorie ) || ( is_aleatorie && vis < count ) ) && posi >= 0 && posi <= count ) {
        const char* filepath = playl[ idx ];
        if ( !avis [ idx ] ) {
            vis++;
            avis [ idx ] = 1;
        }
        
        if (filepath == NULL) {
            printf("⚠️  Ruta nula, saltando...\n");
            if ( is_aleatorie ) play_aleatorie(avis, &idx, &count , order , &posi , &ke );
            else if ( ke ) {
                if ( ke == 2 ) idx--;
                else if ( ke == 1 ) idx++;
            }
            continue;
        }
        
        FILE* test = fopen(filepath, "rb");
        if (test == NULL) {
            printf("⚠️  No se encuentra: %s, saltando...\n", filepath);
            if ( is_aleatorie ) play_aleatorie(avis, &idx, &count , order , &posi , &ke );
            else if ( ke ) {
                if ( ke == 2 ) idx--;
                else if ( ke == 1 ) idx++;
            }
            continue;
        }
        fclose(test);
        
        printf("▶️  Reproduciendo: %s\n", filepath);
        ma_sound sound;
        result = ma_sound_init_from_file(&engine, filepath, 0, NULL, NULL, &sound);
        if (result != MA_SUCCESS) {
            printf("⚠️  Error cargando: %s\n", filepath);
            if ( is_aleatorie ) play_aleatorie(avis, &idx, &count , order, &posi , &ke);
            else if ( ke ) {
                if ( ke == 2 ) idx--;
                else if ( ke == 1 ) idx++;
            }
            continue;
        }
        
        
        result = ma_sound_start(&sound);
        if (result != MA_SUCCESS) {
            printf("⚠️  Error iniciando: %s\n", filepath);
            ma_sound_uninit(&sound);
            if ( is_aleatorie ) play_aleatorie(avis, &idx, &count , order, &posi, &ke );
            else if ( ke ) {
                if ( ke == 2 ) idx--;
                else if ( ke == 1 ) idx++;
            }
            continue;
        }


        
        int pause = 0;
        while (!ma_sound_at_end(&sound) && !should_exit ) {
            if ( ( ke = get_action() ) ) { 
                if ( ke == 4 ) {
                    if ( !pause ) { ma_sound_stop ( &sound ); pause = 1; }
                    else {ma_sound_start(&sound); pause = 0;}
                }
                else if ( ke == 5 ) {
                    if ( is_aleatorie ) is_aleatorie = 0;
                    else is_aleatorie = 1;
                    printf("Modo aleatorio: %d\n" , is_aleatorie );
                }
                else break;
            }
            ma_sleep(100);
        }
        
        if ( !pause ) ma_sound_stop( &sound );
        ma_sound_uninit(&sound);
        
        if ( should_exit ) { printf("SALIENDO\n"); break;}
        if ( is_aleatorie && ke != 3 ) { 
            
            play_aleatorie(avis, &idx, &count , order, &posi , &ke ); continue; 
        }
        else if ( ke ) {
            if (ke == 2 ) {
                printf("ANTERIOR\n");
                idx--;
                continue;
            }
            else if ( ke == 1 ) { printf("SIGUIENTE\n");}
            else {
                printf("SALIENDO\n");
                break;
            }
        }
        else { printf("FINAL\n");}
        
        
        idx++;
    }
    
    free(avis);
    ma_engine_uninit(&engine);
    
    return 0;
}