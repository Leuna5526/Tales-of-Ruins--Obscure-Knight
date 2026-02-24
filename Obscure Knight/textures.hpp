#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include "config.hpp"
#include "iGraphics.h"
#include <cstdio>

unsigned int idle[IDLE_FRAMES];
unsigned int walkLeft[WALK_FRAMES];
unsigned int walkRight[WALK_FRAMES];
unsigned int turnLeft[TURN_FRAMES];
unsigned int turnRight[TURN_FRAMES];
unsigned int jumpLeft[JUMP_LEFT_FRAMES];
unsigned int jumpRight[JUMP_RIGHT_FRAMES];
unsigned int fallLeft[FALL_LEFT_FRAMES];
unsigned int fallRight[FALL_RIGHT_FRAMES];
unsigned int landLeft[LAND_FRAMES];
unsigned int landRight[LAND_FRAMES];

unsigned int dashLeft[DASH_LEFT_FRAMES];
unsigned int dashRight[DASH_RIGHT_FRAMES];

unsigned int downstabAnticipate[DOWNSTAB_ANTICIPATE_FRAMES];
unsigned int downstabSlam[DOWNSTAB_SLAM_FRAMES];
unsigned int downstabLand[DOWNSTAB_LAND_FRAMES];

unsigned int overheadAttackRecoverLeft[OVERHEAD_ATTACK_RECOVER_FRAMES];
unsigned int overheadAttackRecoverRight[OVERHEAD_ATTACK_RECOVER_FRAMES];
unsigned int overheadAttackSlashingLeft[OVERHEAD_ATTACK_SLASHING_FRAMES];
unsigned int overheadAttackSlashingRight[OVERHEAD_ATTACK_SLASHING_FRAMES];

unsigned int evadeLeftPre[EVADE_LEFT_PRE_FRAMES];
unsigned int evadeLeftActive[EVADE_LEFT_ACTIVE_FRAMES];
unsigned int evadeLeftRecover[EVADE_LEFT_RECOVER_FRAMES];
unsigned int evadeRightPre[EVADE_RIGHT_PRE_FRAMES];
unsigned int evadeRightActive[EVADE_RIGHT_ACTIVE_FRAMES];
unsigned int evadeRightRecover[EVADE_RIGHT_RECOVER_FRAMES];

unsigned int death[DEATH_FRAMES];

unsigned int wakePart1[WAKE_FRAMES_PART1];
unsigned int wakePart2[WAKE_FRAMES_PART2];
unsigned int wakePart3[WAKE_FRAMES_PART3];
unsigned int roar[ROAR_FRAMES];
unsigned int roarActive[ROAR_ACTIVE_FRAMES];
unsigned int roarEnd[ROAR_END_FRAMES];

unsigned int overheadAttackSlashwaveLeft[OVERHEAD_ATTACK_SLASHWAVE_FRAMES];
unsigned int overheadAttackSlashwaveRight[OVERHEAD_ATTACK_SLASHWAVE_FRAMES];

unsigned int backgroundTextures[BG_SEGMENTS];
unsigned int level2BackgroundTextures[BG_SEGMENTS];
unsigned int tunnelTextures[2];

unsigned int creatureNormal[CREATURE_NORMAL_FRAMES];
unsigned int creatureFlyL[CREATURE_FLYL_FRAMES];
unsigned int creatureFlyR[CREATURE_FLYR_FRAMES];
unsigned int creatureTurn[CREATURE_TURN_FRAMES];
unsigned int creatureAttack[CREATURE_ATTACK_FRAMES];
unsigned int creatureBurst[CREATURE_BURST_FRAMES];
unsigned int creatureDamage[CREATURE_DAMAGE_FRAMES];

