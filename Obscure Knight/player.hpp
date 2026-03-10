#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "midground.hpp"
#include "sounds.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <windows.h>

void initPlayer(struct Player *player) {
  player->x = 200;
  player->y = GROUND_Y;
  player->vy = 0;
  player->frame = 0;
  player->onGround = 1;
  player->state = WAKE_PART1;
  player->facingRight = 1;
  player->stateTimer = 0;

  player->maxHealth = PLAYER_MAX_HEALTH;
  player->health = PLAYER_MAX_HEALTH;
  player->maxStamina = PLAYER_MAX_STAMINA;
  player->stamina = PLAYER_MAX_STAMINA;
  player->staminaRegenTimer = 0;
  player->invincibilityTimer = 0;
  player->isAttacking = 0;
  player->isTrapped = 0;
}

void setPlayerState(struct Player *player, enum State s) {
  if (player->state != s) {
    player->state = s;
    player->frame = 0;

    player->isAttacking =
        (s == ATTACK_OVERHEAD_RECOVER || s == ATTACK_OVERHEAD_SLASHING ||
         s == ATTACK_OVERHEAD_SLASHWAVE || s == DOWNSTAB_PRE ||
         s == DOWNSTAB_ACTIVE || s == DOWNSTAB_LAND);
  }
}

