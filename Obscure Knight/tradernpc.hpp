#ifndef TRADERNPC_HPP
#define TRADERNPC_HPP

#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "player.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <math.h>

void initTraderNPC(struct TraderNPC *t) {
  t->x = TRADER_NPC_X;
  t->y = TRADER_NPC_Y;
  t->frame = 0;
  t->animTimer = 0;
  t->state = TRADER_IDLE_STATE;
  t->stateTimer = 0;
  t->facingRight = 0;
  t->active = 1;
  t->traded = 0;
  t->initialX = TRADER_NPC_X;
}

void updateTraderNPC(struct TraderNPC *t, struct Player *player) {
  if (!t->active || t->state == TRADER_DONE)
    return;

  float dx = (float)(player->x - t->x);
  float dy = (float)(player->y - t->y);
  float distance = sqrt(dx * dx + dy * dy);

  t->stateTimer++;
  t->animTimer++;

  // Animation tick
  if (t->animTimer >= TRADER_ANIM_SPEED) {
    t->animTimer = 0;

    switch (t->state) {
    case TRADER_IDLE_STATE:
      t->frame = (t->frame + 1) % TRADER_IDLE_FRAMES;
      break;
    case TRADER_WALK_TO_PLAYER:
      if (t->facingRight)
        t->frame = (t->frame + 1) % TRADER_WALK_R_FRAMES;
      else
        t->frame = (t->frame + 1) % TRADER_WALK_L_FRAMES;
      break;
    case TRADER_TURN_STATE:
      t->frame++;
      if (t->frame >= TRADER_TURN_FRAMES)
        t->frame = TRADER_TURN_FRAMES - 1;
      break;
    case TRADER_TRADING:
      t->frame = (t->frame + 1) % TRADER_TRADE_FRAMES;
      break;
    case TRADER_WALK_AWAY:
      if (t->facingRight)
        t->frame = (t->frame + 1) % TRADER_WALK_R_FRAMES;
      else
        t->frame = (t->frame + 1) % TRADER_WALK_L_FRAMES;
      break;
    case TRADER_WALK_BACK:
      if (t->facingRight)
        t->frame = (t->frame + 1) % TRADER_WALK_R_FRAMES;
      else
        t->frame = (t->frame + 1) % TRADER_WALK_L_FRAMES;
      break;
    default:
      break;
    }
  }

  // State logic
  switch (t->state) {
  case TRADER_IDLE_STATE:
    // Check if player is out of range from NPC's initial position
    {
      float dxFromInit = (float)(player->x - t->initialX);
      float distFromInit = (dxFromInit < 0) ? -dxFromInit : dxFromInit;
      if (distFromInit > TRADER_DETECTION_RADIUS && t->x != t->initialX) {
        // Player left — walk back to initial position
        t->state = TRADER_WALK_BACK;
        t->frame = 0;
        t->stateTimer = 0;
        t->facingRight = (t->initialX > t->x) ? 1 : 0;
        break;
      }
    }
    // Detect player within radius
    if (distance <= TRADER_DETECTION_RADIUS) {
      t->state = TRADER_WALK_TO_PLAYER;
      t->frame = 0;
      t->stateTimer = 0;
      // Face towards player
      t->facingRight = (dx > 0) ? 1 : 0;
    }
    break;

  case TRADER_WALK_TO_PLAYER: {
    // Check if player walked away during approach
    float dxFromInit = (float)(player->x - t->initialX);
    float distFromInit = (dxFromInit < 0) ? -dxFromInit : dxFromInit;
    if (distFromInit > TRADER_DETECTION_RADIUS) {
      // Player left — walk back to initial position
      t->state = TRADER_WALK_BACK;
      t->frame = 0;
      t->stateTimer = 0;
      t->facingRight = (t->initialX > t->x) ? 1 : 0;
      break;
    }

    // Walk towards player
    if (dx > 0) {
      t->x += TRADER_WALK_SPEED;
      t->facingRight = 1;
    } else {
      t->x -= TRADER_WALK_SPEED;
      t->facingRight = 0;
    }

    // Stop when close enough
    if (distance <= TRADER_STOP_RADIUS) {
      t->state = TRADER_PROMPT_INTERACT;
      t->frame = 0;
      t->stateTimer = 0;
    }
    break;
  }

  case TRADER_PROMPT_INTERACT: {
    // Check if player walked away while waiting
    float dxFromInit = (float)(player->x - t->initialX);
    float distFromInit = (dxFromInit < 0) ? -dxFromInit : dxFromInit;
    if (distFromInit > TRADER_DETECTION_RADIUS) {
      t->state = TRADER_WALK_BACK;
      t->frame = 0;
      t->stateTimer = 0;
      t->facingRight = (t->initialX > t->x) ? 1 : 0;
      break;
    }
    // Animate idle while waiting
    if (t->animTimer == 0) {
      t->frame = (t->frame + 1) % TRADER_IDLE_FRAMES;
    }
    break;
  }

  case TRADER_TURN_STATE:
    // Turning to face player
    if (t->stateTimer >= TRADER_TURN_FRAMES * TRADER_ANIM_SPEED) {
      // After turning, show key prompt
      t->facingRight = (dx > 0) ? 1 : 0;
      t->state = TRADER_SHOW_KEY;
      t->frame = 0;
      t->stateTimer = 0;
    }
    break;

  case TRADER_SHOW_KEY:
    // Idle while showing key prompt - waiting for Space/X input
    if (t->animTimer == 0) {
      t->frame = (t->frame + 1) % TRADER_IDLE_FRAMES;
    }
    break;

  case TRADER_TRADING:
    // Trade animation playing - make it slower and last longer
    if (t->stateTimer >= TRADER_TRADE_FRAMES * TRADER_ANIM_SPEED * 12) {
      t->state = TRADER_IDLE_STATE;
      t->traded = 1;
      t->active = 1; // Explicitly keep active
    }
    break;

  case TRADER_WALK_AWAY:
    // Walk away from player (opposite direction)
    if (t->facingRight)
      t->x += TRADER_WALK_SPEED;
    else
      t->x -= TRADER_WALK_SPEED;

    // Deactivate when far enough away or off screen
    if (t->x < -200 || t->x > TOTAL_BG_WIDTH + 200 ||
        t->stateTimer > 200) {
      t->state = TRADER_DONE;
      t->active = 0;
    }
    break;

  case TRADER_WALK_BACK:
    // Walk back towards initial position
    if (t->initialX > t->x) {
      t->x += TRADER_WALK_SPEED;
      t->facingRight = 1;
    } else if (t->initialX < t->x) {
      t->x -= TRADER_WALK_SPEED;
      t->facingRight = 0;
    }

    // Reached initial position (within walking speed tolerance)
    if (abs(t->x - t->initialX) <= TRADER_WALK_SPEED) {
      t->x = t->initialX;
      t->state = TRADER_IDLE_STATE;
      t->frame = 0;
      t->stateTimer = 0;
    }
    break;

  case TRADER_DONE:
    break;
  }
}

