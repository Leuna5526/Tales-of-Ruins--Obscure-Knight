#ifndef TEXTURES_H
#define TEXTURES_H

#include "config.h"

// ---------------- TEXTURES ----------------
// Movement & Physics Assets
extern unsigned int idle[IDLE_FRAMES];
extern unsigned int walkLeft[WALK_FRAMES];
extern unsigned int walkRight[WALK_FRAMES];
extern unsigned int turnLeft[TURN_FRAMES];
extern unsigned int turnRight[TURN_FRAMES];
extern unsigned int jumpLeft[JUMP_LEFT_FRAMES];
extern unsigned int jumpRight[JUMP_RIGHT_FRAMES];
extern unsigned int fallLeft[FALL_LEFT_FRAMES];
extern unsigned int fallRight[FALL_RIGHT_FRAMES];
extern unsigned int landLeft[LAND_FRAMES];
extern unsigned int landRight[LAND_FRAMES];

// Combat & Action Assets
extern unsigned int dashLeft[DASH_LEFT_FRAMES];
extern unsigned int dashRight[DASH_RIGHT_FRAMES];

// Downstab (direction-agnostic)
extern unsigned int downstabAnticipate[DOWNSTAB_ANTICIPATE_FRAMES];
extern unsigned int downstabSlam[DOWNSTAB_SLAM_FRAMES];
extern unsigned int downstabLand[DOWNSTAB_LAND_FRAMES];

// Overhead Attack (directional)
extern unsigned int overheadAttackRecoverLeft[OVERHEAD_ATTACK_RECOVER_FRAMES];
extern unsigned int overheadAttackRecoverRight[OVERHEAD_ATTACK_RECOVER_FRAMES];
extern unsigned int overheadAttackSlashingLeft[OVERHEAD_ATTACK_SLASHING_FRAMES];
extern unsigned int overheadAttackSlashingRight[OVERHEAD_ATTACK_SLASHING_FRAMES];

// Evade
extern unsigned int evadeLeftPre[EVADE_LEFT_PRE_FRAMES];
extern unsigned int evadeLeftActive[EVADE_LEFT_ACTIVE_FRAMES];
extern unsigned int evadeLeftRecover[EVADE_LEFT_RECOVER_FRAMES];
extern unsigned int evadeRightPre[EVADE_RIGHT_PRE_FRAMES];
extern unsigned int evadeRightActive[EVADE_RIGHT_ACTIVE_FRAMES];
extern unsigned int evadeRightRecover[EVADE_RIGHT_RECOVER_FRAMES];

// Death
extern unsigned int death[DEATH_FRAMES];

// Background textures (array of 5 segments)
extern unsigned int backgroundTextures[BG_SEGMENTS];

// Creature textures
extern unsigned int creatureNormal[CREATURE_NORMAL_FRAMES];    // Ground loading
extern unsigned int creatureFlyL[CREATURE_FLYL_FRAMES];        // Flying left
extern unsigned int creatureFlyR[CREATURE_FLYR_FRAMES];        // Flying right
extern unsigned int creatureTurn[CREATURE_TURN_FRAMES];        // Turning
extern unsigned int creatureAttack[CREATURE_ATTACK_FRAMES];    // Attacking
extern unsigned int creatureBurst[CREATURE_BURST_FRAMES];      // Death burst

// ---------------- TEXTURE FUNCTIONS ----------------
void loadSet(unsigned int* arr, int n, const char* fmt);
void loadImages();
void loadBackgroundTexture();
void loadCreatureTextures();

#endif // TEXTURES_H