void handleInput(struct Player *player, int gameState) {
  int spriteW = SPRITE_SIZE * SCALE;

  int right = GetAsyncKeyState(VK_RIGHT) & 0x8000;
  int left = GetAsyncKeyState(VK_LEFT) & 0x8000;
  int jumpKey = GetAsyncKeyState(VK_SPACE) & 0x8000;
  int downKey = GetAsyncKeyState(VK_DOWN) & 0x8000;
  int sKey = GetAsyncKeyState('S') & 0x8000;
  if (sKey)
    downKey = 1;

  int shiftKey = GetAsyncKeyState(VK_SHIFT) & 0x8000;

  int dashKey = GetAsyncKeyState('Z') & 0x8000;
  int attackKey = GetAsyncKeyState('X') & 0x8000;
  int evadeKey = GetAsyncKeyState('C') & 0x8000;

  int isIntroOrDeathAnimation =
      (player->state == WAKE_PART1 || player->state == WAKE_PART2 ||
       player->state == WAKE_PART3 || player->state == ROAR_START ||
       player->state == ROAR_ACTIVE || player->state == ROAR_END ||
       player->state == DEATH);

  int canMove = (!player->isTrapped && !isIntroOrDeathAnimation &&
                 (player->state == IDLE || player->state == WALK ||
                  player->state == JUMP || player->state == FALL ||
                  player->state == LANDING || player->state == DASHING ||
                  player->state == ATTACK_OVERHEAD_RECOVER ||
                  player->state == ATTACK_OVERHEAD_SLASHING ||
                  player->state == ATTACK_OVERHEAD_SLASHWAVE));
  int canJump = (!player->isTrapped && player->onGround && canMove);
  int canDash = (!player->isTrapped && !isIntroOrDeathAnimation);
  int canAttack = (!player->isTrapped && !isIntroOrDeathAnimation);
  int canEvade = (!player->isTrapped && !isIntroOrDeathAnimation);
  int canDownstab =
      (!player->isTrapped && !isIntroOrDeathAnimation && !player->onGround);

  if (jumpKey && canJump) {
    player->vy = JUMP_V;
    player->onGround = 0;
    setPlayerState(player, JUMP);
    playJumpSound();
  } else if (downKey && player->onGround && player->y > GROUND_Y) {
    player->vy = JUMP_DOWN_VELOCITY;
    player->onGround = 0;
  }

  static int dashKeyWasPressed = 0;
  if (dashKey && !dashKeyWasPressed && canDash) {
    if (player->stamina >= STAMINA_COST_DASH) {
      setPlayerState(player, DASHING);
      player->stateTimer = 0;
      player->stamina -= STAMINA_COST_DASH;
      player->staminaRegenTimer = 0;
      if (player->stamina < 0)
        player->stamina = 0;
      playDashSound();
    }
  }
  dashKeyWasPressed = dashKey;

  if (attackKey && canAttack) {
    setPlayerState(player, ATTACK_OVERHEAD_RECOVER);
    player->stateTimer = 0;
  }

  if (player->state == DOWNSTAB_PRE || player->state == DOWNSTAB_ACTIVE ||
      player->state == DOWNSTAB_LAND) {
    player->invincibilityTimer = 2;
  }

  if (evadeKey && canEvade) {
    if (player->facingRight) {
      setPlayerState(player, EVADE_PRE);
    } else {
      setPlayerState(player, EVADE_PRE);
    }
    player->stateTimer = 0;
  }

  static int downKeyWasPressed = 0;
  if (downKey && !downKeyWasPressed && canDownstab) {
    if (player->stamina >= STAMINA_COST_DOWNSTAB) {
      setPlayerState(player, DOWNSTAB_PRE);
      player->stateTimer = 0;
      player->stamina -= STAMINA_COST_DOWNSTAB;
      player->staminaRegenTimer = 0;
      if (player->stamina < 0)
        player->stamina = 0;
      playDownslashSound();
    }
  }
  downKeyWasPressed = downKey;

  if (canMove) {
    if (right) {
      if (!player->facingRight && player->onGround &&
          player->state != ATTACK_OVERHEAD_RECOVER &&
          player->state != ATTACK_OVERHEAD_SLASHING &&
          player->state != ATTACK_OVERHEAD_SLASHWAVE) {
        player->facingRight = 1;
      }

      player->x += SPEED;
      if (player->onGround && player->state != ATTACK_OVERHEAD_RECOVER &&
          player->state != ATTACK_OVERHEAD_SLASHING &&
          player->state != ATTACK_OVERHEAD_SLASHWAVE) {
        setPlayerState(player, WALK);
      }
    } else if (left) {
      if (player->facingRight && player->onGround &&
          player->state != ATTACK_OVERHEAD_RECOVER &&
          player->state != ATTACK_OVERHEAD_SLASHING &&
          player->state != ATTACK_OVERHEAD_SLASHWAVE) {
        player->facingRight = 0;
      }

      player->x -= SPEED;
      if (player->onGround && player->state != ATTACK_OVERHEAD_RECOVER &&
          player->state != ATTACK_OVERHEAD_SLASHING &&
          player->state != ATTACK_OVERHEAD_SLASHWAVE) {
        setPlayerState(player, WALK);
      }
    } else {
      if (player->onGround && player->state != ATTACK_OVERHEAD_RECOVER &&
          player->state != ATTACK_OVERHEAD_SLASHING &&
          player->state != ATTACK_OVERHEAD_SLASHWAVE) {
        if (player->state != IDLE) {
          setPlayerState(player, IDLE);
        }
      }
    }
  }

  if (player->x < 0)
    player->x = 0;

  int maxX = TOTAL_BG_WIDTH - spriteW;
  if (gameState == BOSS_STATE) {
    maxX = BG_WIDTH - spriteW;
  }

  if (player->x > maxX)
    player->x = maxX;
}

void updatePhysics(struct Player *player) {
  int spriteH = SPRITE_SIZE * SCALE;
  int jumpDownActive =
      (GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('S') & 0x8000);
}

