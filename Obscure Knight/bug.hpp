#ifndef BUG_HPP
#define BUG_HPP

#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "inventory.hpp"
#include "player.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <math.h>

void initCreatures(struct Creature creatures[]) {
  for (int i = 0; i < MAX_CREATURES; i++) {
    creatures[i].active = 0;                
    creatures[i].state = CREATURE_INACTIVE; 
  }
}

void spawnCreature(struct Creature *creature, int x, int y) {
  creature->x = x;
  creature->y = y;
  creature->vx = 0; 
  creature->vy = 0; 
  creature->frame = 0;
  creature->active = 1; 
  creature->state = CREATURE_LOADING; 
  creature->patrolStartX = x;
  creature->subStateTimer = 0;
  creature->animationTimer = 0;
  creature->facingRight = 1; 
  creature->maxHealth = BUG_MAX_HEALTH;
  creature->currentHealth = BUG_MAX_HEALTH;
  creature->invincibilityTimer = 0;
  creature->damageAnimTimer = 0;
  creature->damageFrame = 0;
}

int checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2,int h2) {
  return !(x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1);
}

void updateCreatures(struct Creature creatures[], struct Player *player,struct Pickup pickups[]) {

  int spriteW = SPRITE_SIZE * SCALE;
  for (int i = 0; i < MAX_CREATURES; i++) {
    if (!creatures[i].active && creatures[i].state == CREATURE_INACTIVE) {
      int spawnX = SPAWN_POINT_1_X;
      if (i == 1)
        spawnX = SPAWN_POINT_2_X;
      else if (i == 2)
        spawnX = SPAWN_POINT_3_X;

      int dx = player->x - spawnX;
      int dy = player->y - SPAWN_POINT_Y;
      float spawnDistance = sqrt((double)(dx * dx + dy * dy));

      if (spawnDistance <= CREATURE_SPAWN_TRIGGER) {
        spawnCreature(&creatures[i], spawnX, SPAWN_POINT_Y);
      }
    }
  }

  for (int i = 0; i < MAX_CREATURES; i++) {
    if (!creatures[i].active)
      continue;

    struct Creature *enemy = &creatures[i];
    int dx = player->x - enemy->x;
    int dy = player->y - enemy->y;
    float distance = sqrt((double)(dx * dx + dy * dy));
    enemy->subStateTimer++;
    enemy->animationTimer++;

    switch (enemy->state) {
    case CREATURE_LOADING:
      if (enemy->subStateTimer >= CREATURE_NORMAL_FRAMES * 20) {
        enemy->state = CREATURE_RISING;
        enemy->subStateTimer = 0;
        enemy->vy = -CREATURE_SPEED; 
      }
      break;

    case CREATURE_RISING:
      enemy->y += enemy->vy;
      if (enemy->y <= 150) { 
        enemy->state = CREATURE_PATROL_RIGHT;
        enemy->subStateTimer = 0;
        enemy->vx = CREATURE_SPEED;
        enemy->vy = 0;
        enemy->facingRight = 1; 
      }
      break;

    case CREATURE_PATROL_RIGHT:
      enemy->x += enemy->vx;
      enemy->y = 150; 
      if (enemy->x >= enemy->patrolStartX + CREATURE_PATROL_DISTANCE) {
        enemy->state = CREATURE_TURNING;
        enemy->subStateTimer = 0;
      }
      break;

    case CREATURE_PATROL_LEFT:
      enemy->x += enemy->vx;
      enemy->y = 150; 
      if (enemy->x <= enemy->patrolStartX - CREATURE_PATROL_DISTANCE) {
        enemy->state = CREATURE_TURNING;
        enemy->subStateTimer = 0;
      }
      break;

    case CREATURE_TURNING:
      enemy->vx = 0; 
      enemy->vy = 0;

      if (enemy->subStateTimer >= CREATURE_TURN_FRAMES * 15) {
        if (enemy->facingRight) {
          enemy->state = CREATURE_PATROL_LEFT;
          enemy->vx = -CREATURE_SPEED;
          enemy->facingRight = 0; 
        } else {
          enemy->state = CREATURE_PATROL_RIGHT;
          enemy->vx = CREATURE_SPEED;
          enemy->facingRight = 1; 
        }
        enemy->subStateTimer = 0;
      }
      break;

    case CREATURE_ATTACKING:
      if (dx > 0)
        enemy->vx = CREATURE_ATTACK_SPEED;
      else if (dx < 0)
        enemy->vx = -CREATURE_ATTACK_SPEED;
      else
        enemy->vx = 0;
      if (dy > 0)
        enemy->vy = CREATURE_ATTACK_SPEED / 2;
      else if (dy < 0)
        enemy->vy = -CREATURE_ATTACK_SPEED / 2;
      else
        enemy->vy = 0;

      enemy->x += enemy->vx;
      enemy->y += enemy->vy;
      if (enemy->subStateTimer >= 60) { 
        enemy->state =
            enemy->facingRight ? CREATURE_PATROL_RIGHT : CREATURE_PATROL_LEFT;
        enemy->subStateTimer = 0;
        enemy->vx = enemy->facingRight ? CREATURE_SPEED : -CREATURE_SPEED;
        enemy->vy = 0;
      }
      break;

    case CREATURE_DEAD:
      if (enemy->subStateTimer >= CREATURE_BURST_FRAMES * 20) {
        enemy->active = 0; 
      }
      break;

    default:
      break;
    }

    if ((enemy->state == CREATURE_PATROL_LEFT ||
         enemy->state == CREATURE_PATROL_RIGHT) &&
        distance <= CREATURE_ATTACK_RANGE) {
      enemy->state = CREATURE_ATTACKING;
      enemy->subStateTimer = 0;
      enemy->vy = 0;
    }

    if (enemy->animationTimer >= 8) {
      enemy->animationTimer = 0;

      switch (enemy->state) {
      case CREATURE_LOADING:
        enemy->frame = (enemy->frame + 1) % CREATURE_NORMAL_FRAMES;
        break;
      case CREATURE_RISING:
        enemy->frame = (enemy->frame + 1) % CREATURE_FLYL_FRAMES;
        break;
      case CREATURE_PATROL_LEFT:
        enemy->frame = (enemy->frame + 1) % CREATURE_FLYL_FRAMES;
        break;
      case CREATURE_PATROL_RIGHT:
        enemy->frame = (enemy->frame + 1) % CREATURE_FLYR_FRAMES;
        break;
      case CREATURE_TURNING:
        enemy->frame = (enemy->frame + 1) % CREATURE_TURN_FRAMES;
        break;
      case CREATURE_ATTACKING:
        enemy->frame = (enemy->frame + 1) % CREATURE_ATTACK_FRAMES;
        break;
      case CREATURE_DEAD:
        enemy->frame = (enemy->frame + 1) % CREATURE_BURST_FRAMES;
        break;
      default:
        break;
      }
    }

    if (enemy->state != CREATURE_DEAD && player->state != DEATH &&player->invincibilityTimer == 0 &&checkCollision(player->x + 24, player->y + 24, 80,80, enemy->x + 16, enemy->y + 16, 32,32)) { 

        if (player->invincibilityTimer == 0) {
        int damagePercent =
            (enemy->state == CREATURE_ATTACKING) ? DAMAGE_PLAYER_TAKES : 10;
        int damage = PLAYER_MAX_HEALTH * damagePercent / 100;

        player->health -= damage;
        if (player->health < 0)
          player->health = 0;

        int invincibilityDuration =
            (enemy->state == CREATURE_ATTACKING) ? 60 : 10;
        player->invincibilityTimer = invincibilityDuration;
      }

      if (player->health <= 0) {
        player->state = DEATH;
        player->frame = 0;
        player->stateTimer = 0;
      }
    }

    int attackHitboxX = player->x;
    int attackHitboxY = player->y;
    int attackHitboxW = 0;
    int attackHitboxH = ATTACK_RANGE; 

    if (player->state == ATTACK_OVERHEAD_SLASHWAVE) {
      attackHitboxW = SLASHWAVE_RANGE;
      if (player->facingRight) {
        attackHitboxX = player->x + spriteW / 2; 
      } else {
        attackHitboxX = player->x - SLASHWAVE_RANGE; 
      }
    } else { 
      attackHitboxW = ATTACK_RANGE;
      if (player->facingRight) {
        attackHitboxX = player->x + spriteW / 2; 
      } else {
        attackHitboxW = ATTACK_RANGE_LEFT; 
        attackHitboxX = player->x - ATTACK_RANGE_LEFT; 
      }
    }
    int isPlayerAttacking_local =
        (player->state == ATTACK_OVERHEAD_RECOVER ||
         player->state == ATTACK_OVERHEAD_SLASHING ||
         player->state == ATTACK_OVERHEAD_SLASHWAVE ||
         player->state == DOWNSTAB_PRE || player->state == DOWNSTAB_ACTIVE ||
         player->state == DOWNSTAB_LAND);

    if (isPlayerAttacking_local &&
        checkCollision(attackHitboxX, attackHitboxY, attackHitboxW, attackHitboxH, enemy->x, enemy->y, CREATURE_SIZE, CREATURE_SIZE) && enemy->state != CREATURE_DEAD) {

      if (enemy->invincibilityTimer == 0) {
        int damagePercent = (player->state == DOWNSTAB_ACTIVE || player->state == DOWNSTAB_LAND) ? DAMAGE_DEALT_HEAVY : DAMAGE_DEALT_NORMAL;
        int damage = (enemy->maxHealth * damagePercent / 100) * player->damageMultiplier;
        enemy->currentHealth -= damage;
        enemy->invincibilityTimer = 90;

        if (enemy->currentHealth <= 0) {
          enemy->currentHealth = 0;
          enemy->state = CREATURE_DEAD;
          enemy->subStateTimer = 0;
          enemy->animationTimer = 0;
          enemy->frame = 0;
          enemy->vx = 0;
          enemy->vy = 0;
          tryDropItem(pickups, enemy->x, enemy->y);
        } else {
          enemy->damageAnimTimer = CREATURE_DAMAGE_FRAMES * 4;
          enemy->damageFrame = 0;
        }
      }
    }

    if (enemy->invincibilityTimer > 0) {
      enemy->invincibilityTimer--;
    }

    if (enemy->damageAnimTimer > 0) {
      enemy->damageAnimTimer--;
      if (enemy->damageAnimTimer % 4 == 0 && enemy->damageFrame < CREATURE_DAMAGE_FRAMES - 1) {
        enemy->damageFrame++;
      }
    }
  }
}

