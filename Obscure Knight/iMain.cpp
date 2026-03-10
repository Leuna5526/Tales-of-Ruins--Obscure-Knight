#include "Sentry.hpp"
#include "background.hpp"
#include "boss.hpp"
#include "bug.hpp"
#include "camera.hpp"
#include "game.hpp"
#include "healthbar.hpp"
#include "iGraphics.h"
#include "midground.hpp"
#include "player.hpp"
#include "sounds.hpp"
#include "staminabar.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include "title.hpp"
#include <math.h>
#include <time.h>

int gameState = TITLE_SCREEN_STATE;
struct Player player;
struct TitleScreen titleScreen;
struct Background bg;
struct Midground mg;
struct Creature creatures[MAX_CREATURES];
struct Sentry sentries[MAX_SENTRIES];
struct SparkleEffect sparkles[MAX_SPARKLES];
struct Camera camera;
struct Pickup pickups[MAX_PICKUPS];
struct GlowProjectile glowProjectile;
struct NPC npc;
struct Boss boss;
struct BossMinion bossMinions[MAX_BOSS_MINIONS];
struct BossHazard bossHazards[MAX_BOSS_HAZARDS];

void iDraw() {
  iClear();

  if (gameState == TITLE_SCREEN_STATE) {
    renderTitleScreen(&titleScreen);
  } else if (gameState == CREDITS_STATE) {
    renderCredits(&titleScreen);
  } else if (gameState == CONTROLS_STATE) {
    renderControls(&titleScreen);
  } else if (gameState == LOADING_STATE) {
    renderLoadingScreen(&titleScreen);
  } else if (gameState == PLAYING_STATE) {
    renderBackgroundWithCamera(&camera, backgroundTextures);
    renderMidground(&mg, &camera, gameState);
    renderCreatures(creatures, &camera);
    renderStaminaBar(&player);
    renderHealthBar(&player);
    renderPickups(pickups, &camera);
    renderInventoryUI(&player);
    renderGlowProjectile(&glowProjectile, &camera);
    renderPlayer(&player, &camera);

  } else if (gameState == TUNNEL_STATE) {
    renderTunnelBackground(bg.x, bg.tunnelTransitionY);
    player.state = FALL;
    renderPlayer(&player, &camera);

  } else if (gameState == LEVEL2_STATE) {
    renderBackgroundWithCamera(&camera, level2BackgroundTextures);
    renderMidground(&mg, &camera, gameState);
    renderCreatures(creatures, &camera);
    renderSentries(sentries, &camera);
    renderSparkles(sparkles, &camera);
    renderStaminaBar(&player);
    renderHealthBar(&player);
    renderPickups(pickups, &camera);
    renderInventoryUI(&player);
    renderGlowProjectile(&glowProjectile, &camera);
    renderPlayer(&player, &camera);
  } else if (gameState == LEVEL3_STATE) {
    renderBackgroundWithCamera(&camera, level3BackgroundTextures);
    renderStaminaBar(&player);
    renderHealthBar(&player);
    renderPickups(pickups, &camera);
    renderInventoryUI(&player);
    renderGlowProjectile(&glowProjectile, &camera);
    renderPlayer(&player, &camera);
  } else if (gameState == BOSS_STATE) {
    if (bossBackgroundTexture != 0) {
      iShowImage(0, 0, SCREEN_W, SCREEN_H, bossBackgroundTexture);
    }
    renderHazards(bossHazards, &camera);
    renderBoss(&boss, &camera);
    renderMinions(bossMinions, &camera);
    renderStaminaBar(&player);
    renderHealthBar(&player);
    renderPickups(pickups, &camera);
    renderInventoryUI(&player);
    renderGlowProjectile(&glowProjectile, &camera);
    renderPlayer(&player, &camera);
  } else if (gameState == CAVE_STATE) {
    renderCaveBackground();
    renderStaminaBar(&player);
    renderHealthBar(&player);
    renderInventoryUI(&player);
    renderNPC(&npc, &camera);
    renderPickups(pickups, &camera);
    renderPlayer(&player, &camera);
    renderDialogue(&npc);
  }
}