void updatePhysicsWithMap(struct Player *player, struct Midground *mg,
                          int gameState) {
  int spriteH = SPRITE_SIZE * SCALE;
  int jumpDownActive =
      (GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('S') & 0x8000);

  int currentGroundY = GROUND_Y;
  if (gameState == LEVEL2_STATE) {
    currentGroundY = LEVEL2_GROUND_Y;
    if (player->x >= LEVEL2_RAISED_GROUND_X) {
      currentGroundY = LEVEL2_RAISED_GROUND_Y;
    }
  } else if (gameState == LEVEL3_STATE) {
    currentGroundY = LEVEL3_GROUND_Y;
  } else if (gameState == BOSS_STATE) {
    currentGroundY = BOSS_GROUND_Y;
  }

  int inLevel2Pit =
      (gameState == LEVEL2_STATE && player->x >= LEVEL2_PIT_X_START &&
       player->x <= LEVEL2_PIT_X_END && player->x < LEVEL2_RAISED_GROUND_X);

  if (player->state == DASHING) {
    int dashDirection = player->facingRight ? 1 : -1;
    player->x += dashDirection * DASH_SPEED;
    player->y += player->vy;
  }

  else if (player->state == DOWNSTAB_PRE) {
    player->y += player->vy;
  }

  else if (player->state == DOWNSTAB_ACTIVE) {
    player->vy = DOWNSTAB_VELOCITY;
    player->y += player->vy;

    int platformY = 0;
    if (checkMidgroundCollision(player, mg, &platformY, jumpDownActive)) {
      if (player->y <= platformY && player->stateTimer >= 10) {
        player->y = platformY;
        player->vy = 0;
        player->onGround = 1;
        setPlayerState(player, DOWNSTAB_LAND);
        player->stateTimer = 0;
      }
    } else if (player->y <= currentGroundY && player->stateTimer >= 10 &&
               player->vy <= 0) {
      player->y = currentGroundY;
      player->vy = 0;
      player->onGround = 1;
      setPlayerState(player, DOWNSTAB_LAND);
      player->stateTimer = 0;
    }

  } else {
    if (!player->onGround) {
      player->y += player->vy;
      player->vy -= GRAVITY;

      if (player->state == JUMP && player->vy <= 0) {
        setPlayerState(player, FALL);
        playFallSound();
      }

      if (player->vy > 0) {
        int ceilingY = checkMidgroundCeilingCollision(player, mg);
        if (ceilingY != -1) {
          player->y = ceilingY - spriteH - 10;
          player->vy = 0;
        }
      }
    }
  }

  int platformY = 0;
  int onMidground =
      checkMidgroundCollision(player, mg, &platformY, jumpDownActive);

  if (onMidground) {
    if (player->vy <= 0) {
      player->y = platformY - 80;
      player->vy = 0;

      if (!player->onGround) {
        player->onGround = 1;

        if (player->state == FALL) {
          setPlayerState(player, LANDING);
          player->stateTimer = 0;
          playLandSound();
        } else if (player->state == JUMP) {
          setPlayerState(player, IDLE);
        }
      }
    }
  } else {

    int inTunnel = 0;
    if (gameState == PLAYING_STATE) {
      int tunnelHoleLeft = TUNNEL_HOLE_X - 60;
      int tunnelHoleRight = TUNNEL_HOLE_X + 60;
      int playerCenterX = player->x + (SPRITE_SIZE * SCALE) / 2;
      inTunnel = (playerCenterX >= tunnelHoleLeft &&
                  playerCenterX <= tunnelHoleRight && player->vy < -1);
    }

    if (player->y <= currentGroundY && !inTunnel && !inLevel2Pit) {
      player->y = currentGroundY;
      player->vy = 0;

      if (!player->onGround) {
        player->onGround = 1;

        if (player->state == FALL) {
          setPlayerState(player, LANDING);
          player->stateTimer = 0;
          playLandSound();
        } else if (player->state == JUMP) {
          setPlayerState(player, IDLE);
        }
      }
    } else {
      if (player->state != DASHING && player->state != DOWNSTAB_PRE &&
          player->state != DOWNSTAB_ACTIVE) {
        player->onGround = 0;
      }
    }
  }

  if (gameState == PLAYING_STATE) {
    int tunnelHoleLeftB = TUNNEL_HOLE_X - 100;
    int tunnelHoleRightB = TUNNEL_HOLE_X + 100;
    int playerCenterXB = player->x + (SPRITE_SIZE * SCALE) / 2;

    if (!(playerCenterXB >= tunnelHoleLeftB &&
          playerCenterXB <= tunnelHoleRightB && player->vy < -1)) {
      if (player->y < currentGroundY) {
        player->y = currentGroundY;
        player->vy = 0;
        player->onGround = 1;
      }
    }
  } else {
    if (player->y < currentGroundY && !inLevel2Pit) {
      player->y = currentGroundY;
      player->vy = 0;
      player->onGround = 1;
    }
  }
}

