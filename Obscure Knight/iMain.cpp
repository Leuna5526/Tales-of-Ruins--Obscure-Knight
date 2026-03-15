#include "Sentry.hpp"
#include "background.hpp"
#include "boss.hpp"
#include "bug.hpp"
#include "camera.hpp"
#include "game.hpp"
#include "grimmaster.hpp"
#include "healthbar.hpp"
#include "iGraphics.h"
#include "midground.hpp"
#include "player.hpp"
#include "sounds.hpp"
#include "staminabar.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include "title.hpp"
#include "tradernpc.hpp"
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
struct TraderNPC traderNpc;
struct GrimMaster grims[MAX_GRIMS];
struct GrimFireball grimFireballs[MAX_GRIM_FIREBALLS];
struct BossDoor bossDoor;
struct GreetNPC greetNpc;

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
    renderGreetNPC(&greetNpc, &camera);
    renderStaminaBar(&player);
    renderHealthBar(&player);
    renderPickups(pickups, &camera);
    renderInventoryUI(&player);
    renderGlowProjectile(&glowProjectile, &camera);
    renderPlayer(&player, &camera);
  } else if (gameState == LEVEL3_STATE) {
    renderBackgroundWithCamera(&camera, level3BackgroundTextures);
    renderMidground(&mg, &camera, gameState);
    renderBossDoor(&bossDoor, &camera);
    renderTraderNPC(&traderNpc, &camera);
    renderGrims(grims, &camera);
    renderGrimFireballs(grimFireballs, &camera);
    renderStaminaBar(&player);
    renderHealthBar(&player);
    renderPickups(pickups, &camera);
    renderInventoryUI(&player);
    renderGlowProjectile(&glowProjectile, &camera);
    renderPlayer(&player, &camera);
    renderEquippedIcons(&player);
    // Trade menu overlay on top of everything
    renderTradeMenu(&traderNpc, &player);
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
    renderEquippedIcons(&player);
  } else if (gameState == CAVE_STATE) {
    renderCaveBackground();
    renderStaminaBar(&player);
    renderHealthBar(&player);
    renderInventoryUI(&player);
    renderNPC(&npc, &camera);
    renderPickups(pickups, &camera);
    renderPlayer(&player, &camera);
    renderDialogue(&npc);
  } else if (gameState == GAME_OVER_STATE) {
    if (gameOverFrames[player.gameOverFrame] != 0) {
      iShowImage(0, 0, SCREEN_W, SCREEN_H, gameOverFrames[player.gameOverFrame]);
    }
  }

  // Global cursor draw - always on top
  if (titleScreen.cursorTexture != 0) {
    iShowImage(titleScreen.mouseX, titleScreen.mouseY - 32, 32, 32, titleScreen.cursorTexture);
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
      updateGreetNPC(&greetNpc);
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
    if (gameState == LEVEL3_STATE) {
      updateTraderNPC(&traderNpc, &player);
      updateGrims(grims, grimFireballs, &player, pickups);
      updateGrimFireballs(grimFireballs, &player);
      updateBossDoor(&bossDoor, &player);

      // Trader NPC keyboard interaction
      static int traderEKeyWas = 0;
      int traderEKey = (GetAsyncKeyState('E') & 0x8000);
      if (traderEKey && !traderEKeyWas) {
        if (traderNpc.state == TRADER_PROMPT_INTERACT) {
          traderNpc.state = TRADER_TURN_STATE;
          traderNpc.frame = 0;
          traderNpc.stateTimer = 0;
        } else if (traderNpc.tradeMenuOpen) {
          // Close trade menu with E
          traderNpc.tradeMenuOpen = 0;
          traderNpc.state = TRADER_WALK_BACK;
          traderNpc.facingRight = (traderNpc.initialX > traderNpc.x) ? 1 : 0;
          traderNpc.frame = 0;
          traderNpc.stateTimer = 0;
        }
      }
      traderEKeyWas = traderEKey;
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
  } else if (gameState == GAME_OVER_STATE) {
    player.gameOverTimer++;
    // roughly 60ms delay per frame (3 * 20ms)
    if (player.gameOverTimer >= 3) {
      player.gameOverTimer = 0;
      // Index 61 is frame 62, which is the last image before the black screens
      if (player.gameOverFrame < 61) {
        player.gameOverFrame++;
      }
    }
  }
}