unsigned int sentryIdle[SENTRY_IDLE_FRAMES];
unsigned int sentryWalkL[SENTRY_WALKL_FRAMES];
unsigned int sentryWalkR[SENTRY_WALKR_FRAMES];
unsigned int sentryRunL[SENTRY_RUNL_FRAMES];
unsigned int sentryRunR[SENTRY_RUNR_FRAMES];
unsigned int sentryAttackL[SENTRY_ATTACKL_FRAMES];
unsigned int sentryAttackR[SENTRY_ATTACKR_FRAMES];
unsigned int sentrySlashL[SENTRY_SLASHL_FRAMES];
unsigned int sentrySlashR[SENTRY_SLASHR_FRAMES];
unsigned int sentryTurnL[SENTRY_TURNL_FRAMES];
unsigned int sentryTurnR[SENTRY_TURNR_FRAMES];
unsigned int sentryWake[SENTRY_WAKE_FRAMES];
unsigned int sentryDeath[SENTRY_DEATH_FRAMES];
unsigned int sentryDeathAir[SENTRY_DEATHAIR_FRAMES];
unsigned int sentryDamage[SENTRY_DAMAGE_FRAMES];

unsigned int sparkleTextures[SPARKLE_FRAMES];

unsigned int level2TileFlat = 0;
unsigned int level2Tile1 = 0;

unsigned int inventoryBaseTex = 0;
unsigned int inventoryHeartTex = 0;
unsigned int inventoryPotionTex = 0;
unsigned int inventoryPowerTex = 0;
unsigned int pickableHeartTex = 0;
unsigned int pickablePotionTex = 0;
unsigned int pickablePowerTex = 0;
unsigned int inventoryGainTex = 0;
unsigned int pickableGainTex = 0;
unsigned int glowTextures[GLOW_FRAMES];
unsigned int caveTexture = 0;

void loadSet(unsigned int *arr, int n, const char *fmt) {
  char name[64];
  for (int i = 0; i < n; i++) {
    sprintf_s(name, fmt, i + 1);
    arr[i] = iLoadImage(name);
  }
}

void loadBackgroundTexture() {
  loadSet(backgroundTextures, BG_SEGMENTS, "Assets/Background/bg (%d).png");
}

void loadCreatureTextures() {
  loadSet(creatureNormal, CREATURE_NORMAL_FRAMES, "Assets/Bug/Normal/%d.png");
  loadSet(creatureFlyL, CREATURE_FLYL_FRAMES, "Assets/Bug/FlyL/%d.png");
  loadSet(creatureFlyR, CREATURE_FLYR_FRAMES, "Assets/Bug/FlyR/%d.png");
  loadSet(creatureTurn, CREATURE_TURN_FRAMES, "Assets/Bug/Turn/%d.png");
  loadSet(creatureAttack, CREATURE_ATTACK_FRAMES, "Assets/Bug/Attack/%d.png");
  loadSet(creatureBurst, CREATURE_BURST_FRAMES, "Assets/Bug/Burst/%d.png");
  loadSet(creatureDamage, CREATURE_DAMAGE_FRAMES, "Assets/Bug/Damage/%d.png");
}

