#ifndef CAVE_HPP
#define CAVE_HPP

#include "config.hpp"
#include "structs.hpp"
#include "sounds.hpp"
#include "player.hpp"
#include "midground.hpp"
#include <stdlib.h>

#define CAVE_P1_Y 10
#define CAVE_P2_Y 260
#define CAVE_P3_Y 450

#define CAVE_P1_X_MIN 300
#define CAVE_P1_X_MAX 710
#define CAVE_P2_X_MIN 380
#define CAVE_P2_X_MAX 650
#define CAVE_P3_X_MIN 0
#define CAVE_P3_X_MAX 335

void spawnCaveItems(struct Pickup pickups[]) {
    for (int i = 0; i < MAX_PICKUPS; i++) pickups[i].active = 0;

    int numItems = 2 + (rand() % 2); 
    
    for (int i = 0; i < numItems; i++) {
        for (int j = 0; j < MAX_PICKUPS; j++) {
            if (!pickups[j].active) {
                pickups[j].active = 1;
                pickups[j].type = (enum ItemType)(rand() % 4);
                
                int pIdx = rand() % 3;
                int xMin, xMax, y;
                if (pIdx == 0) { xMin = CAVE_P1_X_MIN; xMax = CAVE_P1_X_MAX; y = CAVE_P1_Y; }
                else if (pIdx == 1) { xMin = CAVE_P2_X_MIN; xMax = CAVE_P2_X_MAX; y = CAVE_P2_Y; }
                else { xMin = CAVE_P3_X_MIN; xMax = CAVE_P3_X_MAX; y = CAVE_P3_Y; }
                
                pickups[j].x = xMin + (rand() % (xMax - xMin - PICKUP_SIZE));
                pickups[j].y = y + 10; 
                break;
            }
        }
    }
}

#define CAVE_JUMP_P1_TO_P2_MIN 300
#define CAVE_JUMP_P1_TO_P2_MAX 380
#define CAVE_JUMP_P2_TO_P3_MIN 380
#define CAVE_JUMP_P2_TO_P3_MAX 450

#define CAVE_P2_LAND_X 420
#define CAVE_P3_LAND_X 250

#define CAVE_EXIT_X 20

#define CAVE_NPC_X 275
#define CAVE_NPC_Y 18

void initNPC(struct NPC *npc) {
  npc->x = CAVE_NPC_X;
  npc->y = CAVE_NPC_Y;
  npc->frame = 0;
  npc->state = NPC_STILL;
  npc->stateTimer = 0;
  npc->dialogueIndex = -1;
  npc->showPrompt = 0;
  npc->isInteracting = 0;
  npc->initialized = 1;
}

void updateNPC(struct NPC *npc, struct Player *player) {
  float dx = (float)(npc->x - player->x);
  float dy = (float)(npc->y - player->y);
  float dist = sqrt(dx * dx + dy * dy);

  if (npc->state == NPC_STILL) {
    if (dist < 180) {
      npc->state = NPC_APPROACHING;
      npc->frame = 0;
      npc->stateTimer = 0;
    }
  }

  npc->stateTimer++;
  if (npc->stateTimer % 5 == 0) {
    if (npc->state == NPC_APPROACHING) {
      npc->frame++;
      if (npc->frame >= NPC_APPROACH_FRAMES) {
        npc->state = NPC_IDLE_LEFT;
        npc->frame = 0;
      }
    } else if (npc->state == NPC_IDLE_LEFT) {
      npc->frame++;
      if (npc->frame >= NPC_IDLE_LEFT_FRAMES) {
        npc->state = NPC_IDLE_RIGHT;
        npc->frame = 0;
      }
    } else if (npc->state == NPC_IDLE_RIGHT) {
      npc->frame++;
      if (npc->frame >= NPC_IDLE_RIGHT_FRAMES) {
        npc->state = NPC_TURNING;
        npc->frame = 0;
      }
    } else if (npc->state == NPC_TURNING) {
      npc->frame++;
      if (npc->frame >= NPC_TURN_FRAMES) {
        npc->frame = NPC_TURN_FRAMES - 1;
      }
    } else if (npc->state == NPC_SUMMON_START) {
      npc->frame++;
      if (npc->frame >= NPC_SUMMON_START_FRAMES) {
        npc->state = NPC_SUMMONING;
        npc->frame = 0;
      }
    } else if (npc->state == NPC_SUMMONING) {
      npc->frame = (npc->frame + 1) % NPC_SUMMONING_FRAMES;
    } else if (npc->state == NPC_RETREAT) {
      npc->frame++;
      if (npc->frame >= NPC_RETREAT_FRAMES) {
        npc->state = NPC_DONE;
        npc->frame = 0;
      }
    }
  }

  if (npc->state == NPC_TURNING && !npc->isInteracting) {
    if (dist < 150) {
      npc->showPrompt = 1;
    } else {
      npc->showPrompt = 0;
    }
  } else {
    npc->showPrompt = 0;
  }
}

