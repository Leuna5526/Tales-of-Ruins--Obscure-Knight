#ifndef SENTRY_HPP
#define SENTRY_HPP

#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "inventory.hpp"
#include "player.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <math.h>

// ---------------- SENTRY FUNCTIONS ----------------mainly
inline int checkCollisionSentry(int x1, int y1, int w1, int h1, int x2, int y2,
                                int w2, int h2) {
  return !(x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1);
}

// ================================================================
// ============== SENTRY FUNCTIONS (Level 2 Enemy) ================
// ================================================================

void initSentries(struct Sentry sentries[]) {
  for (int i = 0; i < MAX_SENTRIES; i++) {
    sentries[i].active = 0;
    sentries[i].state = SENTRY_INACTIVE;
    sentries[i].frame = 0;
    sentries[i].vx = 0;
    sentries[i].vy = 0;
    sentries[i].subStateTimer = 0;
    sentries[i].animationTimer = 0;
    sentries[i].facingRight = 1;
  }
}

void spawnSentry(struct Sentry *sentry, int x, int y) {
  sentry->x = x;
  sentry->y = y;
  sentry->vx = 0;
  sentry->vy = 0;
  sentry->frame = 0;
  sentry->active = 1;
  sentry->state = SENTRY_WAKING;
  sentry->patrolStartX = x;
  sentry->subStateTimer = 0;
  sentry->subStateTimer = 0;
  sentry->animationTimer = 0;
  sentry->facingRight = 1;
  sentry->maxHealth = SENTRY_MAX_HEALTH;
  sentry->currentHealth = SENTRY_MAX_HEALTH;
  sentry->invincibilityTimer = 0;
  sentry->damageAnimTimer = 0;
  sentry->damageFrame = 0;
}

