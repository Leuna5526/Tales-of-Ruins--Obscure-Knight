#ifndef GRIMMASTER_HPP
#define GRIMMASTER_HPP

#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "inventory.hpp"
#include "player.hpp"
#include "sounds.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <math.h>
#include <stdlib.h>

inline int checkCollisionGrim(int x1, int y1, int w1, int h1, int x2, int y2,
                              int w2, int h2) {
  return !(x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1);
}

void initGrims(struct GrimMaster grims[]) {
  for (int i = 0; i < MAX_GRIMS; i++) {
    grims[i].active = 0;
    grims[i].state = GRIM_INACTIVE;
    grims[i].frame = 0;
    grims[i].animTimer = 0;
    grims[i].stateTimer = 0;
    grims[i].subStateTimer = 0;
    grims[i].facingRight = 0;
    grims[i].currentHealth = GRIM_MAX_HEALTH;
    grims[i].maxHealth = GRIM_MAX_HEALTH;
    grims[i].invincibilityTimer = 0;
    grims[i].damageAnimTimer = 0;
    grims[i].damageFrame = 0;
    grims[i].teleportTargetX = 0;
    grims[i].spawnX = 0;
    grims[i].spawnY = 0;
  }
}

void initGrimFireballs(struct GrimFireball fireballs[]) {
  for (int i = 0; i < MAX_GRIM_FIREBALLS; i++) {
    fireballs[i].active = 0;
    fireballs[i].exploding = 0;
  }
}

void spawnGrim(struct GrimMaster *grim, int x, int y) {
  grim->x = x;
  grim->y = y;
  grim->frame = 0;
  grim->animTimer = 0;
  grim->active = 1;
  grim->state = GRIM_IDLE_STATE;
  grim->stateTimer = 0;
  grim->subStateTimer = 0;
  grim->facingRight = 0;
  grim->currentHealth = GRIM_MAX_HEALTH;
  grim->maxHealth = GRIM_MAX_HEALTH;
  grim->invincibilityTimer = 0;
  grim->damageAnimTimer = 0;
  grim->damageFrame = 0;
  grim->teleportTargetX = 0;
  grim->spawnX = x;
  grim->spawnY = y;
}

void spawnGrimFireball(struct GrimFireball fireballs[], float startX, float startY,
                       int targetX, int targetY) {
  for (int i = 0; i < MAX_GRIM_FIREBALLS; i++) {
    if (!fireballs[i].active) {
      fireballs[i].x = startX;
      fireballs[i].y = startY;
      fireballs[i].targetX = targetX;
      fireballs[i].targetY = targetY;
      float dx = (float)(targetX - startX);
      float dy = (float)(targetY - startY);
      float dist = sqrt(dx * dx + dy * dy);
      if (dist < 1.0f)
        dist = 1.0f;
      fireballs[i].vx = (dx / dist) * GRIM_FIREBALL_SPEED;
      fireballs[i].vy = (dy / dist) * GRIM_FIREBALL_SPEED;
      fireballs[i].active = 1;
      fireballs[i].frame = 0;
      fireballs[i].animTimer = 0;
      fireballs[i].exploding = 0;
      fireballs[i].explodeFrame = 0;
      fireballs[i].explodeTimer = 0;
      break;
    }
  }
}