void renderNPC(struct NPC *npc, struct Camera *camera) {
  if (npc->state == NPC_STILL || npc->state == NPC_DONE) return;

  unsigned int tex = 0;
  if (npc->state == NPC_APPROACHING) {
    tex = npcApproach[npc->frame];
  } else if (npc->state == NPC_IDLE_LEFT) {
    tex = npcIdleLeft[npc->frame];
  } else if (npc->state == NPC_IDLE_RIGHT) {
    tex = npcIdleRight[npc->frame];
  } else if (npc->state == NPC_TURNING) {
    tex = npcTurn[npc->frame];
  } else if (npc->state == NPC_SUMMON_START) {
    tex = npcSummonStart[npc->frame];
  } else if (npc->state == NPC_SUMMONING) {
    tex = npcSummoning[npc->frame];
  } else if (npc->state == NPC_RETREAT) {
    tex = npcRetreat[npc->frame];
  }

  if (tex) {
    iShowImage(npc->x - camera->x, npc->y - camera->y, SPRITE_SIZE * SCALE,
               SPRITE_SIZE * SCALE, tex);
  }

  if (npc->showPrompt) {
    iSetColor(255, 255, 255);
    iText(npc->x - camera->x + 20, npc->y - camera->y + 120,
          "press E to interact", GLUT_BITMAP_HELVETICA_18);
  }
}

void renderDialogue(struct NPC *npc) {
  if (npc->isInteracting && npc->dialogueIndex >= 0 &&
      npc->dialogueIndex < DIALOGUE_COUNT) {
    int dialW = 600;
    int dialH = 300;
    int dialX = (SCREEN_W - dialW) / 2;
    int dialY = (SCREEN_H - dialH) / 2;

    iShowImage(dialX, dialY, dialW, dialH, dialogues[npc->dialogueIndex]);
    
    iSetColor(255, 255, 255);
    iText(dialX + dialW/2 - 80, dialY - 40, "press enter to continue",
          GLUT_BITMAP_HELVETICA_18);
  }
}

extern struct NPC npc;