void renderTraderNPC(struct TraderNPC *t, struct Camera *camera) {
  if (!t->active || t->state == TRADER_DONE)
    return;

  unsigned int tex = 0;

  switch (t->state) {
  case TRADER_IDLE_STATE:
  case TRADER_PROMPT_INTERACT:
  case TRADER_SHOW_KEY:
    tex = traderIdle[t->frame % TRADER_IDLE_FRAMES];
    break;
  case TRADER_WALK_TO_PLAYER:
    if (t->facingRight)
      tex = traderWalkR[t->frame % TRADER_WALK_R_FRAMES];
    else
      tex = traderWalkL[t->frame % TRADER_WALK_L_FRAMES];
    break;
  case TRADER_TURN_STATE:
    tex = traderTurn[t->frame % TRADER_TURN_FRAMES];
    break;
  case TRADER_TRADING:
    tex = traderTrade[t->frame % TRADER_TRADE_FRAMES];
    break;
  case TRADER_WALK_AWAY:
  case TRADER_WALK_BACK:
    if (t->facingRight)
      tex = traderWalkR[t->frame % TRADER_WALK_R_FRAMES];
    else
      tex = traderWalkL[t->frame % TRADER_WALK_L_FRAMES];
    break;
  default:
    tex = traderIdle[0];
    break;
  }

  if (tex != 0) {
    float screenX = getScreenX((float)t->x, camera);
    float screenY = getScreenY((float)t->y, camera);
    iShowImage(screenX, screenY, TRADER_SIZE, TRADER_SIZE, tex);
  }

  // Render prompts
  if (t->state == TRADER_PROMPT_INTERACT) {
    float screenX = getScreenX((float)t->x, camera);
    float screenY = getScreenY((float)t->y, camera);
    iSetColor(255, 255, 255);
    iText((int)(screenX - 30), (int)(screenY + TRADER_SIZE + 15),
          "Enter E to interact", GLUT_BITMAP_HELVETICA_18);
  }

  if (t->state == TRADER_SHOW_KEY) {
    float screenX = getScreenX((float)t->x, camera);
    float screenY = getScreenY((float)t->y, camera);

    // Show the key image 3 times larger
    if (traderKeyTex != 0) {
      float keyScreenY = getScreenY((float)(LEVEL3_GROUND_Y + 40), camera);
      iShowImage((int)(screenX + TRADER_SIZE / 2 - 96),
                 (int)keyScreenY, 192, 192, traderKeyTex);
    }

    iSetColor(255, 255, 255);
    iText((int)(screenX - 60), (int)(screenY + TRADER_SIZE + 110),
          "Space: Collect Key  /  X: Ignore",
          GLUT_BITMAP_HELVETICA_18);
  }
}

#endif
