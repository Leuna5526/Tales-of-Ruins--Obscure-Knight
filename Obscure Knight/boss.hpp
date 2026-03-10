#ifndef BOSS_HPP
#define BOSS_HPP

#include "config.hpp"
#include "iGraphics.h"
#include "player.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <stdlib.h>

// ============================================================
// HELPERS
// ============================================================

static int bossRectsOverlap(int ax, int ay, int aw, int ah, int bx, int by,
                            int bw, int bh) {
  return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}

// Choose a random teleport X coordinate on the ground (not the spawn end)
static int randomTeleportX() {
  int range = BOSS_RAND_TELEPORT_MAX - BOSS_RAND_TELEPORT_MIN;
  return BOSS_RAND_TELEPORT_MIN + (rand() % range);
}

// Advance an animation frame
static void bossAnimTick(int *frame, int *animTimer, int totalFrames,
                         int speed) {
  (*animTimer)++;
  if (*animTimer >= speed) {
    *animTimer = 0;
    (*frame)++;
    if (*frame >= totalFrames)
      *frame = 0;
  }
}

// Advance an animation frame but stay on last frame (non-looping)
static int bossAnimTickOnce(int *frame, int *animTimer, int totalFrames,
                            int speed) {
  if (*frame >= totalFrames - 1)
    return 1; // done
  (*animTimer)++;
  if (*animTimer >= speed) {
    *animTimer = 0;
    (*frame)++;
  }
  return (*frame >= totalFrames - 1);
}

// ============================================================
// BOSS INIT
// ============================================================

void initBoss(struct Boss *boss) {
  boss->x = BOSS_SPAWN_X;
  boss->y = BOSS_GROUND_Y;
  boss->frame = 0;
  boss->animTimer = 0;
  boss->active = 1;
  boss->facingRight = 0; // faces left (towards player start)
  boss->state = BOSS_IDLE_STATE;
  boss->stateTimer = 0;
  boss->currentHealth = BOSS_MAX_HEALTH;
  boss->maxHealth = BOSS_MAX_HEALTH;
  boss->hasUsedTrap = 0;
  boss->teleportTargetX = BOSS_SPAWN_X;
  boss->phase = 1;
  boss->riseY = 0;
  boss->invincibilityTimer = 0;
  boss->intendedNextState = BOSS_IDLE_STATE;
}

void initMinions(struct BossMinion minions[]) {
  for (int i = 0; i < MAX_BOSS_MINIONS; i++) {
    minions[i].active = 0;
    minions[i].health = 0;
    minions[i].isDying = 0;
    minions[i].deathTimer = 0;
    minions[i].frame = 0;
    minions[i].animTimer = 0;
    minions[i].invincibilityTimer = 0;
  }
}

void initHazards(struct BossHazard hazards[]) {
  for (int i = 0; i < MAX_BOSS_HAZARDS; i++) {
    hazards[i].active = 0;
    hazards[i].frame = 0;
    hazards[i].animTimer = 0;
    hazards[i].hasHit = 0;
  }
}

// ============================================================
// BOSS STATE TRANSITIONS
// ============================================================

static void bossSetState(struct Boss *boss, enum BossState s) {
  boss->state = s;
  boss->stateTimer = 0;
  boss->frame = 0;
  boss->animTimer = 0;
}

// Determines if player is to the left of boss
static int playerIsLeft(struct Boss *boss, struct Player *player) {
  return player->x < boss->x;
}

// Start teleport to a target x
static void bossTeleportTo(struct Boss *boss, int targetX) {
  boss->teleportTargetX = targetX;
  bossSetState(boss, BOSS_TELEPORT_OUT_STATE);
}