void renderCreatures(struct Creature creatures[], struct Camera *camera) {
  for (int i = 0; i < MAX_CREATURES; i++) {
    if (!creatures[i].active)
      continue;

    struct Creature *c = &creatures[i];
    unsigned int tex;

    switch (c->state) {
    case CREATURE_LOADING:
      tex = creatureNormal[c->frame % CREATURE_NORMAL_FRAMES];
      break;
    case CREATURE_RISING:
      tex = creatureFlyL[c->frame % CREATURE_FLYL_FRAMES];
      break;
    case CREATURE_PATROL_LEFT:
      tex = creatureFlyL[c->frame % CREATURE_FLYL_FRAMES];
      break;
    case CREATURE_PATROL_RIGHT:
      tex = creatureFlyR[c->frame % CREATURE_FLYR_FRAMES];
      break;
    case CREATURE_TURNING:
      tex = creatureTurn[c->frame % CREATURE_TURN_FRAMES];
      break;
    case CREATURE_ATTACKING:
      tex = creatureAttack[c->frame % CREATURE_ATTACK_FRAMES];
      break;
    case CREATURE_DEAD:
      tex = creatureBurst[c->frame % CREATURE_BURST_FRAMES];
      break;
    default:
      tex = creatureNormal[0]; 
      break;
    }

    float screenX = getScreenX(c->x, camera);
    float screenY = getScreenY(c->y, camera);
    iShowImage(screenX, screenY, CREATURE_SIZE, CREATURE_SIZE, tex);

    if (c->damageAnimTimer > 0) {
      unsigned int dmgTex =
          creatureDamage[c->damageFrame % CREATURE_DAMAGE_FRAMES];
      if (dmgTex != 0) {
        iShowImage(screenX, screenY, CREATURE_SIZE, CREATURE_SIZE, dmgTex);
      }
    }
  }
}

