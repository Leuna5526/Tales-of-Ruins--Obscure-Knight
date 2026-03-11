#ifndef MIDGROUND_HPP
#define MIDGROUND_HPP

#include "camera.hpp"
#include "iGraphics.h"
#include "structs.hpp"
#include <math.h>

void initMidground(struct Midground *mg) {
  mg->tileCount = 0;
  mg->tileTexture1 = 0;
  mg->tileTexture2 = 0;
  mg->tunnelHoleTexture = 0;

  for (int i = 0; i < MAX_TILES; i++) {
    mg->tiles[i].active = 0;
    mg->tiles[i].texture = 0;
    mg->tiles[i].x = 0;
    mg->tiles[i].y = 0;
  }
}

void loadMidgroundTextures(struct Midground *mg) {
  mg->tileTexture1 = iLoadImage("Assets/Midground/tile (1).png");
  mg->tileTexture2 = iLoadImage("Assets/Midground/tile (2).png");
  mg->tunnelHoleTexture = iLoadImage("Assets/Midground/tunnel hole.png");
}

void addTile(struct Midground *mg, int x, int y, int type) {
  if (mg->tileCount >= MAX_TILES)
    return;

  mg->tiles[mg->tileCount].x = x;

  if (type == 1) {
    mg->tiles[mg->tileCount].y = y - 7;
    mg->tiles[mg->tileCount].texture = mg->tileTexture1;
    mg->tiles[mg->tileCount].width = TILE_WIDTH * TILE_SCALE_FACTOR;
    mg->tiles[mg->tileCount].height = TILE_HEIGHT * TILE_SCALE_FACTOR;
  } else {
    mg->tiles[mg->tileCount].y = y - 5;
    mg->tiles[mg->tileCount].texture = mg->tileTexture2;
    mg->tiles[mg->tileCount].width = TILE_WIDTH;
    mg->tiles[mg->tileCount].height = TILE_HEIGHT;
  }

  mg->tiles[mg->tileCount].active = 1;
  mg->tiles[mg->tileCount].isJumpThrough = 0;
  mg->tileCount++;
}

void setupTiles(struct Midground *mg) {
  mg->tileCount = 0;
  int baseHeight = GROUND_Y + 100;

  addTile(mg, 1300, baseHeight, 1);
  addTile(mg, 1700, baseHeight + 50, 2);
  addTile(mg, 2050, baseHeight + 30, 1);

  addTile(mg, 2500, baseHeight + 60, 1);

  addTile(mg, 2900, baseHeight + 40, 2);
  addTile(mg, 3300, baseHeight + 90, 1);
  addTile(mg, 3700, baseHeight + 50, 2);
}

void renderMidground(struct Midground *mg, struct Camera *camera, int gameState) {
  for (int i = 0; i < mg->tileCount; i++) {
    if (!mg->tiles[i].active)
      continue;

    float screenX = getScreenX(mg->tiles[i].x, camera);
    float screenY = getScreenY(mg->tiles[i].y, camera);

    if (screenX > -mg->tiles[i].width &&
        screenX < SCREEN_W + mg->tiles[i].width &&
        screenY > -mg->tiles[i].height &&
        screenY < SCREEN_H + mg->tiles[i].height) {
      if (mg->tiles[i].texture != 0) {
        iShowImage(screenX, screenY, mg->tiles[i].width, mg->tiles[i].height,
                   mg->tiles[i].texture);
      }
    }
  }

  if (gameState == PLAYING_STATE && mg->tunnelHoleTexture != 0) {
    float tunnelScreenX = getScreenX(TUNNEL_HOLE_X - 100, camera);
    float tunnelScreenY = getScreenY(TUNNEL_HOLE_Y, camera);

    if (tunnelScreenX > -200 && tunnelScreenX < SCREEN_W + 200 &&
        tunnelScreenY > -200 && tunnelScreenY < SCREEN_H + 200) {
      iShowImage(tunnelScreenX, tunnelScreenY, 200, 200, mg->tunnelHoleTexture);
    }
  }
}

int checkMidgroundCollision(struct Player *player, struct Midground *mg,
                            int *platformY, int jumpDownActive) {
  int playerLeft = player->x;
  int spriteW = SPRITE_SIZE * SCALE;
  int spriteH = SPRITE_SIZE * SCALE;

  int playerRight = player->x + spriteW;
  int playerBottom = player->y;
  int playerTop = player->y + spriteH;
  int playerCenterX = player->x + spriteW / 2;
  int playerCenterY = player->y + spriteH / 2;

  int tunnelLeft = TUNNEL_HOLE_X - 75;
  int tunnelRight = TUNNEL_HOLE_X + 75;
  int tunnelTop = TUNNEL_HOLE_Y + 150;
  int tunnelBottom = TUNNEL_HOLE_Y - 50;

  if (playerCenterX >= tunnelLeft && playerCenterX <= tunnelRight &&
      playerCenterY >= tunnelBottom && playerCenterY <= tunnelTop) {
    return 0;
  }

  int highestPlatformY = -1;
  int foundCollision = 0;

  for (int i = 0; i < mg->tileCount; i++) {
    if (!mg->tiles[i].active)
      continue;

    int tileLeft = mg->tiles[i].x;
    int tileRight = mg->tiles[i].x + mg->tiles[i].width;
    int tileTop = mg->tiles[i].y + mg->tiles[i].height;

    int margin = 40;
    if (playerRight > tileLeft + margin && playerLeft < tileRight - margin) {
      if (jumpDownActive && mg->tiles[i].isJumpThrough &&
          playerBottom >= tileTop - (spriteH / 2) && playerBottom <= tileTop) {
        continue;
      }

      if ((playerBottom <= tileTop + 20 && playerBottom >= tileTop - 90 &&
           player->vy <= 0) ||
          (abs(playerBottom - tileTop) <= 90 && player->vy == 0)) {
        int effectiveHeight = tileTop;

        if (mg->tiles[i].texture == mg->tileTexture2) {
          effectiveHeight += 20;
        }


        if (effectiveHeight > highestPlatformY) {
          highestPlatformY = effectiveHeight;
          foundCollision = 1;
        }
      }
    }
  }

  if (foundCollision) {
    *platformY = highestPlatformY;
    return 1;
  }

  return 0;
}

int checkMidgroundCeilingCollision(struct Player *player,
                                   struct Midground *mg) {
  int spriteW = SPRITE_SIZE * SCALE;
  int spriteH = SPRITE_SIZE * SCALE;
  int playerTop = player->y + spriteH;
  int playerLeft = player->x;
  int playerRight = player->x + spriteW;

  int solidTop = playerTop - 40;

  for (int i = 0; i < mg->tileCount; i++) {
    if (!mg->tiles[i].active)
      continue;

    int tileBottom = mg->tiles[i].y;

    if (mg->tiles[i].texture == mg->tileTexture2) {
      tileBottom += 25;
    }
    else if (mg->tiles[i].texture == mg->tileTexture1) {
      tileBottom += 40;
    }

    int tileLeft = mg->tiles[i].x;
    int tileRight = mg->tiles[i].x + mg->tiles[i].width;

    if (playerRight > tileLeft + 30 && playerLeft < tileRight - 30) {
      if (solidTop >= tileBottom && solidTop <= tileBottom + 15) {
        return tileBottom;
      }
    }
  }
  return -1;
}

#endif