// Spawn a FireBat in the minions array
static void spawnFireBat(struct BossMinion minions[], int bossX, int bossY,
                         int playerX) {
  for (int i = 0; i < MAX_BOSS_MINIONS; i++) {
    if (!minions[i].active) {
      minions[i].active = 1;
      minions[i].x = bossX;
      minions[i].y = bossY + 20;
      minions[i].health = FIREBAT_MAX_HEALTH;
      minions[i].isDying = 0;
      minions[i].deathTimer = 0;
      minions[i].frame = 0;
      minions[i].animTimer = 0;
      minions[i].invincibilityTimer = 0;
      minions[i].facingRight = (playerX > bossX) ? 1 : 0;
      minions[i].type = MINION_FIREBAT;
      break;
    }
  }
}

// Spawn 2 bats after boss death
static void spawnBats(struct BossMinion minions[], int bossX, int bossY,
                      int playerX) {
  int spawned = 0;
  for (int i = 0; i < MAX_BOSS_MINIONS && spawned < 2; i++) {
    if (!minions[i].active) {
      minions[i].active = 1;
      minions[i].x = bossX + (spawned == 0 ? -40 : 40);
      minions[i].y = bossY + 30;
      minions[i].health = BAT_MAX_HEALTH;
      minions[i].isDying = 0;
      minions[i].deathTimer = 0;
      minions[i].frame = 0;
      minions[i].animTimer = 0;
      minions[i].invincibilityTimer = 0;
      minions[i].facingRight = (playerX > minions[i].x) ? 1 : 0;
      minions[i].type = MINION_BAT;
      spawned++;
    }
  }
}

// Spawn spikes at up to 2 random ground X locations (avoids boss position)
static void spawnSpikes(struct BossHazard hazards[]) {
  int spawned = 0;
  for (int i = 0; i < MAX_BOSS_HAZARDS && spawned < 2; i++) {
    if (!hazards[i].active) {
      hazards[i].active = 1;
      // Spawn spikes across the arena, away from the extreme edges
      hazards[i].x = 150 + (rand() % (BOSS_SPAWN_X - 250));
      hazards[i].y = BOSS_GROUND_Y;
      hazards[i].frame = 0;
      hazards[i].animTimer = 0;
      hazards[i].hasHit = 0;
      hazards[i].type = HAZARD_SPIKE;
      spawned++;
    }
  }
}

// Spawn a Trap exactly where the player stands
static void spawnTrap(struct BossHazard hazards[], int playerX) {
  for (int i = 0; i < MAX_BOSS_HAZARDS; i++) {
    if (!hazards[i].active) {
      hazards[i].active = 1;
      hazards[i].x = playerX;
      hazards[i].y = BOSS_GROUND_Y;
      hazards[i].frame = 0;
      hazards[i].animTimer = 0;
      hazards[i].hasHit = 0;
      hazards[i].type = HAZARD_TRAP;
      break;
    }
  }
}

// Returns 1 if any FireBat is still alive
static int anyFireBatAlive(struct BossMinion minions[]) {
  for (int i = 0; i < MAX_BOSS_MINIONS; i++) {
    if (minions[i].active && !minions[i].isDying &&
        minions[i].type == MINION_FIREBAT)
      return 1;
  }
  return 0;
}

// Returns 1 if any spike/trap hazard is still animating
static int anyHazardActive(struct BossHazard hazards[]) {
  for (int i = 0; i < MAX_BOSS_HAZARDS; i++) {
    if (hazards[i].active)
      return 1;
  }
  return 0;
}

// Pick the next attack at random, considering boss phase
static enum BossState pickNextAttack(struct Boss *boss) {
  int r = rand() % 100;

  if (boss->phase >= 3 && !boss->hasUsedTrap) {
    // Trap attack — one time only at <= 25%
    return BOSS_TRAP_STATE;
  }

  if (boss->phase >= 2) {
    // Phase 2 extra spike chance
    if (r < 30)
      return BOSS_SPIKE_STATE;
    if (r < 50)
      return BOSS_WALK_STATE;
    if (r < 75)
      return BOSS_DASH_STATE; // Increased dash chance (50-75 = 25%)
    if (r < 90)
      return BOSS_CAST_STATE;
    return BOSS_TELEPORT_OUT_STATE; // random teleport
  }

