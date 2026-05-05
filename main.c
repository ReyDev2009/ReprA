#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "PlayManager.h"
#include "free_console.h"
#include "Engine.h"
#include "controls.h"

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>





int main () {
    atexit(restore_terminal);
    signal(SIGINT, handle_sigint);
    Player player = {0};
    player.init_player = init_player;
    player.play = play;
    player.set_volume_impl = set_volumen_impl;
    if ( !player.init_player(&player) ) {
        printf("%s","ERROR AL INICIAR EL MOTOR: \n", player.result );
        
        return -1;
    }
    
    player.set_volume_impl(&player, 0.5f);
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
        ma_engine_uninit(&player.engine);
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
    int sig = 1;
    while ( ( ( idx < count && idx >= 0 && !is_aleatorie ) || ( is_aleatorie && vis < count ) ) && posi >= 0 && posi <= count ) {
        const char* filepath = playl[ idx ];
        if ( !avis [ idx ] ) {
            vis++;
            avis [ idx ] = 1;
        }
        
        if ( !player.play(&player, filepath , &is_aleatorie) ) {
            play_aleatorie(avis, &idx, &count , order, &posi , &sig );
            continue;
        }
        
        int pause = 0;
        while (!ma_sound_at_end(&player.sound) && !should_exit ) {
            if ( ( ke = get_action() ) ) { 
                if ( ke == 3 ) {
                    if ( !pause ) { ma_sound_stop ( &player.sound ); pause = 1; }
                    else {ma_sound_start(&player.sound); pause = 0;}
                }
                else if ( ke == 5 ) {
                    if ( is_aleatorie ) is_aleatorie = 0;
                    else is_aleatorie = 1;
                    printf("Modo aleatorio: %d\n" , is_aleatorie );
                }
                else if ( ke == 6 ) player.set_volume_impl( &player, player.volume + 0.1f );
                else if ( ke == 7 ) player.set_volume_impl( &player, player.volume - 0.1f );
                else break;
            }
            ma_sleep(30);
        }
        
        if ( !pause ) ma_sound_stop( &player.sound );
        ma_sound_uninit(&player.sound);
        
        if ( should_exit ) { printf("SALIENDO\n"); break;}
        if ( is_aleatorie && ke < 4 ) {
            if ( ke == 2 ) sig = 0;
            else if ( ke == 1 ) sig = 1;
            play_aleatorie(avis, &idx, &count , order, &posi , &sig ); continue; 
        }
        else if ( ke ) {
            if (ke == 2 ) {
                printf("ANTERIOR\n");
                idx--;
                sig = 0;
                continue;
            }
            else if ( ke == 1 ) { printf("SIGUIENTE\n"); sig = 1;}
            else if ( ke == 4 ) {
                printf("SALIENDO\n");
                break;
            }
        }
        else { printf("FINAL\n");}
        
        
        idx++;
    }
    
    free(avis);
    ma_engine_uninit(&player.engine);
    
    return 0;
}