void updatePlayerAnimation(struct Player *player, struct Midground *mg,
                           int gameState) {
  int isIntroOrDeathAnimation =
      (player->state == WAKE_PART1 || player->state == WAKE_PART2 ||
       player->state == WAKE_PART3 || player->state == ROAR_START ||
       player->state == ROAR_ACTIVE || player->state == ROAR_END ||
       player->state == DEATH);

  handleInput(player, gameState);

  player->stateTimer++;

  if (player->stamina < player->maxStamina) {
    player->staminaRegenTimer++;
    if (player->staminaRegenTimer >= STAMINA_REGEN_INTERVAL) {
      player->stamina += STAMINA_REGEN_RATE;
      if (player->stamina > player->maxStamina)
        player->stamina = player->maxStamina;
      player->staminaRegenTimer = 0;
    }
    if (player->stamina < 0) {
      player->stamina = 0;
    }
  }

  if (player->invincibilityTimer > 0) {
    player->invincibilityTimer--;
  }

  if (player->state == DOWNSTAB_PRE || player->state == DOWNSTAB_ACTIVE ||
      player->state == DOWNSTAB_LAND) {
    player->invincibilityTimer = 2;
  }

  switch (player->state) {
  case WAKE_PART1:
    if (player->stateTimer >= WAKE_FRAMES_PART1 * INTRO_ANIMATION_SPEED) {
      setPlayerState(player, WAKE_PART2);
      player->stateTimer = 0;
    }
    break;
  case WAKE_PART2:
    if (player->stateTimer >= WAKE_FRAMES_PART2 * INTRO_ANIMATION_SPEED) {
      setPlayerState(player, WAKE_PART3);
      player->stateTimer = 0;
    }
    break;
  case WAKE_PART3:
    if (player->stateTimer >= WAKE_FRAMES_PART3 * INTRO_ANIMATION_SPEED) {
      setPlayerState(player, ROAR_START);
      player->stateTimer = 0;
    }
    break;
  case ROAR_START:
    if (player->stateTimer >= ROAR_FRAMES * INTRO_ANIMATION_SPEED) {
      setPlayerState(player, ROAR_ACTIVE);
      player->stateTimer = 0;
    }
    break;
  case ROAR_ACTIVE:
    if (player->stateTimer >= ROAR_ACTIVE_FRAMES * INTRO_ANIMATION_SPEED) {
      setPlayerState(player, ROAR_END);
      player->stateTimer = 0;
    }
    break;
  case ROAR_END:
    if (player->stateTimer >= ROAR_END_FRAMES * INTRO_ANIMATION_SPEED) {
      setPlayerState(player, IDLE);
      player->stateTimer = 0;
    }
    break;
  case DEATH:
    if (player->stateTimer >= DEATH_FRAMES * 8) {
      // Logic handled in game.hpp for respawn
      player->health = player->maxHealth;
      player->stamina = player->maxStamina;
      player->staminaRegenTimer = 0;
      player->invincibilityTimer = 0;

      setPlayerState(player, IDLE);
      player->stateTimer = 0;
    }
    break;

  case DASHING:
    if (player->stateTimer >= 12) {
      if (player->onGround) {
        int right = GetAsyncKeyState(VK_RIGHT);
        int left = GetAsyncKeyState(VK_LEFT);
        if (right || left) {
          setPlayerState(player, WALK);
        } else {
          setPlayerState(player, IDLE);
        }
      } else {
        setPlayerState(player, player->vy > 0 ? JUMP : FALL);
      }
    }
    break;

  case ATTACK_OVERHEAD_RECOVER:
    if (player->stateTimer >= OVERHEAD_ATTACK_RECOVER_FRAMES * 4) {
      setPlayerState(player, ATTACK_OVERHEAD_SLASHING);
      player->stateTimer = 0;
      playAttackSound();
    }
    break;

  case ATTACK_OVERHEAD_SLASHING:
    if (player->stateTimer >= OVERHEAD_ATTACK_SLASHING_FRAMES * 4) {
      setPlayerState(player, ATTACK_OVERHEAD_SLASHWAVE);
      player->stateTimer = 0;
    }
    break;

  case ATTACK_OVERHEAD_SLASHWAVE:
    if (player->stateTimer >= OVERHEAD_ATTACK_SLASHWAVE_FRAMES * 6) {

      player->invincibilityTimer = POST_ATTACK_INVINCIBILITY_DURATION;

      if (player->onGround) {
        int right = GetAsyncKeyState(VK_RIGHT) & 0x8000;
        int left = GetAsyncKeyState(VK_LEFT) & 0x8000;
        if (right || left) {
          setPlayerState(player, WALK);
        } else {
          setPlayerState(player, IDLE);
        }
      } else {
        setPlayerState(player, player->vy > 0 ? JUMP : FALL);
      }
    }
    break;

  case EVADE_PRE:
    if (player->stateTimer >= EVADE_LEFT_PRE_FRAMES * 8) {
      setPlayerState(player, EVADE_ACTIVE);
      player->stateTimer = 0;
    }
    break;

  case EVADE_ACTIVE:
    if (player->stateTimer >= EVADE_LEFT_ACTIVE_FRAMES * 8) {
      setPlayerState(player, EVADE_RECOVER);
      player->stateTimer = 0;
    }
    break;

  case EVADE_RECOVER:
    if (player->stateTimer >= EVADE_LEFT_RECOVER_FRAMES * 8) {
      if (player->onGround) {
        int right = GetAsyncKeyState(VK_RIGHT);
        int left = GetAsyncKeyState(VK_LEFT);
        if (right || left) {
          setPlayerState(player, WALK);
        } else {
          setPlayerState(player, IDLE);
        }
      } else {
        setPlayerState(player, player->vy > 0 ? JUMP : FALL);
      }
    }
    break;

  case DOWNSTAB_PRE:
    if (player->stateTimer >= DOWNSTAB_ANTICIPATE_FRAMES * 8) {
      setPlayerState(player, DOWNSTAB_ACTIVE);
      player->stateTimer = 0;
    }
    break;

  case DOWNSTAB_ACTIVE:
    if (player->stateTimer >= 120) {
      setPlayerState(player, DOWNSTAB_LAND);
      player->stateTimer = 0;
    }
    break;

  case DOWNSTAB_LAND:
    if (player->stateTimer >= DOWNSTAB_LAND_FRAMES * 8) {
      setPlayerState(player, IDLE);
      player->invincibilityTimer = DOWNSTAB_INVINCIBILITY_DURATION;
    }
    break;

  case LANDING:
    if (player->stateTimer >= LAND_FRAMES * 3) {
      setPlayerState(player, IDLE);
    }
    break;

  default:
    break;
  }

  if (isIntroOrDeathAnimation && player->state != DEATH) {
    switch (player->state) {
    case WAKE_PART1:
      player->frame =
          (player->stateTimer / INTRO_ANIMATION_SPEED) % WAKE_FRAMES_PART1;
      break;
    case WAKE_PART2:
      player->frame =
          (player->stateTimer / INTRO_ANIMATION_SPEED) % WAKE_FRAMES_PART2;
      break;
    case WAKE_PART3:
      player->frame =
          (player->stateTimer / INTRO_ANIMATION_SPEED) % WAKE_FRAMES_PART3;
      break;
    case ROAR_START:
      player->frame =
          (player->stateTimer / INTRO_ANIMATION_SPEED) % ROAR_FRAMES;
      break;
    case ROAR_ACTIVE:
      player->frame =
          (player->stateTimer / INTRO_ANIMATION_SPEED) % ROAR_ACTIVE_FRAMES;
      break;
    case ROAR_END:
      player->frame =
          (player->stateTimer / INTRO_ANIMATION_SPEED) % ROAR_END_FRAMES;
      break;
    default:
      break;
    }
  } else if (player->stateTimer % 8 == 0) {
    if (player->frame < 0)
      player->frame = 0;

    switch (player->state) {
    case IDLE:
      player->frame = (player->frame + 1) % IDLE_FRAMES;
      break;

    case WALK:
      player->frame = (player->frame + 1) % WALK_FRAMES;
      break;

    case TURNING:
      player->frame = (player->frame + 1) % TURN_FRAMES;
      break;

    case JUMP:
      player->frame = (player->frame + 1) % JUMP_LEFT_FRAMES;
      break;

    case FALL:
      player->frame = (player->frame + 1) % FALL_LEFT_FRAMES;
      break;

    case LANDING:
      player->frame = (player->frame + 1) % LAND_FRAMES;
      break;

    case DASHING:
      player->frame = (player->frame + 1) % DASH_LEFT_FRAMES;
      break;

    case ATTACK_OVERHEAD_RECOVER:
      player->frame = (player->frame + 1) % OVERHEAD_ATTACK_RECOVER_FRAMES;
      break;

    case ATTACK_OVERHEAD_SLASHING:
      player->frame = (player->frame + 1) % OVERHEAD_ATTACK_SLASHING_FRAMES;
      break;

    case ATTACK_OVERHEAD_SLASHWAVE:
      player->frame = (player->frame + 1) % OVERHEAD_ATTACK_SLASHWAVE_FRAMES;
      break;

    case EVADE_PRE:
      player->frame = (player->frame + 1) % EVADE_LEFT_PRE_FRAMES;
      break;

    case EVADE_ACTIVE:
      player->frame = (player->frame + 1) % EVADE_LEFT_ACTIVE_FRAMES;
      break;

    case EVADE_RECOVER:
      player->frame = (player->frame + 1) % EVADE_LEFT_RECOVER_FRAMES;
      break;

    case DOWNSTAB_PRE:
      player->frame = (player->frame + 1) % DOWNSTAB_ANTICIPATE_FRAMES;
      break;

    case DOWNSTAB_ACTIVE:
      player->frame = (player->frame + 1) % DOWNSTAB_SLAM_FRAMES;
      break;

    case DOWNSTAB_LAND:
      player->frame = (player->frame + 1) % DOWNSTAB_LAND_FRAMES;
      break;

    case DEATH:
      player->frame = (player->frame + 1) % DEATH_FRAMES;
      break;

    default:
      player->frame = 0;
      break;
    }
  }

  updatePhysicsWithMap(player, mg, gameState);
}