  // Phase 1: walk (35%), dash (30%), cast (20%), teleport (15%)
  if (r < 35)
    return BOSS_WALK_STATE;
  if (r < 65)
    return BOSS_DASH_STATE; // Increased dash chance
  if (r < 85)
    return BOSS_CAST_STATE;
  return BOSS_TELEPORT_OUT_STATE;
}

// Forward declaration
void clearAllTraps(struct BossHazard hazards[]);

// ============================================================
// UPDATE BOSS
// ============================================================

void updateBoss(struct Boss *boss, struct Player *player,
                struct BossMinion minions[], struct BossHazard hazards[]) {
  if (!boss->active)
    return;

  // Update phase
  int hpPct = (boss->currentHealth * 100) / boss->maxHealth;
  if (hpPct <= 25 && boss->phase < 3)
    boss->phase = 3;
  else if (hpPct <= 50 && boss->phase < 2)
    boss->phase = 2;

  // Cool down invincibility
  if (boss->invincibilityTimer > 0)
    boss->invincibilityTimer--;

  // Face player by default while idle / walking
  boss->facingRight = (player->x < boss->x) ? 0 : 1;

  boss->stateTimer++;

  switch (boss->state) {

  // ─── IDLE ───────────────────────────────────────────────
  case BOSS_IDLE_STATE: {
    bossAnimTick(&boss->frame, &boss->animTimer, BOSS_IDLE_FRAMES,
                 BOSS_ANIM_SPEED);

    // Don't start fighting until player is close
    int dist = player->x - boss->x;
    if (dist < 0)
      dist = -dist;
    if (dist > BOSS_DETECTION_RANGE && boss->phase == 1 &&
        boss->stateTimer < BOSS_IDLE_DURATION)
      break;

    if (boss->stateTimer >= BOSS_IDLE_DURATION) {
      enum BossState next = pickNextAttack(boss);
      if (next == BOSS_TELEPORT_OUT_STATE) {
        // Random teleport
        boss->intendedNextState = BOSS_IDLE_STATE;
        bossTeleportTo(boss, randomTeleportX());
      } else if (next == BOSS_SPIKE_STATE) {
        boss->intendedNextState = BOSS_SPIKE_STATE;
        bossTeleportTo(boss, BOSS_SPAWN_X);
      } else if (next == BOSS_CAST_STATE) {
        boss->intendedNextState = BOSS_CAST_STATE;
        bossTeleportTo(boss, BOSS_SPAWN_X);
      } else if (next == BOSS_TRAP_STATE && boss->phase >= 3 &&
                 !boss->hasUsedTrap) {
        boss->intendedNextState = BOSS_TRAP_STATE;
        bossTeleportTo(boss, BOSS_SPAWN_X);
      } else {
        bossSetState(boss, next);
      }
    }
    break;
  }

  // ─── WALK ────────────────────────────────────────────────
  case BOSS_WALK_STATE: {
    bossAnimTick(&boss->frame, &boss->animTimer, BOSS_WALK_FRAMES,
                 BOSS_ANIM_SPEED);

    // Move towards player
    if (player->x < boss->x)
      boss->x -= BOSS_WALK_SPEED;
    else
      boss->x += BOSS_WALK_SPEED;

    // Close enough → slash
    int dist = player->x - boss->x;
    if (dist < 0)
      dist = -dist;
    if (dist <= BOSS_SLASH_RANGE) {
      bossSetState(boss, BOSS_SLASH_STATE);
    }
    break;
  }

  // ─── SLASH ───────────────────────────────────────────────
  case BOSS_SLASH_STATE: {
    int done = bossAnimTickOnce(&boss->frame, &boss->animTimer,
                                BOSS_SLASH_FRAMES, BOSS_ANIM_SPEED);

    // Hit player on frame 4 of slash
    if (boss->frame == 4) {
      int dist = player->x - boss->x;
      if (dist < 0)
        dist = -dist;
      if (dist <= BOSS_SLASH_RANGE + 30 && player->invincibilityTimer == 0) {
        player->health -= BOSS_SLASH_PLAYER_DAMAGE;
        if (player->health < 0)
          player->health = 0;
        player->invincibilityTimer = 60;
      }
    }

    if (done) {
      bossSetState(boss, BOSS_IDLE_STATE);
      player->isTrapped = 0;
      clearAllTraps(hazards);
    }
    break;
  }

  // ─── DASH ────────────────────────────────────────────────
  case BOSS_DASH_STATE: {
    // Animate frames 4-6 (indices 3-5) in a loop while dashing
    boss->animTimer++;
    if (boss->animTimer >= BOSS_ANIM_SPEED) {
      boss->animTimer = 0;
      boss->frame++;
      if (boss->frame < 3)
        boss->frame = 3; // start at frame 4 (index 3)
      if (boss->frame > 5)
        boss->frame = 3; // loop 4-6
    }

    // Dash towards player at high speed
    int dir = (player->x < boss->x) ? -1 : 1;
    boss->x += dir * BOSS_DASH_SPEED;

    // Collision check
    if (bossRectsOverlap(boss->x, boss->y, BOSS_SIZE_W, BOSS_SIZE_H, player->x,
                         player->y, (int)(SPRITE_SIZE * SCALE),
                         (int)(SPRITE_SIZE * SCALE))) {
      if (player->invincibilityTimer == 0) {
        player->health -= BOSS_DASH_PLAYER_DAMAGE;
        if (player->health < 0)
          player->health = 0;
        player->invincibilityTimer = 60;
      }
      // Immediately slash after dash hit
      bossSetState(boss, BOSS_SLASH_STATE);
      break;
    }

    // If we overshoot / reach player area without collision — slash anyway
    int dist = player->x - boss->x;
    if (dist < 0)
      dist = -dist;
    if (dist <= BOSS_SLASH_RANGE) {
      bossSetState(boss, BOSS_SLASH_STATE);
    }

    // Clamp to map bounds
    if (boss->x < 100)
      boss->x = 100;
    if (boss->x > BOSS_SPAWN_X + 50)
      boss->x = BOSS_SPAWN_X + 50;
    break;
  }

  // ─── CAST (spawn FireBat) ────────────────────────────────
  case BOSS_CAST_STATE: {
    int done = bossAnimTickOnce(&boss->frame, &boss->animTimer,
                                BOSS_CAST_FRAMES, BOSS_ANIM_SPEED);
    if (done) {
      spawnFireBat(minions, boss->x, boss->y, player->x);
      bossSetState(boss, BOSS_WAIT_MINION_STATE);
    }
    break;
  }

  // ─── WAIT FOR MINION TO DIE ──────────────────────────────
  case BOSS_WAIT_MINION_STATE: {
    bossAnimTick(&boss->frame, &boss->animTimer, BOSS_IDLE_FRAMES,
                 BOSS_ANIM_SPEED);
    if (!anyFireBatAlive(minions)) {
      bossSetState(boss, BOSS_IDLE_STATE);
    }
    break;
  }

  // ─── TELEPORT OUT ────────────────────────────────────────
  case BOSS_TELEPORT_OUT_STATE: {
    int done = bossAnimTickOnce(&boss->frame, &boss->animTimer,
                                BOSS_TELEPORT_FRAMES, BOSS_ANIM_SPEED);
    if (done) {
      // Actually relocate
      boss->x = boss->teleportTargetX;
      bossSetState(boss, BOSS_TELEPORT_IN_STATE);
    }
    break;
  }

  // ─── TELEPORT IN ─────────────────────────────────────────
  case BOSS_TELEPORT_IN_STATE: {
    int done = bossAnimTickOnce(&boss->frame, &boss->animTimer,
                                BOSS_TELEPORT_FRAMES, BOSS_ANIM_SPEED);
    if (done) {
      // Determine what to do based on intended state
      if (boss->intendedNextState == BOSS_TRAP_STATE && boss->phase >= 3 &&
          !boss->hasUsedTrap) {
        // Trap sequence: spawn trap on player, then walk
        spawnTrap(hazards, player->x);
        boss->hasUsedTrap = 1;
        bossSetState(boss, BOSS_TRAP_STATE);
      } else if (boss->intendedNextState == BOSS_SPIKE_STATE) {
        // Spike sequence
        spawnSpikes(hazards);
        bossSetState(boss, BOSS_SPIKE_STATE);
      } else if (boss->intendedNextState == BOSS_CAST_STATE) {
        // Cast FireBat
        bossSetState(boss, BOSS_CAST_STATE);
      } else {
        // Default back to idle
        bossSetState(boss, BOSS_IDLE_STATE);
      }
    }
    break;
  }

  // ─── SPIKE ───────────────────────────────────────────────
  case BOSS_SPIKE_STATE: {
    bossAnimTick(&boss->frame, &boss->animTimer, BOSS_IDLE_FRAMES,
                 BOSS_ANIM_SPEED);
    // Wait for all spikes to finish animating
    if (!anyHazardActive(hazards)) {
      // Short idle after spikes
      if (boss->stateTimer >= BOSS_POST_SPIKE_IDLE) {
        bossSetState(boss, BOSS_IDLE_STATE);
      }
    }
    break;
  }

  // ─── TRAP (one time, guaranteed hit) ─────────────────────
  case BOSS_TRAP_STATE: {
    // Trap is already placed on player. Boss walks towards player.
    bossAnimTick(&boss->frame, &boss->animTimer, BOSS_WALK_FRAMES,
                 BOSS_ANIM_SPEED);

    if (player->x < boss->x)
      boss->x -= BOSS_WALK_SPEED;
    else
      boss->x += BOSS_WALK_SPEED;

    int dist = player->x - boss->x;
    if (dist < 0)
      dist = -dist;
    // Slash when in range
    if (dist <= BOSS_SLASH_RANGE) {
      bossSetState(boss, BOSS_SLASH_STATE);
      // After slash, teleport out again (handled in SLASH's done → IDLE → picks
      // next) We set a flag so after slash we teleport back to spawn and idle
    }
    break;
  }

  // ─── DEATH RISE ──────────────────────────────────────────
  case BOSS_DEATH_RISE_STATE: {
    boss->y += BOSS_DEATH_RISE_SPEED;
    boss->riseY += BOSS_DEATH_RISE_SPEED;
    bossAnimTick(&boss->frame, &boss->animTimer, BOSS_IDLE_FRAMES,
                 BOSS_ANIM_SPEED); // just idle while rising
    if (boss->riseY >= BOSS_DEATH_RISE_HEIGHT) {
      bossSetState(boss, BOSS_DEATH_ANIM_STATE);
    }
    break;
  }

  // ─── DEATH ANIMATION ─────────────────────────────────────
  case BOSS_DEATH_ANIM_STATE: {
    int done = bossAnimTickOnce(&boss->frame, &boss->animTimer,
                                BOSS_DEATH_FRAMES, BOSS_ANIM_SPEED);
    if (done) {
      // Spawn 2 bats, deactivate boss
      spawnBats(minions, boss->x, boss->y, player->x);
      boss->active = 0;
    }
    break;
  }

  default:
    break;
  }
}