void updateGrims(struct GrimMaster grims[], struct GrimFireball fireballs[],
                 struct Player *player, struct Pickup pickups[]) {
  int spriteW = (int)(SPRITE_SIZE * SCALE);

  for (int i = 0; i < MAX_GRIMS; i++) {
    if (!grims[i].active && grims[i].state == GRIM_INACTIVE) {
      int spawnX = GRIM_SPAWN_1_X;
      if (i == 1)
        spawnX = GRIM_SPAWN_2_X;

      int dx = player->x - spawnX;
      int dy = player->y - (LEVEL3_GROUND_Y + GRIM_SPAWN_Y_OFFSET);
      float dist = sqrt((double)(dx * dx + dy * dy));

      if (dist <= GRIM_DETECTION_RANGE) {
        spawnGrim(&grims[i], spawnX, LEVEL3_GROUND_Y + GRIM_SPAWN_Y_OFFSET);
      }
      continue;
    }

    if (!grims[i].active)
      continue;

    struct GrimMaster *g = &grims[i];

    int sdx = player->x - g->spawnX;
    int sdy = player->y - g->spawnY;
    float distFromSpawn = sqrt((double)(sdx * sdx + sdy * sdy));

    if (distFromSpawn > 600) {
      if (g->state != GRIM_RETURNING) {
        g->state = GRIM_RETURNING;
        g->frame = 0;
        g->stateTimer = 0;
        g->subStateTimer = 0;
        g->facingRight = (g->spawnX > g->x) ? 1 : 0;
      }
    }

    int dx = player->x - g->x;
    int dy = player->y - g->y;
    float distance = sqrt((double)(dx * dx + dy * dy));

    g->stateTimer++;
    g->subStateTimer++;
    g->animTimer++;

    if (g->animTimer >= GRIM_ANIM_SPEED) {
      g->animTimer = 0;

      switch (g->state) {
      case GRIM_IDLE_STATE:
        g->frame = (g->frame + 1) % GRIM_IDLE_FRAMES;
        break;
      case GRIM_TURN_STATE:
        g->frame++;
        if (g->facingRight) {
          if (g->frame >= GRIM_TURN_R_FRAMES)
            g->frame = GRIM_TURN_R_FRAMES - 1;
        } else {
          if (g->frame >= GRIM_TURN_L_FRAMES)
            g->frame = GRIM_TURN_L_FRAMES - 1;
        }
        break;
      case GRIM_DASH_ANTICIPATE:
        if (g->facingRight)
          g->frame = (g->frame + 1) % GRIM_DASH_ANTIC_R_FRAMES;
        else
          g->frame = (g->frame + 1) % GRIM_DASH_ANTIC_L_FRAMES;
        break;
      case GRIM_DASHING:
        if (g->facingRight)
          g->frame = (g->frame + 1) % GRIM_DASH_R_FRAMES;
        else
          g->frame = (g->frame + 1) % GRIM_DASH_L_FRAMES;
        break;
      case GRIM_THROW_ANTICIPATE:
        if (g->facingRight)
          g->frame = (g->frame + 1) % GRIM_THROW_ANTIC_R_FRAMES;
        else
          g->frame = (g->frame + 1) % GRIM_THROW_ANTIC_L_FRAMES;
        break;
      case GRIM_THROWING:
        if (g->facingRight) {
          g->frame++;
          if (g->frame >= GRIM_THROW_R_FRAMES)
            g->frame = GRIM_THROW_R_FRAMES - 1;
        } else {
          g->frame++;
          if (g->frame >= GRIM_THROW_L_FRAMES)
            g->frame = GRIM_THROW_L_FRAMES - 1;
        }
        break;
      case GRIM_TELEPORT_OUT_STATE:
        g->frame++;
        if (g->frame >= GRIM_TELEPORT_OUT_FRAMES)
          g->frame = GRIM_TELEPORT_OUT_FRAMES - 1;
        break;
      case GRIM_TELEPORT_OUT_PILLAR:
        g->frame++;
        if (g->frame >= GRIM_TELEPORT_OUT_PILLAR_FRAMES)
          g->frame = GRIM_TELEPORT_OUT_PILLAR_FRAMES - 1;
        break;
      case GRIM_TELEPORT_IN_PILLAR:
        g->frame++;
        if (g->frame >= GRIM_TELEPORT_IN_PILLAR_FRAMES)
          g->frame = GRIM_TELEPORT_IN_PILLAR_FRAMES - 1;
        break;
      case GRIM_TELEPORT_IN_STATE:
        g->frame++;
        if (g->frame >= GRIM_TELEPORT_IN_FRAMES)
          g->frame = GRIM_TELEPORT_IN_FRAMES - 1;
        break;
      case GRIM_DYING:
        g->frame++;
        if (g->frame >= GRIM_DEATH_FRAMES)
          g->frame = GRIM_DEATH_FRAMES - 1;
        break;
      case GRIM_RETURNING:
        if (g->facingRight)
          g->frame = (g->frame + 1) % GRIM_DASH_R_FRAMES;
        else
          g->frame = (g->frame + 1) % GRIM_DASH_L_FRAMES;
        break;
      default:
        break;
      }
    }

    switch (g->state) {
    case GRIM_IDLE_STATE: {
      int shouldFaceRight = (dx > 0) ? 1 : 0;
      if (shouldFaceRight != g->facingRight) {
        g->state = GRIM_TURN_STATE;
        g->frame = 0;
        g->stateTimer = 0;
        g->subStateTimer = 0;
      } else if (distance <= 500) {
        int choice = rand() % 100;
        if (choice < 25) {
          g->state = GRIM_TELEPORT_OUT_STATE;
          g->frame = 0;
          g->stateTimer = 0;
          g->subStateTimer = 0;
          if (g->x < player->x) {
            g->teleportTargetX = player->x + GRIM_TELEPORT_RADIUS - (rand() % 80);
          } else {
            g->teleportTargetX = player->x - GRIM_TELEPORT_RADIUS + (rand() % 80);
          }
          if (g->teleportTargetX < 50)
            g->teleportTargetX = 50;
          if (g->teleportTargetX > TOTAL_BG_WIDTH - 100)
            g->teleportTargetX = TOTAL_BG_WIDTH - 100;
        } else if (choice < 50 && distance > 160) {
          g->state = GRIM_DASH_ANTICIPATE;
          g->frame = 0;
          g->stateTimer = 0;
          g->subStateTimer = 0;
        } else {
          g->state = GRIM_THROW_ANTICIPATE;
          g->frame = 0;
          g->stateTimer = 0;
          g->subStateTimer = 0;
        }
      } else if (g->stateTimer > 10) {
        int choice = rand() % 100;
        if (choice < 30 && distance <= GRIM_ATTACK_RANGE) {
          g->state = GRIM_DASH_ANTICIPATE;
          g->frame = 0;
          g->stateTimer = 0;
          g->subStateTimer = 0;
        } else if (choice < 65) {
          g->state = GRIM_THROW_ANTICIPATE;
          g->frame = 0;
          g->stateTimer = 0;
          g->subStateTimer = 0;
        } else if (choice < 85) {
          g->state = GRIM_TELEPORT_OUT_STATE;
          g->frame = 0;
          g->stateTimer = 0;
          g->subStateTimer = 0;
          if (g->x < player->x) {
            g->teleportTargetX = player->x + GRIM_TELEPORT_RADIUS - (rand() % 80);
          } else {
            g->teleportTargetX = player->x - GRIM_TELEPORT_RADIUS + (rand() % 80);
          }
          if (g->teleportTargetX < 50)
            g->teleportTargetX = 50;
          if (g->teleportTargetX > TOTAL_BG_WIDTH - 100)
            g->teleportTargetX = TOTAL_BG_WIDTH - 100;
        } else {
          g->stateTimer = 30;
        }
      }
      break;
    }

    case GRIM_TURN_STATE: {
      int turnFrames = g->facingRight ? GRIM_TURN_R_FRAMES : GRIM_TURN_L_FRAMES;
      if (g->subStateTimer >= turnFrames * GRIM_ANIM_SPEED) {
        g->facingRight = !g->facingRight;
        g->state = GRIM_IDLE_STATE;
        g->frame = 0;
        g->stateTimer = 0;
        g->subStateTimer = 0;
      }
      break;
    }

    case GRIM_DASH_ANTICIPATE: {
      int anticFrames = g->facingRight ? GRIM_DASH_ANTIC_R_FRAMES
                                       : GRIM_DASH_ANTIC_L_FRAMES;
      if (g->subStateTimer >= anticFrames * 2) { 
        g->state = GRIM_DASHING;
        g->frame = 0;
        g->stateTimer = 0;
        g->subStateTimer = 0;
      }
      break;
    }

    case GRIM_DASHING:
      if (distance > 150) {
          if (player->x > g->x) { g->x += GRIM_DASH_SPEED; g->facingRight = 1; }
          else { g->x -= GRIM_DASH_SPEED; g->facingRight = 0; }
      } else {
          if (player->x > g->x) { g->x -= GRIM_DASH_SPEED; g->facingRight = 0; }
          else { g->x += GRIM_DASH_SPEED; g->facingRight = 1; }
      }

      if (abs(distance - 150) < 20 || g->subStateTimer > 40) {
        g->state = GRIM_IDLE_STATE;
        g->frame = 0;
        g->stateTimer = 0;
        g->subStateTimer = 0;
      }
      break;

    case GRIM_THROW_ANTICIPATE: {
      int anticFrames = g->facingRight ? GRIM_THROW_ANTIC_R_FRAMES
                                       : GRIM_THROW_ANTIC_L_FRAMES;
      if (g->subStateTimer >= anticFrames * (GRIM_ANIM_SPEED/2)) { 
        g->state = GRIM_THROWING;
        g->frame = 0;
        g->stateTimer = 0;
        g->subStateTimer = 0;
        float fbX = g->facingRight
                  ? (float)(g->x + GRIM_FB_SPAWN_OFFSET_X_RIGHT)
                  : (float)(g->x + GRIM_SIZE - GRIM_FB_SPAWN_OFFSET_X_LEFT - GRIM_FIREBALL_SIZE);
        float fbY = (float)(g->y + GRIM_FB_SPAWN_OFFSET_Y);
        int spriteSize = (int)(SPRITE_SIZE * SCALE);
        int targetFbX = player->x + spriteSize / 2 - 30;
        int targetFbY = player->y - GRIM_FB_TARGET_GROUND_OFFSET;
        spawnGrimFireball(fireballs, fbX, fbY, targetFbX, targetFbY);
      }
      break;
    }

    case GRIM_THROWING: {
      int throwFrames =
          g->facingRight ? GRIM_THROW_R_FRAMES : GRIM_THROW_L_FRAMES;
      if (g->subStateTimer >= throwFrames * 2) { 
        g->state = GRIM_IDLE_STATE;
        g->frame = 0;
        g->stateTimer = 0;
        g->subStateTimer = 0;
      }
      break;
    }

    case GRIM_TELEPORT_OUT_STATE:
      if (g->subStateTimer >= GRIM_TELEPORT_OUT_FRAMES * 2) { 
        g->state = GRIM_TELEPORT_OUT_PILLAR;
        g->frame = 0;
        g->subStateTimer = 0;
      }
      break;

    case GRIM_TELEPORT_OUT_PILLAR:
      if (g->subStateTimer >=
          GRIM_TELEPORT_OUT_PILLAR_FRAMES * 2) { 
        g->x = g->teleportTargetX;
        g->state = GRIM_TELEPORT_IN_PILLAR;
        g->frame = 0;
        g->subStateTimer = 0;
        g->facingRight = (player->x > g->x) ? 1 : 0;
      }
      break;

    case GRIM_TELEPORT_IN_PILLAR:
      if (g->subStateTimer >=
          GRIM_TELEPORT_IN_PILLAR_FRAMES * 2) { 
        g->state = GRIM_TELEPORT_IN_STATE;
        g->frame = 0;
        g->subStateTimer = 0;
      }
      break;

    case GRIM_TELEPORT_IN_STATE:
      if (g->subStateTimer >= GRIM_TELEPORT_IN_FRAMES * 2) { 
        int followUp = rand() % 100;
        if (followUp < 60) {
          g->state = GRIM_THROW_ANTICIPATE;
        } else {
          g->state = GRIM_DASH_ANTICIPATE;
        }
        g->frame = 0;
        g->stateTimer = 0;
        g->subStateTimer = 0;
      }
      break;

    case GRIM_DYING:
      if (g->subStateTimer >= GRIM_DEATH_FRAMES * 20) {
        g->state = GRIM_DEAD;
        g->active = 0;
      }
      break;

    case GRIM_RETURNING:
      if (g->spawnX > g->x) {
        g->x += GRIM_DASH_SPEED;
        g->facingRight = 1;
      } else if (g->spawnX < g->x) {
        g->x -= GRIM_DASH_SPEED;
        g->facingRight = 0;
      }
      if (abs(g->x - g->spawnX) <= GRIM_DASH_SPEED) {
        g->x = g->spawnX;
        g->y = g->spawnY;
        g->state = GRIM_IDLE_STATE;
        g->frame = 0;
        g->stateTimer = 0;
        g->subStateTimer = 0;
      }
      break;

    default:
      break;
    }

    if (g->state != GRIM_DYING && g->state != GRIM_DEAD &&
        g->state != GRIM_INACTIVE && g->state != GRIM_TELEPORT_OUT_STATE &&
        g->state != GRIM_TELEPORT_OUT_PILLAR && player->state != DEATH &&
        player->invincibilityTimer == 0) {
      if (checkCollisionGrim(player->x + 24, player->y + 24, 80, 80, g->x,
                             g->y, GRIM_SIZE, GRIM_SIZE)) {
        int damagePercent = GRIM_CONTACT_DAMAGE;
        if (g->state == GRIM_DASHING) {
          damagePercent = GRIM_FIREBALL_DAMAGE;
        }
        int damage = PLAYER_MAX_HEALTH * damagePercent / 100;
        player->health -= damage;
        if (player->health < 0)
          player->health = 0;
        player->invincibilityTimer = 60;

        if (player->health <= 0) {
          player->state = DEATH;
          player->frame = 0;
          player->stateTimer = 0;
          playDeathSound();
        }
      }
    }

    int isPlayerAttacking =
        (player->state == ATTACK_OVERHEAD_RECOVER ||
         player->state == ATTACK_OVERHEAD_SLASHING ||
         player->state == ATTACK_OVERHEAD_SLASHWAVE ||
         player->state == DOWNSTAB_PRE || player->state == DOWNSTAB_ACTIVE ||
         player->state == DOWNSTAB_LAND);

    if (isPlayerAttacking && g->state != GRIM_DYING &&
        g->state != GRIM_DEAD) {
      int attackHitboxX = player->x;
      int attackHitboxW = ATTACK_RANGE;
      if (player->facingRight) {
        attackHitboxX = player->x + spriteW / 2;
      } else {
        attackHitboxW = ATTACK_RANGE_LEFT;
        attackHitboxX = player->x - ATTACK_RANGE_LEFT;
      }

      if (checkCollisionGrim(attackHitboxX, player->y, attackHitboxW,
                              ATTACK_RANGE, g->x, g->y, GRIM_SIZE,
                              GRIM_SIZE)) {
        if (g->invincibilityTimer == 0) {
          int damagePercent = (player->state == DOWNSTAB_ACTIVE ||
                               player->state == DOWNSTAB_LAND)
                                  ? DAMAGE_DEALT_HEAVY
                                  : DAMAGE_DEALT_NORMAL;
          int damage =
              (g->maxHealth * damagePercent / 100) * player->damageMultiplier;
          g->currentHealth -= damage;
          g->invincibilityTimer = GRIM_INVINCIBILITY_FRAMES;

          if (g->currentHealth <= 0) {
            g->currentHealth = 0;
            g->state = GRIM_DYING;
            g->subStateTimer = 0;
            g->animTimer = 0;
            g->frame = 0;
            tryDropItem(pickups, g->x, g->y);
            playEnemyKillSound();
            player->fragments++;
          } else {
            g->damageAnimTimer = GRIM_DAMAGE_FRAMES * 4;
            g->damageFrame = 0;
          }
        }
      }
    }

    if (g->invincibilityTimer > 0)
      g->invincibilityTimer--;

    if (g->damageAnimTimer > 0) {
      g->damageAnimTimer--;
      if (g->damageAnimTimer % 4 == 0 &&
          g->damageFrame < GRIM_DAMAGE_FRAMES - 1) {
        g->damageFrame++;
      }
    }
  }
}