void loadSentryTextures() {
  loadSet(sentryIdle, SENTRY_IDLE_FRAMES,
          "Assets/Level 2/Sentry/Idle/idle (%d).png");
  loadSet(sentryWalkL, SENTRY_WALKL_FRAMES,
          "Assets/Level 2/Sentry/WalkL/walk (%d).png");
  loadSet(sentryWalkR, SENTRY_WALKR_FRAMES,
          "Assets/Level 2/Sentry/WalkR/walk (%d).png");
  loadSet(sentryRunL, SENTRY_RUNL_FRAMES,
          "Assets/Level 2/Sentry/RunL/run (%d).png");
  loadSet(sentryRunR, SENTRY_RUNR_FRAMES,
          "Assets/Level 2/Sentry/RunR/run (%d).png");
  loadSet(sentryAttackL, SENTRY_ATTACKL_FRAMES,
          "Assets/Level 2/Sentry/AttackL/attack (%d).png");
  loadSet(sentryAttackR, SENTRY_ATTACKR_FRAMES,
          "Assets/Level 2/Sentry/AttackR/attack (%d).png");
  loadSet(sentrySlashL, SENTRY_SLASHL_FRAMES,
          "Assets/Level 2/Sentry/SlashL/slash (%d).png");
  loadSet(sentrySlashR, SENTRY_SLASHR_FRAMES,
          "Assets/Level 2/Sentry/SlashR/slash (%d).png");
  loadSet(sentryTurnL, SENTRY_TURNL_FRAMES,
          "Assets/Level 2/Sentry/TurnL/turn (%d).png");
  loadSet(sentryTurnR, SENTRY_TURNR_FRAMES,
          "Assets/Level 2/Sentry/TurnR/turn (%d).png");
  loadSet(sentryWake, SENTRY_WAKE_FRAMES,
          "Assets/Level 2/Sentry/Wake/wake (%d).png");
  loadSet(sentryDeath, SENTRY_DEATH_FRAMES,
          "Assets/Level 2/Sentry/Death/death (%d).png");
  loadSet(sentryDeathAir, SENTRY_DEATHAIR_FRAMES,
          "Assets/Level 2/Sentry/DeathAir/death (%d).png");
  loadSet(sentryDamage, SENTRY_DAMAGE_FRAMES,
          "Assets/Level 2/Sentry/Damage/%d.png");
}

void loadSparkleTextures() {
  loadSet(sparkleTextures, SPARKLE_FRAMES,
          "Assets/Level 2/Sparkle/sparkle (%d).png");
}

void loadLevel2TileTextures() {
  level2TileFlat = iLoadImage("Assets/Level 2/bg/flattile.png");
  level2Tile1 = iLoadImage("Assets/Level 2/bg/tile (1).png");
}

void loadInventoryTextures() {
  inventoryBaseTex = iLoadImage("Assets/UI/Inventory/inventory.png");
  inventoryHeartTex = iLoadImage("Assets/UI/Inventory/heart.png");
  inventoryPotionTex = iLoadImage("Assets/UI/Inventory/potion.png");
  inventoryPowerTex = iLoadImage("Assets/UI/Inventory/power.png");
  pickableHeartTex = iLoadImage("Assets/UI/Pickables/heart.png");
  pickablePotionTex = iLoadImage("Assets/UI/Pickables/potion.png");
  pickablePowerTex = iLoadImage("Assets/UI/Pickables/power.png");
  inventoryGainTex = iLoadImage("Assets/UI/Inventory/gain.png");
  pickableGainTex = iLoadImage("Assets/UI/Pickables/gain.png");
  loadSet(glowTextures, GLOW_FRAMES, "Assets/UI/Glow/glow (%d).png");
}