// ============================================================
// PLAYER ATTACKS BOSS
// ============================================================

void handlePlayerAttackBoss(struct Boss *boss, struct Player *player,
                            int isSlashwave) {
  if (!boss->active)
    return;
  if (boss->state == BOSS_DEATH_RISE_STATE ||
      boss->state == BOSS_DEATH_ANIM_STATE)
    return;
  if (boss->invincibilityTimer > 0)
    return;

  // Simple distance-based hit detection — GUARANTEED to work regardless of
  // coordinates
  int dx = boss->x - player->x;
  int dy = boss->y - player->y;
  if (dx < 0)
    dx = -dx;
  if (dy < 0)
    dy = -dy;

  int hitRange = isSlashwave ? 300 : 140;

  // For normal slash, only hit if boss is roughly in front of player
  if (!isSlashwave) {
    int bossInFront = player->facingRight ? (boss->x >= player->x - 64)
                                          : (boss->x <= player->x + 192);
    if (!bossInFront)
      return;
  }

  if (dx > hitRange || dy > 200)
    return;

  int damage = isSlashwave ? BOSS_SLASHWAVE_HIT_DAMAGE : BOSS_NORMAL_HIT_DAMAGE;
  boss->currentHealth -= damage;
  boss->invincibilityTimer = BOSS_INVINCIBILITY_FRAMES;

  if (boss->currentHealth <= 0) {
    boss->currentHealth = 0;
    boss->riseY = 0;
    player->isTrapped = 0;
    bossSetState(boss, BOSS_DEATH_RISE_STATE);
  }
}