void animate() {
  if (gameState == TITLE_SCREEN_STATE) {
    updateTitleAnimation(&titleScreen);
  } else if (gameState == LOADING_STATE) {
    updateLoadingScreen(&titleScreen, &gameState);
  } else if (gameState == PLAYING_STATE) {
    updateGame(&player, creatures, &bg, &mg, &camera, &gameState, pickups,
               sparkles);
    updatePickups(pickups, &player);
    updateItemEffects(&player);
    updateGlowProjectile(&glowProjectile, creatures, sentries);
    handleItemInput(&player, &glowProjectile, creatures, sentries, gameState);
    updateFootstepSounds(&player, &mg, gameState);
    updateHealthSound(&player);
  } else if (gameState == TUNNEL_STATE || gameState == LEVEL2_STATE ||
             gameState == LEVEL3_STATE || gameState == BOSS_STATE) {
    updateGame(&player, creatures, &bg, &mg, &camera, &gameState, pickups,
               sparkles);
    if (gameState == LEVEL2_STATE) {
      updateSentries(sentries, &player, pickups);
      updateSparkles(sparkles);
    }
    if (gameState == LEVEL2_STATE || gameState == LEVEL3_STATE ||
        gameState == BOSS_STATE) {
      updatePickups(pickups, &player);
      updateItemEffects(&player);
      updateGlowProjectile(&glowProjectile, creatures, sentries);
      handleItemInput(&player, &glowProjectile, creatures, sentries, gameState);
      updateFootstepSounds(&player, &mg, gameState);
      updateHealthSound(&player);
    }
    if (gameState == BOSS_STATE) {
      updateBoss(&boss, &player, bossMinions, bossHazards);
      updateMinions(bossMinions, &player);
      updateHazards(bossHazards, &player);

      // Stop boss music once boss death animation finishes
      static int bossWasActive = 1;
      if (bossWasActive && !boss.active) {
        stopBossBGMusic();
        bossWasActive = 0;
      }
      if (boss.active) {
        bossWasActive = 1;
      }

      // Hook player attack into boss hit detection
      // Normal overhead slash: trigger on active slashing frame
      if (player.state == ATTACK_OVERHEAD_SLASHING && player.frame == 3) {
        handlePlayerAttackBoss(&boss, &player, 0);
        handlePlayerAttackMinion(bossMinions, &player, 0);
      }
      // Slashwave: trigger while the state is active
      if (player.state == ATTACK_OVERHEAD_SLASHWAVE) {
        handlePlayerAttackBoss(&boss, &player, 1);
        handlePlayerAttackMinion(bossMinions, &player, 1);
      }
      // Downstab: trigger exactly once per slam (on the first frame of the
      // state)
      if (player.state == DOWNSTAB_ACTIVE && player.stateTimer == 1) {
        handlePlayerAttackBoss(&boss, &player, 0);
        handlePlayerAttackMinion(bossMinions, &player, 0);
      }
    }
  } else if (gameState == CAVE_STATE) {
    updateGame(&player, creatures, &bg, &mg, &camera, &gameState, pickups,
               sparkles);
    updateNPC(&npc, &player);
    updatePickups(pickups, &player);
    updateItemEffects(&player);
    handleItemInput(&player, &glowProjectile, creatures, sentries, gameState);
    updateFootstepSounds(&player, &mg, gameState);
    updateHealthSound(&player);
  }
}