void updateCaveState(struct Player *player, struct Background *bg, struct Midground *mg, struct Camera *camera, int *gameState, struct Pickup pickups[]) {
    int right = (GetAsyncKeyState(VK_RIGHT) & 0x8000) && !npc.isInteracting;
    int left  = (GetAsyncKeyState(VK_LEFT) & 0x8000) && !npc.isInteracting;
    int jumpKey = (GetAsyncKeyState(VK_SPACE) & 0x8000) && !npc.isInteracting;
    static int caveJumpWas = 0;

    static int eKeyWas = 0;
    int eKey = (GetAsyncKeyState('E') & 0x8000);
    if (eKey && !eKeyWas && npc.showPrompt && !npc.isInteracting && npc.state == NPC_TURNING) {
        npc.isInteracting = 1;
        npc.dialogueIndex = 0;
        npc.state = NPC_SUMMON_START;
        npc.frame = 0;
        npc.showPrompt = 0;
    }
    eKeyWas = eKey;

    static int enterKeyWas = 0;
    int enterKey = (GetAsyncKeyState(VK_RETURN) & 0x8000);
    if (enterKey && !enterKeyWas && npc.isInteracting) {
        npc.dialogueIndex++;
        if (npc.dialogueIndex >= DIALOGUE_COUNT) {
            npc.isInteracting = 0;
            npc.dialogueIndex = -1;
            npc.state = NPC_RETREAT;
            npc.frame = 0;
        }
    }
    enterKeyWas = enterKey;

    int spriteW = (int)(SPRITE_SIZE * SCALE);

    if (!player->onGround) {
        player->y += player->vy;
        player->vy -= GRAVITY;

        if (player->state == JUMP && player->vy <= 0)
            setPlayerState(player, FALL);

        player->stateTimer++;
        if (player->stateTimer % 8 == 0) {
            if (player->state == FALL)
                player->frame = (player->frame + 1) % FALL_LEFT_FRAMES;
            else if (player->state == JUMP)
                player->frame = (player->frame + 1) % JUMP_LEFT_FRAMES;
        }

        if (right) { player->x += SPEED; player->facingRight = 1; }
        else if (left) { player->x -= SPEED; player->facingRight = 0; }

        if (player->x < 0) player->x = 0;
        if (player->x > SCREEN_W - spriteW) player->x = SCREEN_W - spriteW;

        if (player->vy <= 0) {
            float prevY = (float)(player->y - (player->vy + GRAVITY));
            if (player->y <= CAVE_P3_Y && prevY >= CAVE_P3_Y && player->x >= CAVE_P3_X_MIN && player->x <= CAVE_P3_X_MAX) {
                player->y = CAVE_P3_Y;
                player->vy = 0;
                player->onGround = 1;
                setPlayerState(player, LANDING);
                player->stateTimer = 0;
                playLandSound();
            }
            else if (player->y <= CAVE_P2_Y && prevY >= CAVE_P2_Y && player->x >= CAVE_P2_X_MIN && player->x <= CAVE_P2_X_MAX) {
                player->y = CAVE_P2_Y;
                player->vy = 0;
                player->onGround = 1;
                setPlayerState(player, LANDING);
                player->stateTimer = 0;
                playLandSound();
            }
            else if (player->y <= CAVE_P1_Y && prevY >= CAVE_P1_Y) {
                player->y = CAVE_P1_Y;
                player->vy = 0;
                player->onGround = 1;
                setPlayerState(player, LANDING);
                player->stateTimer = 0;
                playLandSound();
                if (player->x < CAVE_P1_X_MIN) player->x = CAVE_P1_X_MIN;
                if (player->x > CAVE_P1_X_MAX) player->x = CAVE_P1_X_MAX;
            }
        }
    } else {
        int onP1 = (player->y == CAVE_P1_Y);
        int onP2 = (player->y == CAVE_P2_Y);
        int onP3 = (player->y == CAVE_P3_Y);

        if (player->state != LANDING) {
            if (right) {
                player->x += SPEED; player->facingRight = 1;
                if (player->state != WALK) setPlayerState(player, WALK);
            } else if (left) {
                player->x -= SPEED; player->facingRight = 0;
                if (player->state != WALK) setPlayerState(player, WALK);
            } else {
                if (player->state == WALK) setPlayerState(player, IDLE);
            }
        }

        if (jumpKey && !caveJumpWas) {
            player->vy = 25; 
            player->onGround = 0;
            setPlayerState(player, JUMP); 
            playJumpSound();
        }
        caveJumpWas = jumpKey;

        if (onP1) {
            if (player->x < CAVE_P1_X_MIN) player->x = CAVE_P1_X_MIN;
            if (player->x > CAVE_P1_X_MAX) player->x = CAVE_P1_X_MAX;
        } else if (onP2) {
            if (player->x < CAVE_P2_X_MIN || player->x > CAVE_P2_X_MAX) {
                player->onGround = 0; player->vy = 0;
                setPlayerState(player, FALL); player->stateTimer = 0;
            }
        } else if (onP3) {
            if (player->x < CAVE_P3_X_MIN) player->x = CAVE_P3_X_MIN;
            if (player->x > CAVE_P3_X_MAX) {
                player->onGround = 0; player->vy = 0;
                setPlayerState(player, FALL); player->stateTimer = 0;
            }
            if (player->x <= CAVE_EXIT_X && player->onGround) {
                *gameState = LEVEL2_STATE;
                player->x = LEVEL2_RAISED_GROUND_X + 30;
                player->y = LEVEL2_RAISED_GROUND_Y;
                player->vy = 0;
                player->onGround = 1;
                setPlayerState(player, IDLE);
                player->stateTimer = 0;
                camera->x = player->x - SCREEN_W / 2;
                camera->targetX = camera->x;
                mg->tileCount = 0;
                int baseHeight = LEVEL2_GROUND_Y + 100;
                mg->tileTexture1 = level2Tile1;
                mg->tileTexture2 = level2TileFlat;
                mg->tiles[0].x = 600; mg->tiles[0].y = baseHeight;
                mg->tiles[0].texture = level2Tile1; mg->tiles[0].width = TILE_WIDTH;
                mg->tiles[0].height = TILE_HEIGHT; mg->tiles[0].active = 1;
                mg->tiles[0].isJumpThrough = 0; mg->tileCount++;
                mg->tiles[1].x = 1200; mg->tiles[1].y = baseHeight + 50;
                mg->tiles[1].texture = level2TileFlat; mg->tiles[1].width = TILE_WIDTH * 1.5;
                mg->tiles[1].height = TILE_HEIGHT; mg->tiles[1].active = 1;
                mg->tiles[1].isJumpThrough = 0; mg->tileCount++;
                mg->tiles[2].x = 2000; mg->tiles[2].y = baseHeight + 30;
                mg->tiles[2].texture = level2Tile1; mg->tiles[2].width = TILE_WIDTH;
                mg->tiles[2].height = TILE_HEIGHT; mg->tiles[2].active = 1;
                mg->tiles[2].isJumpThrough = 0; mg->tileCount++;
                mg->tiles[3].x = 2800; mg->tiles[3].y = baseHeight + 60;
                mg->tiles[3].texture = level2TileFlat; mg->tiles[3].width = TILE_WIDTH * 1.5;
                mg->tiles[3].height = TILE_HEIGHT; mg->tiles[3].active = 1;
                mg->tiles[3].isJumpThrough = 0; mg->tileCount++;
                mg->tiles[4].x = 3500; mg->tiles[4].y = baseHeight + 40;
                mg->tiles[4].texture = level2Tile1; mg->tiles[4].width = TILE_WIDTH;
                mg->tiles[4].height = TILE_HEIGHT; mg->tiles[4].active = 1;
                mg->tiles[4].isJumpThrough = 0; mg->tileCount++;
            }
        }

        player->stateTimer++;
        if (player->stamina < player->maxStamina) {
            player->staminaRegenTimer++;
            if (player->staminaRegenTimer >= STAMINA_REGEN_INTERVAL) {
                player->stamina += STAMINA_REGEN_RATE;
                if (player->stamina > player->maxStamina)
                    player->stamina = player->maxStamina;
                player->staminaRegenTimer = 0;
            }
        }
        if (player->invincibilityTimer > 0) player->invincibilityTimer--;

        if (player->stateTimer % 8 == 0) {
            if (player->frame < 0) player->frame = 0;
            switch (player->state) {
            case IDLE: player->frame = (player->frame + 1) % IDLE_FRAMES; break;
            case WALK: player->frame = (player->frame + 1) % WALK_FRAMES; break;
            case LANDING: player->frame = (player->frame + 1) % LAND_FRAMES; break;
            default: break;
            }
        }
        if (player->state == LANDING && player->stateTimer >= LAND_FRAMES * 3)
            setPlayerState(player, IDLE);
    }
}

#endif