// ============================================================
// UPDATE MINIONS (FireBats + Bats)
// ============================================================

void updateMinions(struct BossMinion minions[], struct Player *player) {
  for (int i = 0; i < MAX_BOSS_MINIONS; i++) {
    struct BossMinion *m = &minions[i];
    if (!m->active)
      continue;

    // --- dying: play death anim then deactivate ---
    if (m->isDying) {
      m->deathTimer++;
      m->animTimer++;
      if (m->animTimer >= BAT_ANIM_SPEED) {
        m->animTimer = 0;
        m->frame++;
      }
      if (m->frame >= BAT_DEATH_FRAMES) {
        m->active = 0;
      }
      continue;
    }

    // Cool down invincibility
    if (m->invincibilityTimer > 0)
      m->invincibilityTimer--;

    // Move towards player diagonally (like the bug enemy)
    int speed = (m->type == MINION_FIREBAT) ? FIREBAT_SPEED : BAT_SPEED;
    m->facingRight = (player->x > m->x) ? 1 : 0;

    // Horizontal movement
    if (player->x < m->x)
      m->x -= speed;
    else if (player->x > m->x)
      m->x += speed;

    // Vertical movement (fly toward player)
    int dy = player->y - m->y;
    if (dy > 5)
      m->y += speed / 2 + 1;
    else if (dy < -5)
      m->y -= speed / 2 + 1;

    // Animate
    int animSpeed =
        (m->type == MINION_FIREBAT) ? FIREBAT_ANIM_SPEED : BAT_ANIM_SPEED;
    int frames = (m->type == MINION_FIREBAT) ? FIREBAT_FRAMES : BAT_FRAMES;
    m->animTimer++;
    if (m->animTimer >= animSpeed) {
      m->animTimer = 0;
      m->frame = (m->frame + 1) % frames;
    }

    // Damage player on overlap
    int mw = (m->type == MINION_FIREBAT) ? FIREBAT_SIZE : BAT_SIZE;
    int dmg = (m->type == MINION_FIREBAT) ? FIREBAT_DAMAGE : BAT_DAMAGE;
    if (m->invincibilityTimer == 0 &&
        bossRectsOverlap(m->x, m->y, mw, mw, player->x, player->y,
                         (int)(SPRITE_SIZE * SCALE),
                         (int)(SPRITE_SIZE * SCALE))) {
      if (player->invincibilityTimer == 0) {
        player->health -= dmg;
        if (player->health < 0)
          player->health = 0;
        player->invincibilityTimer = 60;
      }
    }
  }
}