void iKeyboard(unsigned char key) {
  printf("Key pressed: %d\n", key);

  if (key == '3') {
    gameState = LEVEL3_STATE;
    player.x = 200;
    player.y = LEVEL3_GROUND_Y;
    player.vy = 0;
    player.onGround = 1;
    setPlayerState(&player, IDLE);
    camera.x = 0;
    camera.targetX = 0;
    stopBGMusic();
    stopBossBGMusic();
    playBG2Music();
    return;
  } else if (key == '4') {
    gameState = BOSS_STATE;
    player.x = 200;
    player.y = BOSS_GROUND_Y;
    player.vy = 0;
    player.onGround = 1;
    camera.x = 0;
    camera.targetX = 0;
    setPlayerState(&player, IDLE);
    initBoss(&boss);
    initMinions(bossMinions);
    initHazards(bossHazards);
    stopBGMusic();
    stopBG2Music();
    playBossBGMusic();
    return;
  }

  if (gameState == TITLE_SCREEN_STATE) {
    if (key == ' ' || key == 13) {
      playStartButtonClickSound();
      gameState = LOADING_STATE;
      titleScreen.loadingTimer = 0;
      glutSetCursor(GLUT_CURSOR_INHERIT);
    }
  } else if (gameState == CREDITS_STATE || gameState == CONTROLS_STATE) {
    if (key == 27 || key == '\b') {
      playButtonClickSound();
      gameState = TITLE_SCREEN_STATE;
      glutSetCursor(GLUT_CURSOR_NONE);
    }
  } else if (gameState == CAVE_STATE) {
  }

  if (key == '3') {
    gameState = LEVEL3_STATE;
    player.x = 200;
    player.y = LEVEL3_GROUND_Y;
    player.vy = 0;
    player.onGround = 1;
    setPlayerState(&player, IDLE);
    camera.x = 0;
    camera.targetX = 0;
    stopBGMusic();
    stopBossBGMusic();
    playBG2Music();
  } else if (key == '4') {
    gameState = BOSS_STATE;
    player.x = 200;
    player.y = BOSS_GROUND_Y;
    player.vy = 0;
    player.onGround = 1;
    camera.x = 0;
    camera.targetX = 0;
    setPlayerState(&player, IDLE);
    initBoss(&boss);
    initMinions(bossMinions);
    initHazards(bossHazards);
    stopBGMusic();
    stopBG2Music();
    playBossBGMusic();
  }
}

void iMouseMove(int mx, int my) {
  printf("x=%d y=%d", mx, my);
  if (gameState == TITLE_SCREEN_STATE) {
    handleTitleMouseMove(&titleScreen, mx, my);
  }
}

void iPassiveMouseMove(int mx, int my) {
  if (gameState == TITLE_SCREEN_STATE || gameState == CREDITS_STATE ||
      gameState == CONTROLS_STATE) {
    handleTitleMouseMove(&titleScreen, mx, my);
  }
  if (gameState == TITLE_SCREEN_STATE) {
    updateButtonHoverSound(mx, my);
  }
}

void iMouse(int button, int state, int mx, int my) {
  if (gameState == TITLE_SCREEN_STATE || gameState == CREDITS_STATE ||
      gameState == CONTROLS_STATE) {
    handleTitleMouseClick(&titleScreen, button, state, mx, my, &gameState);
  }
}

void iSpecialKeyboard(unsigned char key) {}

int main() {
  srand(time(NULL));
  iInitialize(SCREEN_W, SCREEN_H, "Obscure Knight");

  initTitleScreen(&titleScreen);

  initPlayer(&player);
  initBackground(&bg);
  initMidground(&mg);
  initCamera(&camera);
  initCreatures(creatures);
  initSentries(sentries);
  initSparkles(sparkles);
  initHealthBar();
  initStaminaBar();

  initPlayerInventory(&player);
  initPickups(pickups);
  initGlowProjectile(&glowProjectile);

  initSounds();
  initBoss(&boss);
  initMinions(bossMinions);
  initHazards(bossHazards);

  loadImages();
  loadTitleTextures(&titleScreen);

  loadMidgroundTextures(&mg);
  setupTiles(&mg);

  glutSetCursor(GLUT_CURSOR_NONE);

  iSetTimer(20, animate);
  iStart();
  return 0;
}
