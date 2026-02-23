#ifndef SOUNDS_HPP
#define SOUNDS_HPP

#include "config.hpp"
#include "structs.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <mmsystem.h>
#include <windows.h>

#pragma comment(lib, "winmm.lib")

#define FOOTSTEP_LEVEL1_COUNT 8
#define FOOTSTEP_LEVEL2_COUNT 7
#define FOOTSTEP_TILE_COUNT 5
#define FOOTSTEP_INTERVAL 12

static int g_footstepTimer = 0;
static int g_bgMusicPlaying = 0;
static int g_lastHealthForSound = PLAYER_MAX_HEALTH;
static int g_lastHoveredButton = -1;

// -----------------------------------------------------------------------
// waveOut-based one-shot sound system
//   - WAV files loaded into RAM at startup (no disk I/O at play time)
//   - Each play() call spawns a tiny thread that opens its own HWAVEOUT,
//     writes the PCM buffer, waits for completion, then closes.
//   - Only Win32 APIs used inside the thread (CreateThread-safe, no CRT).
//   - Multiple threads run simultaneously => true sound overlap.
// -----------------------------------------------------------------------

struct WavSound {
  WAVEFORMATEX fmt;
  BYTE *pcm;
  DWORD pcmSize;
};

// Parse a WAV file from disk into a WavSound. Called once at startup.
static void loadWavSound(const char *path, WavSound *snd) {
  snd->pcm = NULL;
  snd->pcmSize = 0;

  HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return;

  DWORD fileSize = GetFileSize(hFile, NULL);
  BYTE *buf = (BYTE *)HeapAlloc(GetProcessHeap(), 0, fileSize);
  if (!buf) {
    CloseHandle(hFile);
    return;
  }

  DWORD bytesRead = 0;
  ReadFile(hFile, buf, fileSize, &bytesRead, NULL);
  CloseHandle(hFile);

  // Walk RIFF chunks starting after the 12-byte RIFF header
  DWORD pos = 12;
  while (pos + 8 <= fileSize) {
    DWORD chunkSize = *(DWORD *)(buf + pos + 4);
    if (memcmp(buf + pos, "fmt ", 4) == 0 && chunkSize >= 16) {
      WAVEFORMATEX *wf = (WAVEFORMATEX *)(buf + pos + 8);
      snd->fmt = *wf;
      snd->fmt.cbSize = 0;
    } else if (memcmp(buf + pos, "data", 4) == 0) {
      snd->pcm = (BYTE *)HeapAlloc(GetProcessHeap(), 0, chunkSize);
      snd->pcmSize = chunkSize;
      if (snd->pcm)
        memcpy(snd->pcm, buf + pos + 8, chunkSize);
    }
    pos += 8 + ((chunkSize + 1) & ~1u); // chunks are word-aligned
  }
  HeapFree(GetProcessHeap(), 0, buf);
}

// Thread arg: immutable pointers into the pre-loaded WavSound
struct PlayWaveArgs {
  WAVEFORMATEX fmt;
  BYTE *pcm;
  DWORD pcmSize;
};

// Thread proc — pure Win32, no CRT functions
static DWORD WINAPI PlayWaveThreadProc(LPVOID p) {
  PlayWaveArgs *a = (PlayWaveArgs *)p;
  HWAVEOUT hWave;
  if (waveOutOpen(&hWave, WAVE_MAPPER, &a->fmt, 0, 0, CALLBACK_NULL) ==
      MMSYSERR_NOERROR) {
    WAVEHDR hdr;
    ZeroMemory(&hdr, sizeof(hdr));
    hdr.lpData = (LPSTR)a->pcm;
    hdr.dwBufferLength = a->pcmSize;
    waveOutPrepareHeader(hWave, &hdr, sizeof(hdr));
    waveOutWrite(hWave, &hdr, sizeof(hdr));
    while (!(hdr.dwFlags & WHDR_DONE))
      Sleep(5);
    waveOutUnprepareHeader(hWave, &hdr, sizeof(hdr));
    waveOutClose(hWave);
  }
  HeapFree(GetProcessHeap(), 0, p);
  return 0;
}