// Player X-attack hits a minion
void handlePlayerAttackMinion(struct BossMinion minions[],
                              struct Player *player, int isSlashwave) {
  for (int i = 0; i < MAX_BOSS_MINIONS; i++) {
    struct BossMinion *m = &minions[i];
    if (!m->active || m->isDying)
      continue;
    if (m->invincibilityTimer > 0)
      continue;

    int mw = (m->type == MINION_FIREBAT) ? FIREBAT_SIZE : BAT_SIZE;
    int playerAttackX = player->facingRight
                            ? player->x + (int)(SPRITE_SIZE * SCALE)
                            : player->x - 60;

    int attackW = isSlashwave ? 150 : 80;
    if (!player->facingRight && isSlashwave) {
      playerAttackX = player->x - attackW;
    }

    if (bossRectsOverlap(playerAttackX, player->y, attackW,
                         (int)(SPRITE_SIZE * SCALE), m->x, m->y, mw, mw)) {
      m->health -= 1;
      int invincibility = (m->type == MINION_FIREBAT) ? FIREBAT_INVINCIBILITY
                                                      : BAT_INVINCIBILITY;
      m->invincibilityTimer = invincibility;
      if (m->health <= 0) {
        m->isDying = 1;
        m->frame = 0;
        m->animTimer = 0;
      }
    }
  }
}

