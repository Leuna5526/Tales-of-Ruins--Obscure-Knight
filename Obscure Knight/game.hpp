#ifndef GAME_HPP
#define GAME_HPP

#include "Sentry.hpp"
#include "background.hpp"
#include "bug.hpp"
#include "camera.hpp"
#include "cave.hpp"
#include "midground.hpp"
#include "player.hpp"
#include "sounds.hpp"

extern struct NPC npc;

void updateGame(struct Player *player, struct Creature creatures[],
                struct Background *bg, struct Midground *mg,
                struct Camera *camera, int *gameState, struct Pickup pickups[],
                struct SparkleEffect sparkles[]) {

  if (*gameState == PLAYING_STATE || *gameState == LEVEL2_STATE) {
    updatePlayerAnimation(player, mg, *gameState);
    updateCreatures(creatures, player, pickups, *gameState);

    updateBackground(bg, player);
    updateCamera(camera, player);

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
          restartBGMusic();
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