static void playWavSound(WavSound *snd) {
  if (!snd || !snd->pcm)
    return;
  PlayWaveArgs *a = (PlayWaveArgs *)HeapAlloc(
      GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PlayWaveArgs));
  if (!a)
    return;
  a->fmt = snd->fmt;
  a->pcm = snd->pcm;
  a->pcmSize = snd->pcmSize;
  CloseHandle(CreateThread(NULL, 0, PlayWaveThreadProc, a, 0, NULL));
}

// -----------------------------------------------------------------------
// Pre-loaded sound buffers
// -----------------------------------------------------------------------
static WavSound g_sndJump, g_sndFall, g_sndLand, g_sndDash;
static WavSound g_sndAttack, g_sndDownslash, g_sndDeath, g_sndEnemyKill;
static WavSound g_sndOnButton, g_sndButtonClick, g_sndStartClick;
static WavSound g_sndItemCollect, g_sndLoseLife;
static WavSound g_sndFootL1[FOOTSTEP_LEVEL1_COUNT];
static WavSound g_sndFootL2[FOOTSTEP_LEVEL2_COUNT];
static WavSound g_sndFootTile[FOOTSTEP_TILE_COUNT];

// -----------------------------------------------------------------------
// MCI — kept only for background music (needs looping)
// -----------------------------------------------------------------------
inline void mciOpen(const char *path, const char *alias) {
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

// -----------------------------------------------------------------------
// Init — load all WAVs into RAM
// -----------------------------------------------------------------------
void initSounds() {
  g_footstepTimer = 0;
  g_lastHealthForSound = PLAYER_MAX_HEALTH;
  g_lastHoveredButton = -1;

  loadWavSound("Audios/movements/move/jump.wav", &g_sndJump);
  loadWavSound("Audios/movements/move/jump.wav",
               &g_sndFall); // reuse until fall.wav exists
  loadWavSound("Audios/movements/move/land.wav", &g_sndLand);
  loadWavSound("Audios/movements/move/dash.wav", &g_sndDash);
  loadWavSound("Audios/movements/move/attack.wav", &g_sndAttack);
  loadWavSound("Audios/movements/move/downslash.wav", &g_sndDownslash);
  loadWavSound("Audios/movements/move/death.wav", &g_sndDeath);
  loadWavSound("Audios/movements/move/enemykill.wav", &g_sndEnemyKill);

  loadWavSound("Audios/UI/on_button.wav", &g_sndOnButton);
  loadWavSound("Audios/UI/button_click.wav", &g_sndButtonClick);
  loadWavSound("Audios/UI/start_button_click.wav", &g_sndStartClick);
  loadWavSound("Audios/UI/item_collect.wav", &g_sndItemCollect);
  loadWavSound("Audios/UI/lose_life.wav", &g_sndLoseLife);

  char path[128];
  for (int i = 0; i < FOOTSTEP_LEVEL1_COUNT; i++) {
    sprintf_s(path,
              "Audios/movements/footsteps/level 1/footstep_level1(%d).wav",
              i + 1);
    loadWavSound(path, &g_sndFootL1[i]);
  }
  for (int i = 0; i < FOOTSTEP_LEVEL2_COUNT; i++) {
    sprintf_s(path,
              "Audios/movements/footsteps/level 2/footstep_level2(%d).wav",
              i + 1);
    loadWavSound(path, &g_sndFootL2[i]);
  }
  for (int i = 0; i < FOOTSTEP_TILE_COUNT; i++) {
    sprintf_s(path, "Audios/movements/footsteps/tile/footstep_tile(%d).wav",
              i + 1);
    loadWavSound(path, &g_sndFootTile[i]);
  }

  playBGMusic();
}

// -----------------------------------------------------------------------
// Play functions
// -----------------------------------------------------------------------
void playJumpSound() { playWavSound(&g_sndJump); }
void playFallSound() { playWavSound(&g_sndFall); }
void playLandSound() { playWavSound(&g_sndLand); }
void playDashSound() { playWavSound(&g_sndDash); }
void playAttackSound() { playWavSound(&g_sndAttack); }
void playDownslashSound() { playWavSound(&g_sndDownslash); }
void playDeathSound() { playWavSound(&g_sndDeath); }
void playEnemyKillSound() { playWavSound(&g_sndEnemyKill); }
void playOnButtonSound() { playWavSound(&g_sndOnButton); }
void playButtonClickSound() { playWavSound(&g_sndButtonClick); }
void playStartButtonClickSound() { playWavSound(&g_sndStartClick); }
void playItemCollectSound() { playWavSound(&g_sndItemCollect); }
void playLoseLifeSound() { playWavSound(&g_sndLoseLife); }

void playFootstepLevel1() {
  playWavSound(&g_sndFootL1[rand() % FOOTSTEP_LEVEL1_COUNT]);
}
void playFootstepLevel2() {
  playWavSound(&g_sndFootL2[rand() % FOOTSTEP_LEVEL2_COUNT]);
}
void playFootstepTile() {
  playWavSound(&g_sndFootTile[rand() % FOOTSTEP_TILE_COUNT]);
}

// -----------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------
int isPlayerOnTile(struct Player *player, struct Midground *mg) {
  int spriteW = SPRITE_SIZE * (int)SCALE;
  int playerLeft = player->x;
  int playerRight = player->x + spriteW;
  int playerBottom = player->y;
  for (int i = 0; i < mg->tileCount; i++) {
    if (!mg->tiles[i].active)
      continue;
    int tileLeft = mg->tiles[i].x;
    int tileRight = mg->tiles[i].x + (int)mg->tiles[i].width;
    int tileTop = mg->tiles[i].y + (int)mg->tiles[i].height;
    int margin = 40;
    if (playerRight > tileLeft + margin && playerLeft < tileRight - margin)
      if (abs(playerBottom - tileTop) <= 100 ||
          abs(playerBottom - (tileTop + 20)) <= 100)
        return 1;
  }
  return 0;
}

void updateFootstepSounds(struct Player *player, struct Midground *mg,
                          int gameState) {
  if (player->state != WALK || !player->onGround) {
    g_footstepTimer = FOOTSTEP_INTERVAL;
    return;
  }
  if (++g_footstepTimer >= FOOTSTEP_INTERVAL) {
    g_footstepTimer = 0;
    if (isPlayerOnTile(player, mg))
      playFootstepTile();
    else if (gameState == PLAYING_STATE)
      playFootstepLevel1();
    else if (gameState == LEVEL2_STATE)
      playFootstepLevel2();
  }
}

void updateHealthSound(struct Player *player) {
  if (player->health < g_lastHealthForSound && player->health > 0)
    playLoseLifeSound();
  g_lastHealthForSound = player->health;
}

int getTitleButtonAtPos(int mx, int my) {
  if (mx >= 400 && mx <= 600 && my >= 200 && my <= 280)
    return 0;
  if (mx >= 400 && mx <= 600 && my >= 150 && my <= 190)
    return 1;
  if (mx >= 400 && mx <= 600 && my >= 100 && my <= 140)
    return 2;
  if (mx >= 400 && mx <= 600 && my >= 40 && my <= 100)
    return 3;
  return -1;
}

void updateButtonHoverSound(int mx, int my) {
  int btn = getTitleButtonAtPos(mx, my);
  if (btn != -1 && btn != g_lastHoveredButton)
    playOnButtonSound();
  g_lastHoveredButton = btn;
}

#endif
