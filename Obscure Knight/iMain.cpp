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

int prevState = PLAYING_STATE;
int quitHovered = 0; 

void renderGameplayForState(int st) {
  if (st == PLAYING_STATE) {
    renderBackgroundWithCamera(&camera, backgroundTextures);
    renderMidground(&mg, &camera, st);
    renderCreatures(creatures, &camera);
    renderStaminaBar(&player);
    renderHealthBar(&player);
    renderPickups(pickups, &camera);
    renderInventoryUI(&player);
    renderGlowProjectile(&glowProjectile, &camera);
    renderPlayer(&player, &camera);
  } else if (st == LEVEL2_STATE) {
    renderBackgroundWithCamera(&camera, level2BackgroundTextures);
    renderMidground(&mg, &camera, st);
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
  } else if (st == LEVEL3_STATE) {
    renderBackgroundWithCamera(&camera, level3BackgroundTextures);
    renderMidground(&mg, &camera, st);
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
    renderTradeMenu(&traderNpc, &player);
  } else if (st == BOSS_STATE) {
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
  }
}

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
  } else if (gameState == BOSS_ENTRY_STATE) {
    if (bossEntryFrames[player.bossEntryFrame] != 0) {
      iShowImage(0, 0, SCREEN_W, SCREEN_H,
                 bossEntryFrames[player.bossEntryFrame]);
    }
  } else if (gameState == GAME_OVER_STATE) {
    if (gameOverFrames[player.gameOverFrame] != 0) {
      iShowImage(0, 0, SCREEN_W, SCREEN_H,
                 gameOverFrames[player.gameOverFrame]);
    }
  } else if (gameState == GAME_ENDING_STATE) {
    if (gameEndingFrames[player.gameEndingFrame] != 0) {
      iShowImage(0, 0, SCREEN_W, SCREEN_H,
                 gameEndingFrames[player.gameEndingFrame]);
    }
  } else if (gameState == CONGRATS_STATE) {
    if (congratsTex != 0) {
      iShowImage(0, 0, SCREEN_W, SCREEN_H, congratsTex);
    }
  } else if (gameState == PAUSED_STATE) {
    renderGameplayForState(prevState);
    if (quitBgTex != 0) {
      iShowImage(QUIT_BG_X, QUIT_BG_Y, QUIT_BG_W, QUIT_BG_H, quitBgTex);
    }
    if (quitYesTex != 0) {
      int yOff = (quitHovered == 1) ? QUIT_HOVER_OFFSET : 0;
      iShowImage(QUIT_YES_X, QUIT_YES_Y + yOff, QUIT_BTN_W, QUIT_BTN_H,
                 quitYesTex);
    }
    if (quitNoTex != 0) {
      int yOff = (quitHovered == 2) ? QUIT_HOVER_OFFSET : 0;
      iShowImage(QUIT_NO_X, QUIT_NO_Y + yOff, QUIT_BTN_W, QUIT_BTN_H,
                 quitNoTex);
    }
    if (quitHovered == 1 && quitArrow1Tex != 0) {
      iShowImage(QUIT_ARROW1_X, QUIT_ARROW1_Y, QUIT_ARROW_W, QUIT_ARROW_H,
                 quitArrow1Tex);
    }
    if (quitHovered == 2 && quitArrow2Tex != 0) {
      iShowImage(QUIT_ARROW2_X, QUIT_ARROW2_Y, QUIT_ARROW_W, QUIT_ARROW_H,
                 quitArrow2Tex);
    }
  }

  if (titleScreen.cursorTexture != 0) {
    iShowImage(titleScreen.mouseX, titleScreen.mouseY - 32, 32, 32,
               titleScreen.cursorTexture);
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

      if (bossDoor.opened && !player.hasPlayedBossEntry) {
        gameState = BOSS_ENTRY_STATE;
        player.hasPlayedBossEntry = 1;
        player.bossEntryFrame = 0;
        player.bossEntryTimer = 0;
        stopBG2Music();
        playBossEntryMusic();
      }

      static int traderEKeyWas = 0;
      int traderEKey = (GetAsyncKeyState('E') & 0x8000);
      if (traderEKey && !traderEKeyWas) {
        if (traderNpc.state == TRADER_PROMPT_INTERACT) {
          traderNpc.state = TRADER_TURN_STATE;
          traderNpc.frame = 0;
          traderNpc.stateTimer = 0;
        } else if (traderNpc.tradeMenuOpen) {
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

      static int bossWasActive = 1;
      static int batsDeadWaiting = 0;
      static int batsDeadTimer = 0;
      if (bossWasActive && !boss.active) {
        stopBossBGMusic();
        bossWasActive = 0;
        batsDeadWaiting = 0;
        batsDeadTimer = 0;
      }
      if (boss.active) {
        bossWasActive = 1;
      }

      if (!boss.active && !batsDeadWaiting && !anyBatAlive(bossMinions)) {
        batsDeadWaiting = 1;
        batsDeadTimer = 0;
      }
      if (batsDeadWaiting) {
        batsDeadTimer++;
        if (batsDeadTimer >= 100) {
          batsDeadWaiting = 0;
          gameState = GAME_ENDING_STATE;
          player.gameEndingFrame = 0;
          player.gameEndingTimer = 0;
          playGameEndingMusic();
        }
      }

      if (player.state == ATTACK_OVERHEAD_SLASHING && player.frame == 3) {
        handlePlayerAttackBoss(&boss, &player, 0);
        handlePlayerAttackMinion(bossMinions, &player, 0);
      }
      if (player.state == ATTACK_OVERHEAD_SLASHWAVE) {
        handlePlayerAttackBoss(&boss, &player, 1);
        handlePlayerAttackMinion(bossMinions, &player, 1);
      }
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
    if (player.gameOverTimer >= 3) {
      player.gameOverTimer = 0;
      if (player.gameOverFrame < 61) {
        player.gameOverFrame++;
      }
    }
  } else if (gameState == GAME_ENDING_STATE) {
    player.gameEndingTimer++;
    if (player.gameEndingTimer >= 4) {
      player.gameEndingTimer = 0;
      if (player.gameEndingFrame < GAME_ENDING_FRAMES - 1) {
        player.gameEndingFrame++;
        if (player.gameEndingFrame == GAME_ENDING_FRAMES - 1) {
          stopGameEndingMusic();
          gameState = CONGRATS_STATE;
          playCongratsMusic();
        }
      }
    }
  } else if (gameState == BOSS_ENTRY_STATE) {
    player.bossEntryTimer++;
    if (player.bossEntryTimer >= 2) {
      player.bossEntryTimer = 0;
      if (player.bossEntryFrame < BOSS_ENTRY_FRAMES - 1) {
        player.bossEntryFrame++;
      } else {
        gameState = BOSS_STATE;
        player.x = 200;
        player.y = BOSS_GROUND_Y;
        player.vy = 0;
        player.onGround = 1;
        setPlayerState(&player, IDLE);
        player.stateTimer = 0;
        camera.x = 0;
        camera.targetX = 0;
        initBoss(&boss);
        initMinions(bossMinions);
        initHazards(bossHazards);
        player.deaths = 0;
        stopBossEntryMusic();
        playBossBGMusic();
      }
    }
  }
}

void iKeyboard(unsigned char key) {
  printf("Key pressed: %d\n", key);

  if (gameState == GAME_OVER_STATE || gameState == GAME_ENDING_STATE ||
      gameState == CONGRATS_STATE) {
    if (gameState == GAME_ENDING_STATE)
      stopGameEndingMusic();
    if (gameState == CONGRATS_STATE)
      stopCongratsMusic();
    gameState = TITLE_SCREEN_STATE;
    player.deaths = 0;
    player.hasPlayedBossEntry = 0;
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
    initMidground(&mg);
    loadMidgroundTextures(&mg);
    setupTiles(&mg);
    stopBG2Music();
    stopBossBGMusic();
    stopBossEntryMusic();
    restartBGMusic();
    return;
  }

  if (key == 27 && (gameState == PLAYING_STATE || gameState == LEVEL2_STATE ||
                    gameState == LEVEL3_STATE || gameState == BOSS_STATE)) {
    prevState = gameState;
    gameState = PAUSED_STATE;
    quitHovered = 0;
    return;
  }
  if (gameState == PAUSED_STATE) {
    gameState = prevState;
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
}

void iMouseMove(int mx, int my) {
  titleScreen.mouseX = mx;
  titleScreen.mouseY = my;

  if (gameState == TITLE_SCREEN_STATE) {
    handleTitleMouseMove(&titleScreen, mx, my);
  }
}

void iPassiveMouseMove(int mx, int my) {
  titleScreen.mouseX = mx;
  titleScreen.mouseY = my;

  if (gameState == TITLE_SCREEN_STATE || gameState == CREDITS_STATE ||
      gameState == CONTROLS_STATE) {
    handleTitleMouseMove(&titleScreen, mx, my);
  }
  if (gameState == TITLE_SCREEN_STATE) {
    updateButtonHoverSound(mx, my);
  }
  if (gameState == PAUSED_STATE) {
    if (mx >= QUIT_YES_X && mx <= QUIT_YES_X + QUIT_BTN_W && my >= QUIT_YES_Y &&
        my <= QUIT_YES_Y + QUIT_BTN_H) {
      quitHovered = 1;
    } else if (mx >= QUIT_NO_X && mx <= QUIT_NO_X + QUIT_BTN_W &&
               my >= QUIT_NO_Y && my <= QUIT_NO_Y + QUIT_BTN_H) {
      quitHovered = 2;
    } else {
      quitHovered = 0;
    }
  }
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
  if (gameState == LEVEL3_STATE && traderNpc.tradeMenuOpen &&
      button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    handleTradeClick(&traderNpc, &player, mx, my);
  }
  if ((gameState == LEVEL3_STATE || gameState == BOSS_STATE) &&
      button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    handleEquippedIconClick(&player, mx, my);
  }

  if (gameState == PAUSED_STATE && button == GLUT_LEFT_BUTTON &&
      state == GLUT_DOWN) {
    if (mx >= QUIT_YES_X && mx <= QUIT_YES_X + QUIT_BTN_W && my >= QUIT_YES_Y &&
        my <= QUIT_YES_Y + QUIT_BTN_H) {
      gameState = TITLE_SCREEN_STATE;
      player.deaths = 0;
      player.hasPlayedBossEntry = 0;
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
      initMidground(&mg);
      loadMidgroundTextures(&mg);
      setupTiles(&mg);
      stopBG2Music();
      stopBossBGMusic();
      stopBossEntryMusic();
      restartBGMusic();
    } else if (mx >= QUIT_NO_X && mx <= QUIT_NO_X + QUIT_BTN_W &&
               my >= QUIT_NO_Y && my <= QUIT_NO_Y + QUIT_BTN_H) {
      gameState = prevState;
    }
  }

  if ((gameState == GAME_OVER_STATE || gameState == GAME_ENDING_STATE ||
       gameState == CONGRATS_STATE) &&
      button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (gameState == GAME_ENDING_STATE)
      stopGameEndingMusic();
    if (gameState == CONGRATS_STATE)
      stopCongratsMusic();
    gameState = TITLE_SCREEN_STATE;
    player.deaths = 0;
    player.hasPlayedBossEntry = 0;
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
    initMidground(&mg);
    loadMidgroundTextures(&mg);
    setupTiles(&mg);
    stopBG2Music();
    stopBossBGMusic();
    stopBossEntryMusic();
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
