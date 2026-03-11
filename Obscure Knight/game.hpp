#ifndef GAME_HPP
#define GAME_HPP

#include "Sentry.hpp"
#include "background.hpp"
#include "boss.hpp"
#include "bug.hpp"
#include "camera.hpp"
#include "cave.hpp"
#include "grimmaster.hpp"
#include "midground.hpp"
#include "player.hpp"
#include "sounds.hpp"

extern struct NPC npc;
extern struct Boss boss;
extern struct BossMinion bossMinions[MAX_BOSS_MINIONS];
extern struct BossHazard bossHazards[MAX_BOSS_HAZARDS];
extern struct TraderNPC traderNpc;
extern struct GrimMaster grims[MAX_GRIMS];
extern struct GrimFireball grimFireballs[MAX_GRIM_FIREBALLS];

void updateGame(struct Player *player, struct Creature creatures[],
                struct Background *bg, struct Midground *mg,
                struct Camera *camera, int *gameState, struct Pickup pickups[],
                struct SparkleEffect sparkles[]) {

  if (*gameState == PLAYING_STATE || *gameState == LEVEL2_STATE ||
      *gameState == LEVEL3_STATE || *gameState == BOSS_STATE) {
    updatePlayerAnimation(player, mg, *gameState);

    // Only update creatures for Playing and Level 2 right now.
    // If you add Level 3 or Boss enemies later, add them here.
    if (*gameState == PLAYING_STATE || *gameState == LEVEL2_STATE) {
      updateCreatures(creatures, player, pickups, *gameState);
    }

    // Only scroll background for scrolling levels
    if (*gameState != BOSS_STATE) {
      updateBackground(bg, player);
    }

    // Boss state has a fixed camera, so we only update camera for others
    if (*gameState == BOSS_STATE) {
      camera->x = 0;
      camera->targetX = 0;
    } else {
      updateCamera(camera, player);
    }

    if (*gameState == PLAYING_STATE && player->y < -100) {
      *gameState = TUNNEL_STATE;
      bg->x = 0;
      player->y = 600;
      player->x = 500;
      player->vy = 0;
      camera->x = 0;
      camera->targetX = 0;
    }

    if (*gameState == LEVEL2_STATE && player->y < -100) {
      *gameState = CAVE_STATE;
      player->x = CAVE_ENTRY_X;
      player->y = CAVE_ENTRY_Y;
      player->vy = 0;
      player->onGround = 0;
      player->state = FALL;
      player->frame = 0;
      player->stateTimer = 0;
      camera->x = 0;
      camera->targetX = 0;
      mg->tileCount = 0;
      initNPC(&npc);
      spawnCaveItems(pickups);
    }
    if (*gameState == LEVEL2_STATE && player->x >= LEVEL2_END_X) {
      *gameState = LEVEL3_STATE;
      player->x = 200;
      player->y = LEVEL3_GROUND_Y;
      player->vy = 0;
      player->onGround = 1;
      setPlayerState(player, IDLE);
      player->stateTimer = 0;
      camera->x = 0;
      camera->targetX = 0;
      bg->x = 0;
      mg->tileCount = 0;
      // Initialize trader NPC
      traderNpc.x = TRADER_NPC_X;
      traderNpc.y = TRADER_NPC_Y;
      traderNpc.frame = 0;
      traderNpc.animTimer = 0;
      traderNpc.state = TRADER_IDLE_STATE;
      traderNpc.stateTimer = 0;
      traderNpc.facingRight = 0;
      traderNpc.active = 1;
      traderNpc.traded = 0;
      // Initialize grims
      initGrims(grims);
      initGrimFireballs(grimFireballs);
      // Setup Level 3 tiles (same pattern as Level 1/2)
      {
        mg->tileTexture1 = level3Tile1;
        mg->tileTexture2 = level3Tile2;

        mg->tiles[0].x = LEVEL3_TILE_1_X;
        mg->tiles[0].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_1_Y;
        mg->tiles[0].texture = level3Tile1;
        mg->tiles[0].width = LEVEL3_TILE_W;
        mg->tiles[0].height = LEVEL3_TILE_H;
        mg->tiles[0].active = 1;
        mg->tiles[0].isJumpThrough = 0;
        mg->tileCount++;

        mg->tiles[1].x = LEVEL3_TILE_2_X;
        mg->tiles[1].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_2_Y;
        mg->tiles[1].texture = level3Tile2;
        mg->tiles[1].width = LEVEL3_TILE_WIDE_W;
        mg->tiles[1].height = LEVEL3_TILE_H;
        mg->tiles[1].active = 1;
        mg->tiles[1].isJumpThrough = 0;
        mg->tileCount++;

        mg->tiles[2].x = LEVEL3_TILE_3_X;
        mg->tiles[2].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_3_Y;
        mg->tiles[2].texture = level3Tile1;
        mg->tiles[2].width = LEVEL3_TILE_W;
        mg->tiles[2].height = LEVEL3_TILE_H;
        mg->tiles[2].active = 1;
        mg->tiles[2].isJumpThrough = 0;
        mg->tileCount++;

        mg->tiles[3].x = LEVEL3_TILE_4_X;
        mg->tiles[3].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_4_Y;
        mg->tiles[3].texture = level3Tile2;
        mg->tiles[3].width = LEVEL3_TILE_WIDE_W;
        mg->tiles[3].height = LEVEL3_TILE_H;
        mg->tiles[3].active = 1;
        mg->tiles[3].isJumpThrough = 0;
        mg->tileCount++;

        mg->tiles[4].x = LEVEL3_TILE_5_X;
        mg->tiles[4].y = LEVEL3_TILE_BASE_HEIGHT + LEVEL3_TILE_5_Y;
        mg->tiles[4].texture = level3Tile1;
        mg->tiles[4].width = LEVEL3_TILE_W;
        mg->tiles[4].height = LEVEL3_TILE_H;
        mg->tiles[4].active = 1;
        mg->tiles[4].isJumpThrough = 0;
        mg->tileCount++;
      }
    }

    if (*gameState == LEVEL3_STATE && player->x >= LEVEL3_END_X) {
      // Only allow entry to boss if player has used the key
      if (player->hasUsedKey) {
        player->hasUsedKey = 0; // consume the state
        *gameState = BOSS_STATE;
        player->x = 200;
        player->y = BOSS_GROUND_Y;
        player->vy = 0;
        player->onGround = 1;
        setPlayerState(player, IDLE);
        player->stateTimer = 0;
        camera->x = 0;
        camera->targetX = 0;
        // Re-initialize boss when entering boss level
        initBoss(&boss);
        initMinions(bossMinions);
        initHazards(bossHazards);
        // Switch music: stop bg2, play boss theme
        stopBG2Music();
        playBossBGMusic();
      } else {
        // Block player from going further
        player->x = LEVEL3_END_X - 10;
      }
    }

    // Player death check — health drops to 0
    if ((*gameState == PLAYING_STATE || *gameState == LEVEL2_STATE ||
         *gameState == LEVEL3_STATE || *gameState == BOSS_STATE) &&
        player->health <= 0 && player->state != DEATH) {
      setPlayerState(player, DEATH);
      player->stateTimer = 0;
    }
    // After death animation finishes, respawn at start of current level
    if ((*gameState == PLAYING_STATE || *gameState == LEVEL2_STATE ||
         *gameState == LEVEL3_STATE || *gameState == BOSS_STATE) &&
        player->state == DEATH &&
        player->stateTimer > DEATH_ANIMATION_DURATION) {
      // Respawn position depends on which level the player is in
      if (*gameState == PLAYING_STATE) {
        player->x = 200;
        player->y = GROUND_Y;
      } else if (*gameState == LEVEL2_STATE) {
        player->x = 200;
        player->y = LEVEL2_GROUND_Y;
        camera->x = 0;
        camera->targetX = 0;
        bg->x = 0;
        mg->tileCount = 0;
      } else if (*gameState == LEVEL3_STATE) {
        *gameState = LEVEL3_STATE;
        player->x = 200;
        player->y = LEVEL3_GROUND_Y;
        // Reset grims on respawn
        initGrims(grims);
        initGrimFireballs(grimFireballs);
      } else if (*gameState == BOSS_STATE) {
        *gameState = BOSS_STATE;
        player->x = 200;
        player->y = BOSS_GROUND_Y;
        // Reset boss on death
        initBoss(&boss);
        initMinions(bossMinions);
        initHazards(bossHazards);
      }
      player->health = player->maxHealth;
      player->vy = 0;
      player->onGround = 1;
      player->invincibilityTimer = 60;
      player->isTrapped = 0;
      setPlayerState(player, IDLE);
      player->stateTimer = 0;
      camera->x = 0;
      camera->targetX = 0;
      bg->x = 0;
    }
  } else if (*gameState == CAVE_STATE) {
    updateCaveState(player, bg, mg, camera, gameState, pickups);
  } else if (*gameState == TUNNEL_STATE) {
    if (bg->x == 0 || bg->x == 1) {
      player->state = FALL;
      player->y -= 12;
      player->x = 500;

      player->stateTimer++;
      if (player->stateTimer % 8 == 0) {
        player->frame = (player->frame + 1) % FALL_LEFT_FRAMES;
      }

      if (player->y <= 0) {
        if (bg->x == 0) {
          bg->x = 2;
          bg->tunnelTransitionY = 0;
        } else {
          *gameState = LEVEL2_STATE;
          stopBGMusic();
          playBG2Music();
          initCreatures(creatures);

          initSparkles(sparkles);
          spawnSparkle(sparkles, SPARKLE_1_X, SPARKLE_1_Y);
          spawnSparkle(sparkles, SPARKLE_2_X, SPARKLE_2_Y);
          spawnSparkle(sparkles, SPARKLE_3_X, SPARKLE_3_Y);
          spawnSparkle(sparkles, SPARKLE_4_X, SPARKLE_4_Y);
          spawnSparkle(sparkles, SPARKLE_5_X, SPARKLE_5_Y);

          player->y = 600;
          player->x = 200;
          player->vy = 0;
          bg->x = 0;
          mg->tileCount = 0;
          camera->x = 0;
          camera->targetX = 0;

          int baseHeight = LEVEL2_GROUND_Y + 100;
          mg->tileTexture1 = level2Tile1;
          mg->tileTexture2 = level2TileFlat;

          mg->tiles[0].x = 600;
          mg->tiles[0].y = baseHeight;
          mg->tiles[0].texture = level2Tile1;
          mg->tiles[0].width = TILE_WIDTH;
          mg->tiles[0].height = TILE_HEIGHT;
          mg->tiles[0].active = 1;
          mg->tiles[0].isJumpThrough = 0;
          mg->tileCount++;

          mg->tiles[1].x = 1200;
          mg->tiles[1].y = baseHeight + 50;
          mg->tiles[1].texture = level2TileFlat;
          mg->tiles[1].width = TILE_WIDTH * 1.5;
          mg->tiles[1].height = TILE_HEIGHT;
          mg->tiles[1].active = 1;
          mg->tiles[1].isJumpThrough = 0;
          mg->tileCount++;

          mg->tiles[2].x = 2000;
          mg->tiles[2].y = baseHeight + 30;
          mg->tiles[2].texture = level2Tile1;
          mg->tiles[2].width = TILE_WIDTH;
          mg->tiles[2].height = TILE_HEIGHT;
          mg->tiles[2].active = 1;
          mg->tiles[2].isJumpThrough = 0;
          mg->tileCount++;

          mg->tiles[3].x = 2800;
          mg->tiles[3].y = baseHeight + 60;
          mg->tiles[3].texture = level2TileFlat;
          mg->tiles[3].width = TILE_WIDTH * 1.5;
          mg->tiles[3].height = TILE_HEIGHT;
          mg->tiles[3].active = 1;
          mg->tiles[3].isJumpThrough = 0;
          mg->tileCount++;

          mg->tiles[4].x = 3500;
          mg->tiles[4].y = baseHeight + 40;
          mg->tiles[4].texture = level2Tile1;
          mg->tiles[4].width = TILE_WIDTH;
          mg->tiles[4].height = TILE_HEIGHT;
          mg->tiles[4].active = 1;
          mg->tiles[4].isJumpThrough = 0;
          mg->tileCount++;
        }
      }
    } else if (bg->x == 2) {
      bg->tunnelTransitionY += 12;
      if (bg->tunnelTransitionY >= 600) {
        bg->x = 1;
        player->y = 600;
      }
      player->state = FALL;
      player->y = 600 - bg->tunnelTransitionY;

      player->stateTimer++;
      if (player->stateTimer % 8 == 0) {
        player->frame = (player->frame + 1) % FALL_LEFT_FRAMES;
      }
    }
  }
}

#endif