void iKeyboard(unsigned char key) {
  printf("Key pressed: %d\n", key);

  // Game Over: any key returns to title screen (same as mouse click)
  if (gameState == GAME_OVER_STATE) {
    gameState = TITLE_SCREEN_STATE;
    player.deaths = 0;
    player.health = player.maxHealth;
    player.stamina = player.maxStamina;
    player.invincibilityTimer = 60;
    setPlayerState(&player, IDLE);
    initCreatures(creatures);
    initSentries(sentries);
    initBoss(&boss);
    initMinions(bossMinions);
    initHazards(bossHazards);
    initGrims(grims);
    initGrimFireballs(grimFireballs);
    initGreetNPC(&greetNpc);
    initPlayerInventory(&player);
    initTraderNPC(&traderNpc);
    initBossDoor(&bossDoor);
    player.x = 200;
    player.y = GROUND_Y;
    player.vy = 0;
    player.onGround = 1;
    camera.x = 0;
    camera.targetX = 0;
    bg.x = 0;
    // Reset Level 1 tiles
    initMidground(&mg);
    loadMidgroundTextures(&mg);
    setupTiles(&mg);
    // Reset music: stop gameplay tracks, restart title/bg music
    stopBG2Music();
    stopBossBGMusic();
    restartBGMusic();
    return;
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
    initTraderNPC(&traderNpc);
    initGrims(grims);
    initGrimFireballs(grimFireballs);
    initBossDoor(&bossDoor);
    // Setup Level 3 tiles
    mg.tileCount = 0;
    {
      mg.tileTexture1 = level3Tile1;
      mg.tileTexture2 = level3Tile2;
      mg.tiles[0].x = LEVEL3_TILE_1_X; mg.tiles[0].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_1_Y; mg.tiles[0].texture = level3Tile1;
      mg.tiles[0].width = LEVEL3_TILE_W; mg.tiles[0].height = LEVEL3_TILE_H; mg.tiles[0].active = 1; mg.tiles[0].isJumpThrough = 0; mg.tileCount++;
      mg.tiles[1].x = LEVEL3_TILE_2_X; mg.tiles[1].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_2_Y; mg.tiles[1].texture = level3Tile2;
      mg.tiles[1].width = LEVEL3_TILE_WIDE_W; mg.tiles[1].height = LEVEL3_TILE_H; mg.tiles[1].active = 1; mg.tiles[1].isJumpThrough = 0; mg.tileCount++;
      mg.tiles[2].x = LEVEL3_TILE_3_X; mg.tiles[2].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_3_Y; mg.tiles[2].texture = level3Tile1;
      mg.tiles[2].width = LEVEL3_TILE_W; mg.tiles[2].height = LEVEL3_TILE_H; mg.tiles[2].active = 1; mg.tiles[2].isJumpThrough = 0; mg.tileCount++;
      mg.tiles[3].x = LEVEL3_TILE_4_X; mg.tiles[3].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_4_Y; mg.tiles[3].texture = level3Tile2;
      mg.tiles[3].width = LEVEL3_TILE_WIDE_W; mg.tiles[3].height = LEVEL3_TILE_H; mg.tiles[3].active = 1; mg.tiles[3].isJumpThrough = 0; mg.tileCount++;
      mg.tiles[4].x = LEVEL3_TILE_5_X; mg.tiles[4].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_5_Y; mg.tiles[4].texture = level3Tile1;
      mg.tiles[4].width = LEVEL3_TILE_W; mg.tiles[4].height = LEVEL3_TILE_H; mg.tiles[4].active = 1; mg.tiles[4].isJumpThrough = 0; mg.tileCount++;
    }
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
    initTraderNPC(&traderNpc);
    initGrims(grims);
    initGrimFireballs(grimFireballs);
    initBossDoor(&bossDoor);
    mg.tileCount = 0;
    {
      mg.tileTexture1 = level3Tile1;
      mg.tileTexture2 = level3Tile2;
      mg.tiles[0].x = LEVEL3_TILE_1_X; mg.tiles[0].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_1_Y; mg.tiles[0].texture = level3Tile1;
      mg.tiles[0].width = LEVEL3_TILE_W; mg.tiles[0].height = LEVEL3_TILE_H; mg.tiles[0].active = 1; mg.tiles[0].isJumpThrough = 0; mg.tileCount++;
      mg.tiles[1].x = LEVEL3_TILE_2_X; mg.tiles[1].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_2_Y; mg.tiles[1].texture = level3Tile2;
      mg.tiles[1].width = LEVEL3_TILE_WIDE_W; mg.tiles[1].height = LEVEL3_TILE_H; mg.tiles[1].active = 1; mg.tiles[1].isJumpThrough = 0; mg.tileCount++;
      mg.tiles[2].x = LEVEL3_TILE_3_X; mg.tiles[2].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_3_Y; mg.tiles[2].texture = level3Tile1;
      mg.tiles[2].width = LEVEL3_TILE_W; mg.tiles[2].height = LEVEL3_TILE_H; mg.tiles[2].active = 1; mg.tiles[2].isJumpThrough = 0; mg.tileCount++;
      mg.tiles[3].x = LEVEL3_TILE_4_X; mg.tiles[3].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_4_Y; mg.tiles[3].texture = level3Tile2;
      mg.tiles[3].width = LEVEL3_TILE_WIDE_W; mg.tiles[3].height = LEVEL3_TILE_H; mg.tiles[3].active = 1; mg.tiles[3].isJumpThrough = 0; mg.tileCount++;
      mg.tiles[4].x = LEVEL3_TILE_5_X; mg.tiles[4].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_5_Y; mg.tiles[4].texture = level3Tile1;
      mg.tiles[4].width = LEVEL3_TILE_W; mg.tiles[4].height = LEVEL3_TILE_H; mg.tiles[4].active = 1; mg.tiles[4].isJumpThrough = 0; mg.tileCount++;
    }
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
  // Global cursor move tracking
  titleScreen.mouseX = mx;
  titleScreen.mouseY = my;

  if (gameState == TITLE_SCREEN_STATE) {
    handleTitleMouseMove(&titleScreen, mx, my);
  }
}

void iPassiveMouseMove(int mx, int my) {
  // Global cursor passive move tracking
  titleScreen.mouseX = mx;
  titleScreen.mouseY = my;

  if (gameState == TITLE_SCREEN_STATE || gameState == CREDITS_STATE ||
      gameState == CONTROLS_STATE) {
    handleTitleMouseMove(&titleScreen, mx, my);
  }
  if (gameState == TITLE_SCREEN_STATE) {
    updateButtonHoverSound(mx, my);
  }
  // Update trader mouse position for hover effects
  if (gameState == LEVEL3_STATE) {
    traderNpc.mouseX = mx;
    traderNpc.mouseY = my;
  }
}

void iMouse(int button, int state, int mx, int my) {
  if (gameState == TITLE_SCREEN_STATE || gameState == CREDITS_STATE ||
      gameState == CONTROLS_STATE) {
    handleTitleMouseClick(&titleScreen, button, state, mx, my, &gameState);
  }
  // Trade menu click
  if (gameState == LEVEL3_STATE && traderNpc.tradeMenuOpen &&
      button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    handleTradeClick(&traderNpc, &player, mx, my);
  }
  // Equipped icon click
  if ((gameState == LEVEL3_STATE || gameState == BOSS_STATE) &&
      button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    handleEquippedIconClick(&player, mx, my);
  }
  
  if (gameState == GAME_OVER_STATE && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    gameState = TITLE_SCREEN_STATE;
    player.deaths = 0;
    player.health = player.maxHealth;
    player.stamina = player.maxStamina;
    player.invincibilityTimer = 60;
    setPlayerState(&player, IDLE);
    initCreatures(creatures);
    initSentries(sentries);
    initBoss(&boss);
    initMinions(bossMinions);
    initHazards(bossHazards);
    initGrims(grims);
    initGrimFireballs(grimFireballs);
    initGreetNPC(&greetNpc);
    initPlayerInventory(&player);
    initTraderNPC(&traderNpc);
    initBossDoor(&bossDoor);
    player.x = 200;
    player.y = GROUND_Y;
    player.vy = 0;
    player.onGround = 1;
    camera.x = 0;
    camera.targetX = 0;
    bg.x = 0;
    // Reset Level 1 tiles
    initMidground(&mg);
    loadMidgroundTextures(&mg);
    setupTiles(&mg);
    // Reset music: stop gameplay tracks, restart title/bg music
    stopBG2Music();
    stopBossBGMusic();
    restartBGMusic();
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
  initTraderNPC(&traderNpc);
  initGrims(grims);
  initGrimFireballs(grimFireballs);
  initBossDoor(&bossDoor);
  initGreetNPC(&greetNpc);

  loadImages();
  loadTitleTextures(&titleScreen);

  loadMidgroundTextures(&mg);
  setupTiles(&mg);

  glutSetCursor(GLUT_CURSOR_NONE);

  iSetTimer(20, animate);
  iStart();
  return 0;
}