void updateGrimFireballs(struct GrimFireball fireballs[],
                         struct Player *player) {

  for (int i = 0; i < MAX_GRIM_FIREBALLS; i++) {
    if (!fireballs[i].active)
      continue;

    if (fireballs[i].exploding) {
      fireballs[i].explodeTimer++;
      if (fireballs[i].explodeTimer >= 4) {
        fireballs[i].explodeTimer = 0;
        fireballs[i].explodeFrame++;
        if (fireballs[i].explodeFrame >= GRIM_FIREBALL_EXPLODE_FRAMES) {
          fireballs[i].active = 0;
          continue;
        }
      }
      continue;
    }

    fireballs[i].x += fireballs[i].vx;
    fireballs[i].y += fireballs[i].vy;

    fireballs[i].animTimer++;
    if (fireballs[i].animTimer >= 4) {
      fireballs[i].animTimer = 0;
      fireballs[i].frame = (fireballs[i].frame + 1) % GRIM_FIREBALL_FRAMES;
    }

    int fbPosX = (int)fireballs[i].x;
    int fbPosY = (int)fireballs[i].y;

    if (fbPosY <= LEVEL3_GROUND_Y) {
      fireballs[i].y = LEVEL3_GROUND_Y; 
      fireballs[i].exploding = 1;
      fireballs[i].explodeFrame = 0;
      fireballs[i].explodeTimer = 0;
      continue;
    }

    if (player->invincibilityTimer == 0 && player->state != DEATH) {
      int fbCenterX = fbPosX + GRIM_FIREBALL_DISPLAY_SIZE / 2;
      int fbCenterY = fbPosY + GRIM_FIREBALL_DISPLAY_SIZE / 2;
      int fbHitX = fbCenterX - GRIM_FIREBALL_HIT_W / 2;
      int fbHitY = fbCenterY - GRIM_FIREBALL_HIT_H / 2;

      int playerHitX = player->x + 24;
      int playerHitY = player->y + 10;
      int playerHitW = 80;
      int playerHitH = 100;

      if (checkCollisionGrim(playerHitX, playerHitY, playerHitW, playerHitH,
                             fbHitX, fbHitY,
                             GRIM_FIREBALL_HIT_W, GRIM_FIREBALL_HIT_H)) {
        fireballs[i].exploding = 1;
        fireballs[i].explodeFrame = 0;
        fireballs[i].explodeTimer = 0;

        int damage = PLAYER_MAX_HEALTH * GRIM_FIREBALL_DAMAGE / 100;
        player->health -= damage;
        if (player->health < 0)
          player->health = 0;
        player->invincibilityTimer = 60;

        if (player->health <= 0) {
          player->state = DEATH;
          player->frame = 0;
          player->stateTimer = 0;
          playDeathSound();
        }
        continue;
      }
    }

    if (fbPosX < -200 || fbPosX > TOTAL_BG_WIDTH + 200 ||
        fbPosY < -200 || fbPosY > 800) {
      fireballs[i].active = 0;
    }
  }
}