// ============================================================
// UPDATE HAZARDS (Spikes + Traps)
// ============================================================

void updateHazards(struct BossHazard hazards[], struct Player *player) {
  for (int i = 0; i < MAX_BOSS_HAZARDS; i++) {
    struct BossHazard *h = &hazards[i];
    if (!h->active)
      continue;

    int totalFrames =
        (h->type == HAZARD_SPIKE) ? BOSS_SPIKE_FRAMES : BOSS_TRAPOUT_FRAMES;
    int speed = BOSS_ANIM_SPEED;

    h->animTimer++;
    if (h->animTimer >= speed) {
      h->animTimer = 0;
      h->frame++;
      // Spike loops? No — plays once then deactivates.
      if (h->type == HAZARD_SPIKE && h->frame >= totalFrames) {
        h->active = 0;
        continue;
      }
      // Trap: loops (keeps player trapped until boss arrives)
      if (h->type == HAZARD_TRAP && h->frame >= totalFrames) {
        h->frame = 0; // loop trap animation
      }
    }

    // Damage player on overlap (only once for spike; trap allows repeat at
    // intervals)
    if (!h->hasHit) {
      int hitW = (h->type == HAZARD_SPIKE) ? 64 : BOSS_SIZE_W;
      int hitX = (h->type == HAZARD_SPIKE) ? h->x + 32 : h->x;
      if (bossRectsOverlap(hitX, h->y, hitW, BOSS_SIZE_H, player->x, player->y,
                           (int)(SPRITE_SIZE * SCALE),
                           (int)(SPRITE_SIZE * SCALE))) {
        if (h->type == HAZARD_SPIKE) {
          if (player->invincibilityTimer == 0) {
            player->health -= BOSS_SPIKE_PLAYER_DAMAGE;
            if (player->health < 0)
              player->health = 0;
            player->invincibilityTimer = 60;
            h->hasHit = 1;
          }
        } else if (h->type == HAZARD_TRAP) {
          player->isTrapped = 1;
        }
      }
    }
  }
}

// Deactivate all traps (call after boss performs trap slash)
void clearAllTraps(struct BossHazard hazards[]) {
  for (int i = 0; i < MAX_BOSS_HAZARDS; i++) {
    if (hazards[i].active && hazards[i].type == HAZARD_TRAP)
      hazards[i].active = 0;
  }
}

// ============================================================
// RENDER BOSS
// ============================================================

