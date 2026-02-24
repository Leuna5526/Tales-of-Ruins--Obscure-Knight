#ifndef CAVE_HPP
#define CAVE_HPP

#include "config.hpp"
#include "structs.hpp"
#include "sounds.hpp"
#include "player.hpp"
#include "midground.hpp"

// Cave Coordinate Defines
#define CAVE_P1_Y 10
#define CAVE_P2_Y 260
#define CAVE_P3_Y 450

#define CAVE_P1_X_MIN 300
#define CAVE_P1_X_MAX 710
#define CAVE_P2_X_MIN 390
#define CAVE_P2_X_MAX 650
#define CAVE_P3_X_MIN 0
#define CAVE_P3_X_MAX 400

// Scripted Jump Zones
#define CAVE_JUMP_P1_TO_P2_MIN 300
#define CAVE_JUMP_P1_TO_P2_MAX 380
#define CAVE_JUMP_P2_TO_P3_MIN 390
#define CAVE_JUMP_P2_TO_P3_MAX 450

// Teleport Targets
#define CAVE_P2_LAND_X 420
#define CAVE_P3_LAND_X 250

// Exit Condition
#define CAVE_EXIT_X 20

void updateCaveState(struct Player *player, struct Background *bg, struct Midground *mg, struct Camera *camera, int *gameState, struct Pickup pickups[]) {
    int right = GetAsyncKeyState(VK_RIGHT) & 0x8000;
    int left  = GetAsyncKeyState(VK_LEFT) & 0x8000;
    int jumpKey = GetAsyncKeyState(VK_SPACE) & 0x8000;
    static int caveJumpWas = 0;
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
            if (player->y <= CAVE_P3_Y && player->x >= CAVE_P3_X_MIN && player->x <= CAVE_P3_X_MAX) {
                player->y = CAVE_P3_Y;
                player->vy = 0;
                player->onGround = 1;
                setPlayerState(player, LANDING);
                player->stateTimer = 0;
                playLandSound();
            }
            else if (player->y <= CAVE_P2_Y && player->x >= CAVE_P2_X_MIN && player->x <= CAVE_P2_X_MAX) {
                player->y = CAVE_P2_Y;
                player->vy = 0;
                player->onGround = 1;
                setPlayerState(player, LANDING);
                player->stateTimer = 0;
                playLandSound();
            }
            else if (player->y <= CAVE_P1_Y) {
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
            if (onP1 && right && player->x >= CAVE_JUMP_P1_TO_P2_MIN && player->x <= CAVE_JUMP_P1_TO_P2_MAX) {
                player->x = CAVE_P2_LAND_X; player->y = CAVE_P2_Y;
                player->onGround = 1; player->vy = 0;
                setPlayerState(player, IDLE); playJumpSound();
            } else if (onP2 && left && player->x >= CAVE_JUMP_P2_TO_P3_MIN && player->x <= CAVE_JUMP_P2_TO_P3_MAX) {
                player->x = CAVE_P3_LAND_X; player->y = CAVE_P3_Y;
                player->onGround = 1; player->vy = 0;
                setPlayerState(player, IDLE); playJumpSound();
            } else {
                player->vy = JUMP_V; player->onGround = 0;
                setPlayerState(player, JUMP); playJumpSound();
            }
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
