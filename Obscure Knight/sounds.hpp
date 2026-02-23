#ifndef SOUNDS_HPP
#define SOUNDS_HPP

#include <windows.h>
#include <mmsystem.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "config.hpp"
#include "structs.hpp"

#pragma comment(lib, "winmm.lib")

#define FOOTSTEP_LEVEL1_COUNT 8
#define FOOTSTEP_LEVEL2_COUNT 7
#define FOOTSTEP_TILE_COUNT 5
#define FOOTSTEP_INTERVAL 12

static int g_footstepTimer = 0;
static int g_bgMusicPlaying = 0;
static int g_lastHealthForSound = PLAYER_MAX_HEALTH;
static int g_lastHoveredButton = -1;

inline void mciPlay(const char* alias) {
    char cmd[128];
    sprintf_s(cmd, "play %s from 0", alias);
    mciSendString(cmd, NULL, 0, NULL);
}

inline void mciOpen(const char* path, const char* alias) {
    char cmd[256];
    sprintf_s(cmd, "open \"%s\" type waveaudio alias %s", path, alias);
    mciSendString(cmd, NULL, 0, NULL);
}

void playBGMusic() {
    mciSendString("close bgmusic", NULL, 0, NULL);
    mciOpen("Audios/Background/bg.wav", "bgmusic");
    mciSendString("play bgmusic repeat", NULL, 0, NULL);
    g_bgMusicPlaying = 1;
}

void stopBGMusic() {
    mciSendString("stop bgmusic", NULL, 0, NULL);
    mciSendString("close bgmusic", NULL, 0, NULL);
    g_bgMusicPlaying = 0;
}

void restartBGMusic() {
    stopBGMusic();
    playBGMusic();
}

void initSounds() {
    g_footstepTimer = 0;
    g_lastHealthForSound = PLAYER_MAX_HEALTH;
    g_lastHoveredButton = -1;

    // Pre-open all SFX for 0.01ms latency
    mciOpen("Audios/movements/move/jump.wav", "sfx_jump");
    mciOpen("Audios/movements/move/land.wav", "sfx_land");
    mciOpen("Audios/movements/move/dash.wav", "sfx_dash");
    mciOpen("Audios/movements/move/attack.wav", "sfx_attack");
    mciOpen("Audios/movements/move/downslash.wav", "sfx_downslash");
    mciOpen("Audios/movements/move/death.wav", "sfx_death");
    mciOpen("Audios/movements/move/enemykill.wav", "sfx_enemykill");
    
    mciOpen("Audios/UI/on_button.wav", "sfx_on_button");
    mciOpen("Audios/UI/button_click.wav", "sfx_button_click");
    mciOpen("Audios/UI/start_button_click.wav", "sfx_start_click");
    mciOpen("Audios/UI/item_collect.wav", "sfx_item_collect");
    mciOpen("Audios/UI/lose_life.wav", "sfx_lose_life");

    // Pre-open footsteps to avoid disk lag
    char path[128], alias[32];
    for(int i=1; i<=FOOTSTEP_LEVEL1_COUNT; i++){
        sprintf_s(path, "Audios/movements/footsteps/level 1/footstep_level1(%d).wav", i);
        sprintf_s(alias, "fs_l1_%d", i);
        mciOpen(path, alias);
    }
    for(int i=1; i<=FOOTSTEP_LEVEL2_COUNT; i++){
        sprintf_s(path, "Audios/movements/footsteps/level 2/footstep_level2(%d).wav", i);
        sprintf_s(alias, "fs_l2_%d", i);
        mciOpen(path, alias);
    }
    for(int i=1; i<=FOOTSTEP_TILE_COUNT; i++){
        sprintf_s(path, "Audios/movements/footsteps/tile/footstep_tile(%d).wav", i);
        sprintf_s(alias, "fs_t_%d", i);
        mciOpen(path, alias);
    }

    playBGMusic();
}

void playJumpSound() { mciPlay("sfx_jump"); }
void playLandSound() { mciPlay("sfx_land"); }
void playDashSound() { mciPlay("sfx_dash"); }
void playAttackSound() { mciPlay("sfx_attack" ); }
void playDownslashSound() { mciPlay("sfx_downslash"); }
void playDeathSound() { mciPlay("sfx_death"); }
void playEnemyKillSound() { mciPlay("sfx_enemykill"); }
void playOnButtonSound() { mciPlay("sfx_on_button"); }
void playButtonClickSound() { mciPlay("sfx_button_click"); }
void playStartButtonClickSound() { mciPlay("sfx_start_click"); }
void playItemCollectSound() { mciPlay("sfx_item_collect"); }
void playLoseLifeSound() { mciPlay("sfx_lose_life"); }

void playFootstepLevel1() {
    char alias[32];
    sprintf_s(alias, "fs_l1_%d", (rand() % FOOTSTEP_LEVEL1_COUNT) + 1);
    mciPlay(alias);
}

void playFootstepLevel2() {
    char alias[32];
    sprintf_s(alias, "fs_l2_%d", (rand() % FOOTSTEP_LEVEL2_COUNT) + 1);
    mciPlay(alias);
}

void playFootstepTile() {
    char alias[32];
    sprintf_s(alias, "fs_t_%d", (rand() % FOOTSTEP_TILE_COUNT) + 1);
    mciPlay(alias);
}

int isPlayerOnTile(struct Player *player, struct Midground *mg) {
    int spriteW = SPRITE_SIZE * (int)SCALE;
    int playerLeft = player->x;
    int playerRight = player->x + spriteW;
    int playerBottom = player->y;

    for (int i = 0; i < mg->tileCount; i++) {
        if (!mg->tiles[i].active) continue;
        int tileLeft = mg->tiles[i].x;
        int tileRight = mg->tiles[i].x + (int)mg->tiles[i].width;
        int tileTop = mg->tiles[i].y + (int)mg->tiles[i].height;
        int margin = 40;
        if (playerRight > tileLeft + margin && playerLeft < tileRight - margin) {
            if (abs(playerBottom - tileTop) <= 100 || abs(playerBottom - (tileTop + 20)) <= 100) return 1;
        }
    }
    return 0;
}

void updateFootstepSounds(struct Player *player, struct Midground *mg, int gameState) {
    if (player->state != WALK || !player->onGround) {
        g_footstepTimer = FOOTSTEP_INTERVAL;
        return;
    }
    g_footstepTimer++;
    if (g_footstepTimer >= FOOTSTEP_INTERVAL) {
        g_footstepTimer = 0;
        if (isPlayerOnTile(player, mg)) playFootstepTile();
        else if (gameState == PLAYING_STATE) playFootstepLevel1();
        else if (gameState == LEVEL2_STATE) playFootstepLevel2();
    }
}

void updateHealthSound(struct Player *player) {
    if (player->health < g_lastHealthForSound && player->health > 0) playLoseLifeSound();
    g_lastHealthForSound = player->health;
}

int getTitleButtonAtPos(int mx, int my) {
    if (mx >= 400 && mx <= 600 && my >= 200 && my <= 280) return 0;
    if (mx >= 400 && mx <= 600 && my >= 150 && my <= 190) return 1;
    if (mx >= 400 && mx <= 600 && my >= 100 && my <= 140) return 2;
    if (mx >= 400 && mx <= 600 && my >= 40 && my <= 100) return 3;
    return -1;
}

void updateButtonHoverSound(int mx, int my) {
    int btn = getTitleButtonAtPos(mx, my);
    if (btn != -1 && btn != g_lastHoveredButton) playOnButtonSound();
    g_lastHoveredButton = btn;
}

#endif

