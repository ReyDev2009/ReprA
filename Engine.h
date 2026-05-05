#ifndef ENGINE_PLAYER
#define ENGINE_PLAYER
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

typedef struct Player{
    ma_result result;
    ma_engine engine;
    ma_sound sound;
    float volume;
    int  ( *init_player ) ( struct Player* self);
    int (*play ) ( struct Player* self, const char* filepath, int* is_aleatorie );
    void (*set_volume_impl) ( struct Player* player , float volumen );
} Player;


int init_player ( Player* player ) {
    player -> result = ma_engine_init( NULL , &player -> engine );
    if ( player -> result != MA_SUCCESS ) return 0; 
    return 1;
}

int play ( Player* player , const char* filepath, int* is_aleatorie ) {
    if (filepath == NULL) {
            printf("⚠️  Ruta nula, saltando...\n");
            return 0;
        }
        FILE* test = fopen(filepath, "rb");
        if (test == NULL) {
            printf("⚠️  No se encuentra: %s, saltando...\n", filepath);
            return 0;
        }
        fclose(test);
        
        printf("▶️  Reproduciendo: %s\n", filepath);
        
        player -> result = ma_sound_init_from_file(&player -> engine, filepath, 0, NULL, NULL, &player -> sound);
        if ( player -> result != MA_SUCCESS) {
            printf("⚠️  Error cargando: %s\n", filepath);
            return 0;
        }
        
        
        player -> result = ma_sound_start(&player -> sound);
        if ( player -> result  != MA_SUCCESS) {
            printf("⚠️  Error iniciando: %s\n", filepath);
            ma_sound_uninit(&player -> sound);
            return 0;
        }

    return 1;
}

void set_volumen_impl( Player* player, float volume ) {
    if ( player == NULL ) return;

    if ( volume < 0.0f ) volume = 0.0f;
    if ( volume > 1.0f ) volume = 1.0f;

    player -> volume = volume;
    ma_engine_set_volume(&player->engine , volume );
}


#endif