void renderBoss(struct Boss *boss, struct Camera *cam) {
  if (!boss->active)
    return;

  unsigned int *frames = bossIdle;
  int frameCount = BOSS_IDLE_FRAMES;

  switch (boss->state) {
  case BOSS_IDLE_STATE:
  case BOSS_WAIT_MINION_STATE:
  case BOSS_SPIKE_STATE:
    frames = bossIdle;
    frameCount = BOSS_IDLE_FRAMES;
    break;
  case BOSS_WALK_STATE:
  case BOSS_TRAP_STATE:
    frames = boss->facingRight ? bossWalkR : bossWalkL;
    frameCount = BOSS_WALK_FRAMES;
    break;
  case BOSS_SLASH_STATE:
    frames = boss->facingRight ? bossSlashR : bossSlashL;
    frameCount = BOSS_SLASH_FRAMES;
    break;
  case BOSS_DASH_STATE:
    frames = boss->facingRight ? bossDashR : bossDashL;
    frameCount = BOSS_DASH_FRAMES;
    break;
  case BOSS_CAST_STATE:
    frames = boss->facingRight ? bossCastR : bossCastL;
    frameCount = BOSS_CAST_FRAMES;
    break;
  case BOSS_TELEPORT_OUT_STATE:
    frames = bossTeleportOut;
    frameCount = BOSS_TELEPORT_FRAMES;
    break;
  case BOSS_TELEPORT_IN_STATE:
    frames = bossTeleportIn;
    frameCount = BOSS_TELEPORT_FRAMES;
    break;
  case BOSS_DEATH_RISE_STATE:
    frames = bossIdle;
    frameCount = BOSS_IDLE_FRAMES;
    break;
  case BOSS_DEATH_ANIM_STATE:
    frames = bossDeath;
    frameCount = BOSS_DEATH_FRAMES;
    break;
  default:
    break;
  }

  int frame = boss->frame;
  if (frame < 0)
    frame = 0;
  if (frame >= frameCount)
    frame = frameCount - 1;

  int drawX =
      boss->x; // BOSS_STATE: camera is always at 0, use screen-space directly
  int drawY = boss->y;

  if (frames[frame] != 0)
    iShowImage(drawX, drawY, BOSS_SIZE_W, BOSS_SIZE_H, frames[frame]);

  // Health bar above boss
  int barW = BOSS_SIZE_W;
  int barH = 8;
  int barX = drawX;
  int barY = drawY + BOSS_SIZE_H + 6;
  int filled = (boss->currentHealth * barW) / boss->maxHealth;
  iSetColor(80, 0, 0);
  iFilledRectangle(barX, barY, barW, barH);
  iSetColor(220, 30, 30);
  iFilledRectangle(barX, barY, filled, barH);
  iSetColor(255, 255, 255);
  iRectangle(barX, barY, barW, barH);
}

// ============================================================
// RENDER MINIONS
// ============================================================

void renderMinions(struct BossMinion minions[], struct Camera *cam) {
  for (int i = 0; i < MAX_BOSS_MINIONS; i++) {
    struct BossMinion *m = &minions[i];
    if (!m->active)
      continue;

    unsigned int *frames;
    int frameCount;
    int sz;

    if (m->isDying) {
      frames = batDeath;
      frameCount = BAT_DEATH_FRAMES;
      sz = BAT_SIZE;
    } else if (m->type == MINION_FIREBAT) {
      frames = m->facingRight ? fireBatR : fireBatL;
      frameCount = FIREBAT_FRAMES;
      sz = FIREBAT_SIZE;
    } else {
      frames = m->facingRight ? batR : batL;
      frameCount = BAT_FRAMES;
      sz = BAT_SIZE;
    }

    int frame = m->frame;
    if (frame < 0)
      frame = 0;
    if (frame >= frameCount)
      frame = frameCount - 1;

    int drawX = m->x; // screen-space
    int drawY = m->y;

    if (frames[frame] != 0)
      iShowImage(drawX, drawY, sz, sz, frames[frame]);
  }
}

// ============================================================
// RENDER HAZARDS
// ============================================================

void renderHazards(struct BossHazard hazards[], struct Camera *cam) {
  for (int i = 0; i < MAX_BOSS_HAZARDS; i++) {
    struct BossHazard *h = &hazards[i];
    if (!h->active)
      continue;

    unsigned int *frames;
    int frameCount;

    if (h->type == HAZARD_SPIKE) {
      frames = bossSpike;
      frameCount = BOSS_SPIKE_FRAMES;
    } else {
      frames = bossTrapIn;
      frameCount = BOSS_TRAPIN_FRAMES;
    }

    int frame = h->frame;
    if (frame < 0)
      frame = 0;
    if (frame >= frameCount)
      frame = frameCount - 1;

    int drawX = h->x; // screen-space
    int drawY = h->y;

    int drawW = (h->type == HAZARD_SPIKE) ? 64 : BOSS_SIZE_W;
    drawX += (h->type == HAZARD_SPIKE) ? 32 : 0;

    if (frames[frame] != 0)
      iShowImage(drawX, drawY, drawW, BOSS_SIZE_H, frames[frame]);
  }
}

#endif // BOSS_HPP