void loadImages() {
  loadSet(idle, IDLE_FRAMES, "Assets/mc/idle/%d.png");
  loadSet(walkLeft, WALK_FRAMES, "Assets/mc/walk/walk L/%d.png");
  loadSet(walkRight, WALK_FRAMES, "Assets/mc/walk/walk R/%d.png");
  loadSet(turnLeft, TURN_FRAMES, "Assets/mc/turn/%d.png");
  loadSet(turnRight, TURN_FRAMES, "Assets/mc/turn/%d.png");
  loadSet(jumpLeft, JUMP_LEFT_FRAMES, "Assets/mc/jump/jump L/jump/%d.png");
  loadSet(jumpRight, JUMP_RIGHT_FRAMES, "Assets/mc/jump/jump R/jump/%d.png");
  loadSet(fallLeft, FALL_LEFT_FRAMES, "Assets/mc/fall/fall L/%d.png");
  loadSet(fallRight, FALL_RIGHT_FRAMES, "Assets/mc/fall/fall R/%d.png");
  loadSet(landLeft, LAND_FRAMES, "Assets/mc/fall/fall L/%d.png");
  loadSet(landRight, LAND_FRAMES, "Assets/mc/fall/fall R/%d.png");

  loadSet(dashLeft, DASH_LEFT_FRAMES,
          "Assets/mc/dash/dash L/attack/part 1/%d.png");
  loadSet(dashRight, DASH_RIGHT_FRAMES,
          "Assets/mc/dash/dash R/attack/part 1/%d.png");

  loadSet(downstabAnticipate, DOWNSTAB_ANTICIPATE_FRAMES,
          "Assets/mc/downstab/anticipate/%d.png");
  loadSet(downstabSlam, DOWNSTAB_SLAM_FRAMES, "Assets/mc/downstab/slam/%d.png");
  loadSet(downstabLand, DOWNSTAB_LAND_FRAMES, "Assets/mc/downstab/land/%d.png");

  loadSet(overheadAttackRecoverLeft, OVERHEAD_ATTACK_RECOVER_FRAMES,
          "Assets/mc/slash/slash L/recover/%d.png");
  loadSet(overheadAttackRecoverRight, OVERHEAD_ATTACK_RECOVER_FRAMES,
          "Assets/mc/slash/slash R/recover/%d.png");
  loadSet(overheadAttackSlashingLeft, OVERHEAD_ATTACK_SLASHING_FRAMES,
          "Assets/mc/slash/slash L/slashing/%d.png");
  loadSet(overheadAttackSlashingRight, OVERHEAD_ATTACK_SLASHING_FRAMES,
          "Assets/mc/slash/slash R/slashing/%d.png");
  loadSet(overheadAttackSlashwaveLeft, OVERHEAD_ATTACK_SLASHWAVE_FRAMES,
          "Assets/mc/slash/slash L/slashwave/%d.png");
  loadSet(overheadAttackSlashwaveRight, OVERHEAD_ATTACK_SLASHWAVE_FRAMES,
          "Assets/mc/slash/slash R/slashwave/%d.png");

  loadSet(evadeLeftPre, EVADE_LEFT_PRE_FRAMES,
          "Assets/mc/evade/evade anticipation/%d.png");
  loadSet(evadeLeftActive, EVADE_LEFT_ACTIVE_FRAMES,
          "Assets/mc/evade/evade/%d.png");
  loadSet(evadeLeftRecover, EVADE_LEFT_RECOVER_FRAMES,
          "Assets/mc/evade/evade recover/%d.png");
  loadSet(evadeRightPre, EVADE_RIGHT_PRE_FRAMES,
          "Assets/mc/evade/evade anticipation/%d.png");
  loadSet(evadeRightActive, EVADE_RIGHT_ACTIVE_FRAMES,
          "Assets/mc/evade/evade/%d.png");
  loadSet(evadeRightRecover, EVADE_RIGHT_RECOVER_FRAMES,
          "Assets/mc/evade/evade recover/%d.png");

  loadSet(death, DEATH_FRAMES, "Assets/mc/death/%d.png");

  loadSet(wakePart1, WAKE_FRAMES_PART1, "Assets/mc/wake/wake part 1/%d.png");
  loadSet(wakePart2, WAKE_FRAMES_PART2, "Assets/mc/wake/wake part 2/%d.png");
  loadSet(wakePart3, WAKE_FRAMES_PART3, "Assets/mc/wake/wake part 3/%d.png");
  loadSet(roar, ROAR_FRAMES, "Assets/mc/roar/roar start/%d.png");
  loadSet(roarActive, ROAR_ACTIVE_FRAMES, "Assets/mc/roar/roar/%d.png");
  loadSet(roarEnd, ROAR_END_FRAMES, "Assets/mc/roar/roar end/%d.png");

  loadBackgroundTexture();
  loadSet(level2BackgroundTextures, BG_SEGMENTS,
          "Assets/Level 2/bg/bg (%d).png");
  tunnelTextures[0] = iLoadImage("Assets/tunnel/t1.png");
  tunnelTextures[1] = iLoadImage("Assets/tunnel/t2.png");
  loadCreatureTextures();
  loadSentryTextures();
  loadSparkleTextures();
  loadLevel2TileTextures();
  loadInventoryTextures();
  caveTexture = iLoadImage("Assets/Level 2/bg/cave.png");
}

#endif