void initSparkles(struct SparkleEffect sparkles[]) {
  for (int i = 0; i < MAX_SPARKLES; i++) {
    sparkles[i].active = 0;
    sparkles[i].frame = 0;
    sparkles[i].animationTimer = 0;
  }
}

void spawnSparkle(struct SparkleEffect sparkles[], int x, int y) {
  for (int i = 0; i < MAX_SPARKLES; i++) {
    if (!sparkles[i].active) {
      sparkles[i].x = x;
      sparkles[i].y = y;
      sparkles[i].frame = 0;
      sparkles[i].active = 1;
      sparkles[i].animationTimer = 0;
      break;
    }
  }
}

void updateSparkles(struct SparkleEffect sparkles[]) {
  for (int i = 0; i < MAX_SPARKLES; i++) {
    if (!sparkles[i].active)
      continue;
    sparkles[i].animationTimer++;
    if (sparkles[i].animationTimer >= 6) {
      sparkles[i].animationTimer = 0;
      sparkles[i].frame++;
      if (sparkles[i].frame >= SPARKLE_FRAMES) {
        sparkles[i].active = 0;
      }
    }
  }
}

void renderSparkles(struct SparkleEffect sparkles[], struct Camera *camera) {
  for (int i = 0; i < MAX_SPARKLES; i++) {
    if (!sparkles[i].active)
      continue;
    float screenX = getScreenX(sparkles[i].x, camera);
    float screenY = getScreenY(sparkles[i].y, camera);
    if (sparkleTextures[sparkles[i].frame % SPARKLE_FRAMES] != 0) {
      iShowImage(screenX, screenY, 64, 64,
                 sparkleTextures[sparkles[i].frame % SPARKLE_FRAMES]);
    }
  }
}
#endif 