void renderPlayer(struct Player *player, struct Camera *camera) {
  unsigned int tex;
  int spriteW = SPRITE_SIZE * SCALE;
  int spriteH = SPRITE_SIZE * SCALE;

  switch (player->state) {
  case WAKE_PART1:
    tex = wakePart1[player->frame % WAKE_FRAMES_PART1];
    break;
  case WAKE_PART2:
    tex = wakePart2[player->frame % WAKE_FRAMES_PART2];
    break;
  case WAKE_PART3:
    tex = wakePart3[player->frame % WAKE_FRAMES_PART3];
    break;
  case ROAR_START:
    tex = roar[player->frame % ROAR_FRAMES];
    break;
  case ROAR_ACTIVE:
    tex = roarActive[player->frame % ROAR_ACTIVE_FRAMES];
    break;
  case ROAR_END:
    tex = roarEnd[player->frame % ROAR_END_FRAMES];
    break;
  case IDLE:
    tex = idle[player->frame % IDLE_FRAMES];
    break;

  case WALK:
    tex = player->facingRight ? walkRight[player->frame % WALK_FRAMES]
                              : walkLeft[player->frame % WALK_FRAMES];
    break;

  case TURNING:
    tex = player->facingRight ? turnRight[player->frame % TURN_FRAMES]
                              : turnLeft[player->frame % TURN_FRAMES];
    break;

  case JUMP:
    tex = player->facingRight ? jumpRight[player->frame % JUMP_RIGHT_FRAMES]
                              : jumpLeft[player->frame % JUMP_LEFT_FRAMES];
    break;

  case FALL:
    tex = player->facingRight ? fallRight[player->frame % FALL_RIGHT_FRAMES]
                              : fallLeft[player->frame % FALL_LEFT_FRAMES];
    break;

  case LANDING:
    tex = player->facingRight ? landRight[player->frame % LAND_FRAMES]
                              : landLeft[player->frame % LAND_FRAMES];
    break;

  case DASHING:
    tex = player->facingRight ? dashRight[player->frame % DASH_RIGHT_FRAMES]
                              : dashLeft[player->frame % DASH_LEFT_FRAMES];
    break;

  case ATTACK_OVERHEAD_RECOVER:
    tex = player->facingRight
              ? overheadAttackRecoverRight[player->frame %
                                           OVERHEAD_ATTACK_RECOVER_FRAMES]
              : overheadAttackRecoverLeft[player->frame %
                                          OVERHEAD_ATTACK_RECOVER_FRAMES];
    break;

  case ATTACK_OVERHEAD_SLASHING: {
    tex = player->facingRight
              ? overheadAttackSlashingRight[player->frame %
                                            OVERHEAD_ATTACK_SLASHING_FRAMES]
              : overheadAttackSlashingLeft[player->frame %
                                           OVERHEAD_ATTACK_SLASHING_FRAMES];

    float playerScreenX = getScreenX(player->x, camera);
    float playerScreenY = getScreenY(player->y, camera);

    iShowImage(playerScreenX, playerScreenY, spriteW, spriteH, tex);

    int slashwaveFrameIdx = player->frame % OVERHEAD_ATTACK_SLASHWAVE_FRAMES;
    unsigned int slashwaveTex =
        player->facingRight ? overheadAttackSlashwaveRight[slashwaveFrameIdx]
                            : overheadAttackSlashwaveLeft[slashwaveFrameIdx];

    if (slashwaveTex != 0) {
      iShowImage(playerScreenX, playerScreenY, spriteW, spriteH, slashwaveTex);
    }

    return;
  } break;

  case ATTACK_OVERHEAD_SLASHWAVE: {
    int frameIdx = player->frame % OVERHEAD_ATTACK_SLASHWAVE_FRAMES;
    tex = player->facingRight ? overheadAttackSlashwaveRight[frameIdx]
                              : overheadAttackSlashwaveLeft[frameIdx];
    float playerScreenX = getScreenX(player->x, camera);
    float playerScreenY = getScreenY(player->y, camera);
    if (tex != 0) {
      iShowImage(playerScreenX, playerScreenY, spriteW, spriteH, tex);
    }
    return;
  } break;

  case EVADE_PRE:
    tex = player->facingRight
              ? evadeRightPre[player->frame % EVADE_RIGHT_PRE_FRAMES]
              : evadeLeftPre[player->frame % EVADE_LEFT_PRE_FRAMES];
    break;

  case EVADE_ACTIVE:
    tex = player->facingRight
              ? evadeRightActive[player->frame % EVADE_RIGHT_ACTIVE_FRAMES]
              : evadeLeftActive[player->frame % EVADE_LEFT_ACTIVE_FRAMES];
    break;

  case EVADE_RECOVER:
    tex = player->facingRight
              ? evadeRightRecover[player->frame % EVADE_RIGHT_RECOVER_FRAMES]
              : evadeLeftRecover[player->frame % EVADE_LEFT_RECOVER_FRAMES];
    break;

  case DOWNSTAB_PRE:
    tex = downstabAnticipate[player->frame % DOWNSTAB_ANTICIPATE_FRAMES];
    break;

  case DOWNSTAB_ACTIVE:
    tex = downstabSlam[player->frame % DOWNSTAB_SLAM_FRAMES];
    break;

  case DOWNSTAB_LAND:
    tex = downstabLand[player->frame % DOWNSTAB_LAND_FRAMES];
    break;

  case DEATH:
    tex = death[player->frame % DEATH_FRAMES];
    break;

  default:
    tex = idle[0];
    break;
  }

  if (tex == 0) {
    tex = idle[player->frame % IDLE_FRAMES];
  }

  float playerScreenX = getScreenX(player->x, camera);
  float playerScreenY = getScreenY(player->y, camera);
  iShowImage(playerScreenX, playerScreenY, spriteW, spriteH, tex);
}

#endif