void updateSentries(struct Sentry sentries[], struct Player *player,
                    struct Pickup pickups[]) {
  int spriteW = SPRITE_SIZE * SCALE;

  // ===== 1. SPAWN SYSTEM =====
  for (int i = 0; i < MAX_SENTRIES; i++) {
    if (!sentries[i].active && sentries[i].state == SENTRY_INACTIVE) {
      int spawnX = SENTRY_SPAWN_1_X;
      if (i == 1)
        spawnX = SENTRY_SPAWN_2_X;
      else if (i == 2)
        spawnX = SENTRY_SPAWN_3_X;

      int dx = player->x - spawnX;
      int dy = player->y - GROUND_Y;
      float spawnDistance = sqrt((double)(dx * dx + dy * dy));

      if (spawnDistance <= CREATURE_SPAWN_TRIGGER) {
        spawnSentry(&sentries[i], spawnX, GROUND_Y);
      }
    }
  }

  // ===== 2. UPDATE ACTIVE SENTRIES =====
  for (int i = 0; i < MAX_SENTRIES; i++) {
    if (!sentries[i].active)
      continue;

    struct Sentry *s = &sentries[i];

    int dx = player->x - s->x;
    int dy = player->y - s->y;
    float distance = sqrt((double)(dx * dx + dy * dy));

    // Chase limit logic
    int distanceFromStart = abs(s->x - s->patrolStartX);
    int isPlayerInChaseRange = (distanceFromStart <= SENTRY_MAX_CHASE_DIST);

    // Only detect if within range AND within chase limit
    int isDetected =
        (distance <= SENTRY_DETECTION_RANGE) && isPlayerInChaseRange;

    // Gravity for Jump Attack (Fixed Physics Condition)
    if (s->y > LEVEL2_GROUND_Y || s->vy != 0) {
      s->vy -= GRAVITY;
      s->y += s->vy;

      // Ground collision
      if (s->y <= LEVEL2_GROUND_Y) {
        s->y = LEVEL2_GROUND_Y;
        s->vy = 0;

        // If landing from jump attack or air death
        if (s->state == SENTRY_JUMP_ATTACK) {
          s->state = s->facingRight ? SENTRY_SLASH_RIGHT : SENTRY_SLASH_LEFT;
          s->subStateTimer = 0;
          s->vx = 0;
        } else if (s->state == SENTRY_DYING_AIR) {
          s->state = SENTRY_DYING;
          s->subStateTimer = 0;
          s->frame = 0;
        }
      }
    }

    s->subStateTimer++;
    s->animationTimer++;

    // ===== 3. STATE MACHINE =====
    switch (s->state) {
    case SENTRY_WAKING:
      if (s->subStateTimer >= SENTRY_WAKE_FRAMES * 15) {
        s->state = SENTRY_IDLE_STATE;
        s->subStateTimer = 0;
      }
      break;

    case SENTRY_IDLE_STATE:
      if (s->subStateTimer >= SENTRY_IDLE_FRAMES * 20) {
        s->state = s->facingRight ? SENTRY_WALK_RIGHT : SENTRY_WALK_LEFT;
        s->vx = s->facingRight ? SENTRY_SPEED : -SENTRY_SPEED;
        s->subStateTimer = 0;
      }
      // Detection transition
      if (isDetected) {
        s->state = s->facingRight ? SENTRY_RUN_RIGHT : SENTRY_RUN_LEFT;
        s->vx = s->facingRight ? SENTRY_RUN_SPEED : -SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_WALK_RIGHT:
      s->x += s->vx;
      if (s->x >= s->patrolStartX + SENTRY_PATROL_DISTANCE) {
        s->state = SENTRY_TURNING_STATE;
        s->subStateTimer = 0;
        s->vx = 0;
      }
      // Random idle
      if (rand() % SENTRY_IDLE_CHANCE == 0) {
        s->state = SENTRY_IDLE_STATE;
        s->subStateTimer = 0;
        s->vx = 0;
      }
      // Detection
      if (isDetected) {
        s->state = SENTRY_RUN_RIGHT;
        s->vx = SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_WALK_LEFT:
      s->x += s->vx;
      if (s->x <= s->patrolStartX - SENTRY_PATROL_DISTANCE) {
        s->state = SENTRY_TURNING_STATE;
        s->subStateTimer = 0;
        s->vx = 0;
      }
      if (rand() % SENTRY_IDLE_CHANCE == 0) {
        s->state = SENTRY_IDLE_STATE;
        s->subStateTimer = 0;
        s->vx = 0;
      }
      if (isDetected) {
        s->state = SENTRY_RUN_LEFT;
        s->vx = -SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_RUN_RIGHT:
      s->x += s->vx;
      if (!isDetected) { // Lost player
        // Return logic: Face towards patrol start
        if (s->x > s->patrolStartX) {
          s->state = SENTRY_WALK_LEFT;
          s->vx = -SENTRY_SPEED;
        } else {
          s->state = SENTRY_WALK_RIGHT;
          s->vx = SENTRY_SPEED;
        }
      }
      // Manual face update if player jumps over
      if (dx < -50) { // Player is significantly behind
        s->facingRight = 0;
        s->state = SENTRY_RUN_LEFT;
        s->vx = -SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_RUN_LEFT:
      s->x += s->vx;
      if (!isDetected) {
        // Return logic: Face towards patrol start
        if (s->x > s->patrolStartX) {
          s->state = SENTRY_WALK_LEFT;
          s->vx = -SENTRY_SPEED;
        } else {
          s->state = SENTRY_WALK_RIGHT;
          s->vx = SENTRY_SPEED;
        }
      }
      if (dx > 50) {
        s->facingRight = 1;
        s->state = SENTRY_RUN_RIGHT;
        s->vx = SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_TURNING_STATE:
      if (s->subStateTimer >= SENTRY_TURNL_FRAMES * 15) {
        s->facingRight = !s->facingRight;
        s->state = s->facingRight ? SENTRY_WALK_RIGHT : SENTRY_WALK_LEFT;
        s->vx = s->facingRight ? SENTRY_SPEED : -SENTRY_SPEED;
        s->subStateTimer = 0;
      }
      break;

    case SENTRY_ATTACK_LEFT:
    case SENTRY_ATTACK_RIGHT:
      if (dx > 0)
        s->vx = SENTRY_ATTACK_SPEED;
      else if (dx < 0)
        s->vx = -SENTRY_ATTACK_SPEED;
      else
        s->vx = 0;
      s->x += s->vx;

      if (s->subStateTimer >= SENTRY_ATTACKL_FRAMES * 6) {
        s->state = s->facingRight ? SENTRY_WALK_RIGHT : SENTRY_WALK_LEFT;
        s->vx = s->facingRight ? SENTRY_SPEED : -SENTRY_SPEED;
        s->subStateTimer = 0;
      }
      break;

    // Jump Attack Logic
    case SENTRY_JUMP_ATTACK:
      s->x += s->vx; // Move horizontally while in air
      // State transition happens on ground collision (above)
      break;

    case SENTRY_SLASH_LEFT:
    case SENTRY_SLASH_RIGHT:
      // Ground recovery after jump attack
      if (s->subStateTimer >= SENTRY_SLASHL_FRAMES * 8) {
        s->state = s->facingRight
                       ? SENTRY_IDLE_STATE
                       : SENTRY_IDLE_STATE; // Brief idle after big attack
        s->vx = 0;
        s->subStateTimer = 0;
      }
      break;

    case SENTRY_DYING:
      if (s->subStateTimer >= SENTRY_DEATH_FRAMES * 20) {
        s->active = 0;
      }
      break;

    case SENTRY_DYING_AIR:
      if (s->subStateTimer >= SENTRY_DEATHAIR_FRAMES * 20) {
        s->active = 0;
      }
      break;

    default:
      break;
    }
    // ===== 4. ENTER ATTACK MODE =====
    if ((s->state == SENTRY_WALK_LEFT || s->state == SENTRY_WALK_RIGHT ||
         s->state == SENTRY_IDLE_STATE || s->state == SENTRY_RUN_LEFT ||
         s->state == SENTRY_RUN_RIGHT) &&
        distance <= SENTRY_ATTACK_RANGE) {

      s->facingRight = (dx > 0) ? 1 : 0;

      // Randomly choose Jump Attack or Normal Attack
      if (rand() % 100 < SENTRY_ATTACK_JUMP_CHANCE) {
        s->state = SENTRY_JUMP_ATTACK;
        s->vy = SENTRY_JUMP_V; // Velocity applied here
        // Jump towards player
        s->vx = s->facingRight ? 4 : -4;
        s->subStateTimer = 0;
      } else {
        s->state = s->facingRight ? SENTRY_ATTACK_RIGHT : SENTRY_ATTACK_LEFT;
        s->subStateTimer = 0;
      }
    }

    // ===== 5. ANIMATION SYSTEM =====
    if (s->animationTimer >= 6) {
      s->animationTimer = 0;
      switch (s->state) {
      case SENTRY_WAKING:
        s->frame = (s->frame + 1) % SENTRY_WAKE_FRAMES;
        break;
      case SENTRY_IDLE_STATE:
        s->frame = (s->frame + 1) % SENTRY_IDLE_FRAMES;
        break;
      case SENTRY_WALK_LEFT:
        s->frame = (s->frame + 1) % SENTRY_WALKL_FRAMES;
        break;
      case SENTRY_WALK_RIGHT:
        s->frame = (s->frame + 1) % SENTRY_WALKR_FRAMES;
        break;
      case SENTRY_RUN_LEFT:
        s->frame = (s->frame + 1) % SENTRY_RUNL_FRAMES;
        break;
      case SENTRY_RUN_RIGHT:
        s->frame = (s->frame + 1) % SENTRY_RUNR_FRAMES;
        break;
      case SENTRY_ATTACK_LEFT:
        s->frame = (s->frame + 1) % SENTRY_ATTACKL_FRAMES;
        break;
      case SENTRY_ATTACK_RIGHT:
        s->frame = (s->frame + 1) % SENTRY_ATTACKR_FRAMES;
        break;
      // Jump Attack uses Slash frames but sustained in air
      case SENTRY_JUMP_ATTACK:
        s->frame = (s->frame + 1) % SENTRY_SLASHL_FRAMES;
        break;
      case SENTRY_SLASH_LEFT:
        s->frame = (s->frame + 1) % SENTRY_SLASHL_FRAMES;
        break;
      case SENTRY_SLASH_RIGHT:
        s->frame = (s->frame + 1) % SENTRY_SLASHR_FRAMES;
        break;
      case SENTRY_TURNING_STATE:
        s->frame = (s->frame + 1) % SENTRY_TURNL_FRAMES;
        break;
      case SENTRY_DYING:
        s->frame = (s->frame + 1) % SENTRY_DEATH_FRAMES;
        break;
      case SENTRY_DYING_AIR:
        s->frame = (s->frame + 1) % SENTRY_DEATHAIR_FRAMES;
        break;
      default:
        break;
      }
    }

    // ===== 6. COLLISION & COMBAT =====
    if (s->state != SENTRY_DYING && s->state != SENTRY_DYING_AIR &&
        s->state != SENTRY_WAKING && player->state != DEATH &&
        player->invincibilityTimer == 0) { // Player Invincibility
      int sY = s->y;
      int sH = SENTRY_SIZE;
      // Extend downward for Jump Attack to ensure hit (User visual feedback)
      if (s->state == SENTRY_JUMP_ATTACK) {
        sY -= 80; // Extend significantly downwards
        sH += 80;
      }

      if (checkCollisionSentry(player->x + 24, player->y + 24, 80, 80, s->x, sY,
                               SENTRY_SIZE, sH)) {

        // Reduce player health
        // Double check invincibility before applying damage
        // Check if invincibility is active
        if (player->invincibilityTimer == 0) {
          int damagePercent = 10;
          if (s->state == SENTRY_ATTACK_LEFT ||
              s->state == SENTRY_ATTACK_RIGHT ||
              s->state == SENTRY_SLASH_LEFT || s->state == SENTRY_SLASH_RIGHT ||
              s->state == SENTRY_JUMP_ATTACK) {
            damagePercent = DAMAGE_PLAYER_TAKES; // 20%
          }

          int damage = PLAYER_MAX_HEALTH * damagePercent / 100;
          player->health -= damage;
          if (player->health < 0)
            player->health = 0;

          // Set invincibility
          // Short (10) for bump (10%), Long (60) for attack (20%)
          // If damage is high (attack), give full mercy invincibility
          int invincibilityDuration =
              (damagePercent >= DAMAGE_PLAYER_TAKES) ? 60 : 10;
          player->invincibilityTimer = invincibilityDuration;
        }

        // Check for death
        if (player->health <= 0) {
          player->state = DEATH;
          player->frame = 0;
          player->stateTimer = 0;
        }
      }
    }

    // Enemy death from player attack
    int isPlayerAttacking_local =
        (player->state == ATTACK_OVERHEAD_RECOVER ||
         player->state == ATTACK_OVERHEAD_SLASHING ||
         player->state == ATTACK_OVERHEAD_SLASHWAVE ||
         player->state == DOWNSTAB_PRE || player->state == DOWNSTAB_ACTIVE ||
         player->state == DOWNSTAB_LAND);

    if (isPlayerAttacking_local && s->state != SENTRY_DYING &&
        s->state != SENTRY_DYING_AIR) {
      int attackHitboxX = player->x;
      int attackHitboxW = ATTACK_RANGE;
      if (player->facingRight) {
        attackHitboxX = player->x + spriteW / 2;
      } else {
        attackHitboxW = ATTACK_RANGE_LEFT; // Use smaller range for left
        attackHitboxX = player->x - ATTACK_RANGE_LEFT;
      }

      if (checkCollisionSentry(attackHitboxX, player->y, attackHitboxW,
                               ATTACK_RANGE, s->x, s->y, SENTRY_SIZE,
                               SENTRY_SIZE)) {

        if (s->invincibilityTimer == 0) {
          // Determine damage based on attack type
          int damagePercent = (player->state == DOWNSTAB_ACTIVE ||
                               player->state == DOWNSTAB_LAND)
                                  ? DAMAGE_DEALT_HEAVY
                                  : DAMAGE_DEALT_NORMAL;

          // Damage is percentage of *Enemy's* max health
          int damage =
              (s->maxHealth * damagePercent / 100) * player->damageMultiplier;
          s->currentHealth -= damage;

          // Set Invincibility (cooldown covers entire attack combo ~72 frames)
          // So each attack combo can only hit each enemy ONCE
          s->invincibilityTimer = 90;

          if (s->currentHealth <= 0) {
            s->currentHealth = 0;
            s->state = player->onGround ? SENTRY_DYING : SENTRY_DYING_AIR;
            s->subStateTimer = 0;
            s->animationTimer = 0;
            s->frame = 0;
            s->vx = 0;
            s->vy = 0;

            // Dropped item logic
            tryDropItem(pickups, s->x, s->y);
          } else {
            // Not dead - trigger damage overlay animation
            s->damageAnimTimer = SENTRY_DAMAGE_FRAMES * 4; // Fast overlay
            s->damageFrame = 0;
          }
        }
      }
    }

    // Update invincibility timer
    if (s->invincibilityTimer > 0) {
      s->invincibilityTimer--;
    }

    // Update damage overlay animation
    if (s->damageAnimTimer > 0) {
      s->damageAnimTimer--;
      // Advance frame every 4 ticks (fast)
      if (s->damageAnimTimer % 4 == 0 &&
          s->damageFrame < SENTRY_DAMAGE_FRAMES - 1) {
        s->damageFrame++;
      }
    }
  }
}

void renderSentries(struct Sentry sentries[], struct Camera *camera) {
  for (int i = 0; i < MAX_SENTRIES; i++) {
    if (!sentries[i].active)
      continue;

    struct Sentry *s = &sentries[i];
    unsigned int tex;

    switch (s->state) {
    case SENTRY_WAKING:
      tex = sentryWake[s->frame % SENTRY_WAKE_FRAMES];
      break;
    case SENTRY_IDLE_STATE:
      tex = sentryIdle[s->frame % SENTRY_IDLE_FRAMES];
      break;
    case SENTRY_WALK_LEFT:
      tex = sentryWalkL[s->frame % SENTRY_WALKL_FRAMES];
      break;
    case SENTRY_WALK_RIGHT:
      tex = sentryWalkR[s->frame % SENTRY_WALKR_FRAMES];
      break;
    case SENTRY_RUN_LEFT:
      tex = sentryRunL[s->frame % SENTRY_RUNL_FRAMES];
      break;
    case SENTRY_RUN_RIGHT:
      tex = sentryRunR[s->frame % SENTRY_RUNR_FRAMES];
      break;
    case SENTRY_ATTACK_LEFT:
      tex = sentryAttackL[s->frame % SENTRY_ATTACKL_FRAMES];
      break;
    case SENTRY_ATTACK_RIGHT:
      tex = sentryAttackR[s->frame % SENTRY_ATTACKR_FRAMES];
      break;
    case SENTRY_SLASH_LEFT:
      tex = sentrySlashL[s->frame % SENTRY_SLASHL_FRAMES];
      break;
    case SENTRY_SLASH_RIGHT:
      tex = sentrySlashR[s->frame % SENTRY_SLASHR_FRAMES];
      break;
    case SENTRY_TURNING_STATE:
      tex = s->facingRight ? sentryTurnR[s->frame % SENTRY_TURNR_FRAMES]
                           : sentryTurnL[s->frame % SENTRY_TURNL_FRAMES];
      break;
    case SENTRY_DYING:
      tex = sentryDeath[s->frame % SENTRY_DEATH_FRAMES];
      break;
    case SENTRY_DYING_AIR:
      tex = sentryDeathAir[s->frame % SENTRY_DEATHAIR_FRAMES];
      break;
    default:
      tex = sentryIdle[0];
      break;
    }

    float screenX = getScreenX(s->x, camera);
    float screenY = getScreenY(s->y, camera);
    iShowImage(screenX, screenY, SENTRY_SIZE, SENTRY_SIZE, tex);

    // Render damage overlay on top if active
    if (s->damageAnimTimer > 0) {
      unsigned int dmgTex = sentryDamage[s->damageFrame % SENTRY_DAMAGE_FRAMES];
      if (dmgTex != 0) {
        iShowImage(screenX, screenY, SENTRY_SIZE, SENTRY_SIZE, dmgTex);
      }
    }
  }
}

#endif // SENTRY_HPP
