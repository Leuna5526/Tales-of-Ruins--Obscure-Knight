#include "Sentry.hpp"
#include "background.hpp"
#include "bug.hpp"
#include "camera.hpp"
#include "game.hpp"
#include "healthbar.hpp"
#include "iGraphics.h"
#include "midground.hpp"
#include "player.hpp"
#include "staminabar.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include "title.hpp"
#include "sounds.hpp"
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

void iDraw() {
  iClear();

  if (gameState == TITLE_SCREEN_STATE) {
    renderTitleScreen(&titleScreen);
  } else if (gameState == CREDITS_STATE) {
    renderCredits(&titleScreen);
  } else if (gameState == CONTROLS_STATE) {
    renderControls(&titleScreen);
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
    renderSentries(sentries, &camera);
    renderSparkles(sparkles, &camera);
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
    renderPlayer(&player, &camera);
    renderDialogue(&npc);
  }
}

void animate() {
  if (gameState == TITLE_SCREEN_STATE) {
    updateTitleAnimation(&titleScreen);
  } else if (gameState == PLAYING_STATE) {
    updateGame(&player, creatures, &bg, &mg, &camera, &gameState, pickups);
    updatePickups(pickups, &player);
    updateItemEffects(&player);
    updateGlowProjectile(&glowProjectile, creatures, sentries);
    handleItemInput(&player, &glowProjectile, creatures, sentries, gameState);
    updateFootstepSounds(&player, &mg, gameState);
    updateHealthSound(&player);
  } else if (gameState == TUNNEL_STATE || gameState == LEVEL2_STATE) {
    updateGame(&player, creatures, &bg, &mg, &camera, &gameState, pickups);
    if (gameState == LEVEL2_STATE) {
      updateSentries(sentries, &player, pickups);
      updateSparkles(sparkles);
      updatePickups(pickups, &player);
      updateItemEffects(&player);
      updateGlowProjectile(&glowProjectile, creatures, sentries);
      handleItemInput(&player, &glowProjectile, creatures, sentries, gameState);
      updateFootstepSounds(&player, &mg, gameState);
      updateHealthSound(&player);
    }
  } else if (gameState == CAVE_STATE) {
    updateGame(&player, creatures, &bg, &mg, &camera, &gameState, pickups);
    updateNPC(&npc, &player);
    updateFootstepSounds(&player, &mg, gameState);
    updateHealthSound(&player);
  }
}

void iKeyboard(unsigned char key) {
  printf("Key pressed: %d\n", key);
  if (gameState == TITLE_SCREEN_STATE) {
    if (key == ' ' || key == 13) {
      playStartButtonClickSound();
      gameState = PLAYING_STATE;
      restartBGMusic();
    }
  } else if (gameState == CREDITS_STATE || gameState == CONTROLS_STATE) {
    if (key == 27 || key == '\b') {
      playButtonClickSound();
      gameState = TITLE_SCREEN_STATE;
    }
  } else if (gameState == CAVE_STATE) {
    // Input is handled asynchronously in updateCaveState via GetAsyncKeyState
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

void iSpecialKeyboard(unsigned char key) {
}

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

  loadImages();
  loadTitleTextures(&titleScreen);

  loadMidgroundTextures(&mg);
  setupTiles(&mg);

  iSetTimer(20, animate);
  iStart();
  return 0;
}