void renderGrims(struct GrimMaster grims[], struct Camera *camera) {
  for (int i = 0; i < MAX_GRIMS; i++) {
    if (!grims[i].active)
      continue;

    struct GrimMaster *g = &grims[i];
    unsigned int tex = 0;

    switch (g->state) {
    case GRIM_IDLE_STATE:
      tex = grimIdle[g->frame % GRIM_IDLE_FRAMES];
      break;
    case GRIM_TURN_STATE:
      if (g->facingRight)
        tex = grimTurnR[g->frame % GRIM_TURN_R_FRAMES];
      else
        tex = grimTurnL[g->frame % GRIM_TURN_L_FRAMES];
      break;
    case GRIM_DASH_ANTICIPATE:
      if (g->facingRight)
        tex = grimDashAnticR[g->frame % GRIM_DASH_ANTIC_R_FRAMES];
      else
        tex = grimDashAnticL[g->frame % GRIM_DASH_ANTIC_L_FRAMES];
      break;
    case GRIM_DASHING:
      if (g->facingRight)
        tex = grimDashR[g->frame % GRIM_DASH_R_FRAMES];
      else
        tex = grimDashL[g->frame % GRIM_DASH_L_FRAMES];
      break;
    case GRIM_THROW_ANTICIPATE:
      if (g->facingRight)
        tex = grimThrowAnticR[g->frame % GRIM_THROW_ANTIC_R_FRAMES];
      else
        tex = grimThrowAnticL[g->frame % GRIM_THROW_ANTIC_L_FRAMES];
      break;
    case GRIM_THROWING:
      if (g->facingRight)
        tex = grimThrowR[g->frame % GRIM_THROW_R_FRAMES];
      else
        tex = grimThrowL[g->frame % GRIM_THROW_L_FRAMES];
      break;
    case GRIM_TELEPORT_OUT_STATE:
      tex = grimTeleOut[g->frame % GRIM_TELEPORT_OUT_FRAMES];
      break;
    case GRIM_TELEPORT_OUT_PILLAR:
      tex = grimTeleOutPillar[g->frame % GRIM_TELEPORT_OUT_PILLAR_FRAMES];
      break;
    case GRIM_TELEPORT_IN_PILLAR:
      tex = grimTeleInPillar[g->frame % GRIM_TELEPORT_IN_PILLAR_FRAMES];
      break;
    case GRIM_TELEPORT_IN_STATE:
      tex = grimTeleIn[g->frame % GRIM_TELEPORT_IN_FRAMES];
      break;
    case GRIM_DYING:
      tex = grimDeath[g->frame % GRIM_DEATH_FRAMES];
      break;
    case GRIM_RETURNING:
      if (g->facingRight)
        tex = grimDashR[g->frame % GRIM_DASH_R_FRAMES];
      else
        tex = grimDashL[g->frame % GRIM_DASH_L_FRAMES];
      break;
    default:
      tex = grimIdle[0];
      break;
    }

    if (tex != 0) {
      float screenX = getScreenX((float)g->x, camera);
      float screenY = getScreenY((float)g->y, camera);
      iShowImage(screenX, screenY, GRIM_SIZE, GRIM_SIZE, tex);
    }

    if (g->state != GRIM_DYING && g->state != GRIM_DEAD &&
        g->state != GRIM_INACTIVE && g->maxHealth > 0) {
      float screenX = getScreenX((float)g->x, camera);
      float screenY = getScreenY((float)g->y, camera);
      int barW = 50;
      int barH = 5;
      int barX = (int)(screenX + (GRIM_SIZE - barW) / 2);
      int barY = (int)(screenY + GRIM_SIZE + 5);
      int currHealth = g->currentHealth;
      if (currHealth < 0)
        currHealth = 0;
      int filled = (currHealth * barW) / g->maxHealth;

      iSetColor(80, 0, 0);
      iFilledRectangle(barX, barY, barW, barH);
      iSetColor(180, 0, 180);
      iFilledRectangle(barX, barY, filled, barH);
      iSetColor(255, 255, 255);
      iRectangle(barX, barY, barW, barH);
    }
  }
}

void renderGrimFireballs(struct GrimFireball fireballs[],
                         struct Camera *camera) {
  for (int i = 0; i < MAX_GRIM_FIREBALLS; i++) {
    if (!fireballs[i].active)
      continue;

    float screenX = getScreenX((float)fireballs[i].x, camera);
    float screenY = getScreenY((float)fireballs[i].y, camera);

    if (fireballs[i].exploding) {
      unsigned int tex =
          grimFireballExplode[fireballs[i].explodeFrame %
                              GRIM_FIREBALL_EXPLODE_FRAMES];
      if (tex != 0) {
        iShowImage(screenX, screenY, GRIM_FIREBALL_EXPLODE_DISPLAY_SIZE, GRIM_FIREBALL_EXPLODE_DISPLAY_SIZE,
                   tex);
      }
    } else {
      unsigned int tex =
          grimFireball[fireballs[i].frame % GRIM_FIREBALL_FRAMES];
      if (tex != 0) {
        iShowImage(screenX, screenY, GRIM_FIREBALL_DISPLAY_SIZE, GRIM_FIREBALL_DISPLAY_SIZE,
                   tex);
      }
    }
  }
}

#endif
