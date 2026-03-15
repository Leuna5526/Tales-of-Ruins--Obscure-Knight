#ifndef CONFIG_HPP
#define CONFIG_HPP

#define ATTACK_FRAMES 10
#define PLAYER_MAX_HEALTH 100
#define ATTACK_RANGE 60
#define ATTACK_RANGE_LEFT 2
#define SLASHWAVE_RANGE 20
#define ATTACK_HORIZONTAL_OFFSET 40
#define POST_ATTACK_INVINCIBILITY_DURATION 0
#define DOWNSTAB_INVINCIBILITY_DURATION 180

#define TITLE_SCREEN_STATE 0
#define PLAYING_STATE 1
#define TUNNEL_STATE 2
#define LEVEL2_STATE 3
#define CREDITS_STATE 4
#define CONTROLS_STATE 5
#define CAVE_STATE 6
#define LOADING_STATE 7
#define LEVEL3_STATE 8
#define BOSS_STATE 9
#define GAME_OVER_STATE 10

#define GAME_OVER_FRAMES 65

#define SCREEN_W 1000
#define SCREEN_H 600

#define GROUND_Y 25
#define SPEED 6
#define JUMP_V 18
#define GRAVITY 1
#define LEVEL2_GROUND_Y 2
#define LEVEL2_PIT_X_START 4012
#define LEVEL2_PIT_X_END 4210
#define LEVEL2_RAISED_GROUND_X 4210
#define LEVEL2_RAISED_GROUND_Y 150

#define CAVE_ENTRY_X 710
#define CAVE_ENTRY_Y 600
#define CAVE_LAND_X 710
#define CAVE_LAND_Y 10
#define CAVE_GROUND_Y 10

#define LEVEL3_GROUND_Y 55
#define BOSS_GROUND_Y 63
#define LEVEL2_END_X 4870
#define LEVEL3_END_X 4870

// ============================================================
// GREETING NPC (end of Level 2)
// ============================================================
#define GREET_IDLE_FRAMES 1
#define GREET_ANIM_FRAMES 8
#define GREET_ANIM_SPEED 8
#define GREET_SIZE_W 75
#define GREET_SIZE_H 150
#define GREET_X 4850
#define GREET_Y LEVEL2_RAISED_GROUND_Y + 50
#define GREET_TRIGGER_X_START 4750
#define GREET_TRIGGER_X_END LEVEL2_END_X

// Level 3 tiles
#define LEVEL3_TILE_COUNT 5
#define LEVEL3_TILE_W TILE_WIDTH
#define LEVEL3_TILE_H TILE_HEIGHT
#define LEVEL3_TILE_WIDE_W (int)(TILE_WIDTH * 1.5)
#define LEVEL3_TILE_BASE_HEIGHT (LEVEL3_GROUND_Y + 125)
// Tile positions (X, Y offset from base)
#define LEVEL3_TILE_1_X 700
#define LEVEL3_TILE_1_Y 0
#define LEVEL3_TILE_2_X 1500
#define LEVEL3_TILE_2_Y 50
#define LEVEL3_TILE_3_X 2500
#define LEVEL3_TILE_3_Y 30
#define LEVEL3_TILE_4_X 3400
#define LEVEL3_TILE_4_Y 60
#define LEVEL3_TILE_5_X 4200
#define LEVEL3_TILE_5_Y 40

// Trader key display size (shown during trade prompt)
#define TRADER_KEY_DISPLAY_SIZE 85
#define TRADER_KEY_PROMPT_Y 110
#define TRADER_KEY_PROMPT_X_OFFSET (TRADER_SIZE / 2 - TRADER_KEY_DISPLAY_SIZE / 2)

#define DASH_SPEED 6
#define EVADE_SPEED 10
#define DOWNSTAB_VELOCITY -25

#define PLAYER_MAX_HEALTH 100
#define PLAYER_MAX_STAMINA 100
#define BUG_MAX_HEALTH 100
#define SENTRY_MAX_HEALTH 200

#define DAMAGE_PLAYER_TAKES 20
#define DAMAGE_DEALT_NORMAL 20
#define DAMAGE_DEALT_HEAVY 35

#define STAMINA_COST_DASH 20
#define STAMINA_COST_DOWNSTAB 20
#define STAMINA_REGEN_RATE 1
#define STAMINA_REGEN_INTERVAL 20

#define UI_BAR_X 20
#define UI_BAR_Y 490
#define UI_BAR_WIDTH 250
#define UI_BAR_HEIGHT 100
#define UI_BAR_SPACING 50

#define IDLE_FRAMES 8
#define WALK_FRAMES 7
#define TURN_FRAMES 1
#define JUMP_LEFT_FRAMES 8
#define JUMP_RIGHT_FRAMES 8
#define FALL_LEFT_FRAMES 7
#define FALL_RIGHT_FRAMES 7
#define LAND_FRAMES 7

#define DASH_LEFT_FRAMES 5
#define DASH_RIGHT_FRAMES 5
#define DOWNSTAB_ANTICIPATE_FRAMES 6
#define DOWNSTAB_SLAM_FRAMES 5
#define DOWNSTAB_LAND_FRAMES 7

#define OVERHEAD_ATTACK_RECOVER_FRAMES 2
#define OVERHEAD_ATTACK_SLASHING_FRAMES 7
#define OVERHEAD_ATTACK_SLASHWAVE_FRAMES 3

#define EVADE_LEFT_PRE_FRAMES 2
#define EVADE_LEFT_ACTIVE_FRAMES 4
#define EVADE_LEFT_RECOVER_FRAMES 1
#define EVADE_RIGHT_PRE_FRAMES 2
#define EVADE_RIGHT_ACTIVE_FRAMES 4
#define EVADE_RIGHT_RECOVER_FRAMES 1

#define DEATH_FRAMES 16
#define WAKE_FRAMES_PART1 3
#define WAKE_FRAMES_PART2 3
#define WAKE_FRAMES_PART3 3
#define ROAR_FRAMES 4
#define ROAR_ACTIVE_FRAMES 2
#define ROAR_END_FRAMES 1
#define INTRO_ANIMATION_SPEED 15
#define DEATH_ANIMATION_DURATION 100

#define SPRITE_SIZE 512
#define SCALE 0.25

#define BG_SCROLL_SPEED 2
#define BG_WIDTH 1000
#define BG_HEIGHT 600
#define BG_SEGMENTS 5
#define TOTAL_BG_WIDTH (BG_WIDTH * BG_SEGMENTS)

#define MAX_TILES 100
#define TILE_WIDTH 128
#define TILE_HEIGHT 64
#define TILE_SCALE_FACTOR 1.5
#define JUMP_DOWN_VELOCITY -15
#define TILES_PER_ROW 2
#define TILE_ROWS_ABOVE 2
#define TILE_ROWS_BELOW 2
#define TILE_SPACING_X 150
#define TILE_SPACING_Y 100
#define TUNNEL_HOLE_X 4930
#define TUNNEL_HOLE_Y GROUND_Y

#define CAMERA_FOLLOW_SPEED 0.1
#define CAMERA_EDGE_MARGIN 200

#define MAX_CREATURES 3
#define CREATURE_NORMAL_FRAMES 5
#define CREATURE_FLYL_FRAMES 6
#define CREATURE_FLYR_FRAMES 5
#define CREATURE_TURN_FRAMES 2
#define CREATURE_ATTACK_FRAMES 3
#define CREATURE_BURST_FRAMES 5
#define CREATURE_DAMAGE_FRAMES 3
#define CREATURE_SPEED 2
#define CREATURE_ATTACK_SPEED 4
#define CREATURE_ATTACK_RANGE 150
#define CREATURE_DETECTION_RANGE 300
#define CREATURE_SPAWN_TRIGGER 400
#define CREATURE_SIZE 64
#define CREATURE_PATROL_DISTANCE 120
#define SPAWN_POINT_1_X 800
#define SPAWN_POINT_2_X 2500
#define SPAWN_POINT_3_X 4500
#define SPAWN_POINT_Y GROUND_Y

#define SENTRY_IDLE_FRAMES 5
#define SENTRY_WALKL_FRAMES 8
#define SENTRY_WALKR_FRAMES 8
#define SENTRY_RUNL_FRAMES 8
#define SENTRY_RUNR_FRAMES 8
#define SENTRY_ATTACKL_FRAMES 12
#define SENTRY_ATTACKR_FRAMES 12
#define SENTRY_SLASHL_FRAMES 4
#define SENTRY_SLASHR_FRAMES 4
#define SENTRY_TURNL_FRAMES 2
#define SENTRY_TURNR_FRAMES 2
#define SENTRY_WAKE_FRAMES 7
#define SENTRY_DEATH_FRAMES 3
#define SENTRY_DEATHAIR_FRAMES 2
#define SENTRY_DAMAGE_FRAMES 3
#define SENTRY_SIZE 128
#define SENTRY_SPEED 2
#define SENTRY_ATTACK_SPEED 4
#define SENTRY_ATTACK_RANGE 150
#define SENTRY_DETECTION_RANGE 250
#define SENTRY_PATROL_DISTANCE 120
#define SENTRY_MAX_CHASE_DIST 250
#define SENTRY_RUN_SPEED 5
#define SENTRY_JUMP_V 18
#define SENTRY_IDLE_CHANCE 20
#define SENTRY_ATTACK_JUMP_CHANCE 40
#define MAX_SENTRIES 3
#define SENTRY_SPAWN_1_X 800
#define SENTRY_SPAWN_2_X 2500
#define SENTRY_SPAWN_3_X 4000

#define SPARKLE_FRAMES 8
#define MAX_SPARKLES 5

#define SPARKLE_1_X 800
#define SPARKLE_1_Y 350
#define SPARKLE_2_X 850
#define SPARKLE_2_Y 325
#define SPARKLE_3_X 3000
#define SPARKLE_3_Y 330
#define SPARKLE_4_X 3800
#define SPARKLE_4_Y 480
#define SPARKLE_5_X 4800
#define SPARKLE_5_Y 500

#define TITLE_SKY_FRAMES 27
#define TITLE_FIRE_FRAMES 27
#define TITLE_TRANSITION_FRAMES 2
#define TITLE_ANIMATION_SPEED 8

#define INVENTORY_W 150
#define INVENTORY_H 300
#define INVENTORY_X -10
#define INVENTORY_Y 215

// Crest Inventory (right top corner)
#define CREST_INVENTORY_X 900
#define CREST_INVENTORY_Y 370
#define CREST_INVENTORY_W 80
#define CREST_INVENTORY_H 150
// Key inside crest inventory
#define CREST_KEY_OFFSET_X 20
#define CREST_KEY_OFFSET_Y 57
#define CREST_KEY_SIZE 35

#define PICKUP_SIZE 48
#define PICKUP_DROP_CHANCE 75
#define MAX_PICKUPS 10

#define POTION_DURATION 500
#define POTION_DAMAGE_MULTIPLIER 2

#define POWER_DAMAGE_PERCENT 20
#define GLOW_FRAMES 5
#define GLOW_SIZE 48
#define GLOW_SPEED 10

#define HEART_DURATION 500
#define HEART_REGEN_INTERVAL 150
#define HEART_REGEN_AMOUNT 20

#define GAIN_DURATION 600
#define GAIN_REGEN_INTERVAL 180
#define GAIN_REGEN_AMOUNT 20

// ============================================================
// TRADER NPC (Level 3)
// ============================================================
#define TRADER_NPC_X 2900
#define TRADER_NPC_Y LEVEL3_GROUND_Y+12
#define TRADER_IDLE_FRAMES 8
#define TRADER_WALK_L_FRAMES 9
#define TRADER_WALK_R_FRAMES 9
#define TRADER_TURN_FRAMES 2
#define TRADER_TRADE_FRAMES 4
#define TRADER_DETECTION_RADIUS 300
#define TRADER_STOP_RADIUS 70
#define TRADER_WALK_SPEED 3
#define TRADER_ANIM_SPEED 6
#define TRADER_SIZE 128

// ============================================================
// GRIM MASTER (Level 3 enemy)
// ============================================================
#define GRIM_MAX_HEALTH 250
#define GRIM_IDLE_FRAMES 10
#define GRIM_TURN_L_FRAMES 3
#define GRIM_TURN_R_FRAMES 3
#define GRIM_DASH_ANTIC_L_FRAMES 8
#define GRIM_DASH_ANTIC_R_FRAMES 8
#define GRIM_DASH_L_FRAMES 5
#define GRIM_DASH_R_FRAMES 5
#define GRIM_DEATH_FRAMES 3
#define GRIM_THROW_ANTIC_L_FRAMES 5
#define GRIM_THROW_ANTIC_R_FRAMES 5
#define GRIM_THROW_L_FRAMES 3
#define GRIM_THROW_R_FRAMES 3
#define GRIM_FIREBALL_FRAMES 8
#define GRIM_FIREBALL_EXPLODE_FRAMES 4
#define GRIM_TELEPORT_IN_PILLAR_FRAMES 6
#define GRIM_TELEPORT_IN_FRAMES 9
#define GRIM_TELEPORT_OUT_FRAMES 6
#define GRIM_TELEPORT_OUT_PILLAR_FRAMES 6
#define GRIM_SIZE 220
#define GRIM_SPEED 4
#define GRIM_DASH_SPEED 15
#define GRIM_DETECTION_RANGE 390
#define GRIM_DASH_RANGE 200
#define GRIM_ATTACK_RANGE 300
#define GRIM_FIREBALL_SPEED 12
#define GRIM_FIREBALL_SIZE 50
#define GRIM_FIREBALL_DISPLAY_SIZE 50
#define GRIM_FIREBALL_EXPLODE_DISPLAY_SIZE 100
#define GRIM_FIREBALL_HIT_W 120
#define GRIM_FIREBALL_HIT_H 120
#define GRIM_ANIM_SPEED 4
#define GRIM_TELEPORT_CHANCE 6
#define GRIM_THROW_CHANCE 6
#define GRIM_TELEPORT_RADIUS 250
#define GRIM_INVINCIBILITY_FRAMES 90
#define GRIM_DAMAGE_FRAMES 3
#define MAX_GRIMS 2
#define GRIM_SPAWN_1_X 1000
#define GRIM_SPAWN_2_X 3910
#define GRIM_SPAWN_Y_OFFSET 82
#define GRIM_FIREBALL_DAMAGE 15
#define GRIM_CONTACT_DAMAGE 10
#define MAX_GRIM_FIREBALLS 6

// Fireball spawn position offsets (relative to Grim's x,y)
#define GRIM_FB_SPAWN_OFFSET_X_RIGHT 100  // X offset when facing right (center-front of grim)
#define GRIM_FB_SPAWN_OFFSET_X_LEFT  100 // X offset when facing left (center-front of grim)
#define GRIM_FB_SPAWN_OFFSET_Y       130  // Y offset from grim's y (staff height)
// Fireball target offsets
#define GRIM_FB_TARGET_PLAYER_OFFSET_X 300 // offset to center of player sprite
#define GRIM_FB_TARGET_GROUND_OFFSET   30  // how far below ground level to target

// ============================================================
// TRADE ITEM SYSTEM
// ============================================================
#define TRADE_ITEM_COST_SWIFTNESS 0
#define TRADE_ITEM_COST_SOUL 0
#define TRADE_ITEM_COST_KEY 0

#define SWIFTNESS_SPEED_MULTIPLIER 1.5f
#define SOUL_REGEN_ON_KILL 20  // 1 heart = 20 HP (out of 100)

// Trade menu layout — list panel anchored to the right side of the screen
// The list background occupies the right portion of the screen
#define TRADE_LIST_X 180
#define TRADE_LIST_Y 50
#define TRADE_LIST_W 800
#define TRADE_LIST_H 480

// Fragment image and count display (on the list, upper area)
#define TRADE_FRAGMENT_X (TRADE_LIST_X)
#define TRADE_FRAGMENT_Y (TRADE_LIST_Y)
#define TRADE_FRAGMENT_W 800
#define TRADE_FRAGMENT_H 480
#define TRADE_FRAGMENT_TEXT_OFFSET_X 88   // text X offset from TRADE_FRAGMENT_X
#define TRADE_FRAGMENT_TEXT_OFFSET_Y 34   // text Y offset from TRADE_FRAGMENT_Y

#define TRADE_TOTAL_FRAGMENT_X 843
#define TRADE_TOTAL_FRAGMENT_Y 456

// Purchasable item image sizes
#define TRADE_ITEM_W 175
#define TRADE_ITEM_H 70

// Swiftness item position
#define TRADE_SWIFTNESS_X1 (TRADE_LIST_X + 150)
#define TRADE_SWIFTNESS_Y1 (TRADE_LIST_Y + 311)
#define TRADE_SWIFTNESS_X2 (TRADE_SWIFTNESS_X1 + TRADE_ITEM_W)
#define TRADE_SWIFTNESS_Y2 (TRADE_SWIFTNESS_Y1 + TRADE_ITEM_H)

// Soul item position
#define TRADE_SOUL_X1 (TRADE_LIST_X + 150)
#define TRADE_SOUL_Y1 (TRADE_LIST_Y + 209)
#define TRADE_SOUL_X2 (TRADE_SOUL_X1 + TRADE_ITEM_W)
#define TRADE_SOUL_Y2 (TRADE_SOUL_Y1 + TRADE_ITEM_H)

// Key item position
#define TRADE_KEY_X1 (TRADE_LIST_X + 150)
#define TRADE_KEY_Y1 (TRADE_LIST_Y + 105)
#define TRADE_KEY_X2 (TRADE_KEY_X1 + TRADE_ITEM_W)
#define TRADE_KEY_Y2 (TRADE_KEY_Y1 + TRADE_ITEM_H)

// Description popup size & offset (shown above the hovered item)
#define TRADE_DESC_X 180
#define TRADE_DESC_Y 50
#define TRADE_DESC_W 800
#define TRADE_DESC_H 480
#define TRADE_DESC_Y_OFFSET 0  // gap between item top and desc bottom

// Cost fragment icon (small fragment image shown below each item as cost)
#define TRADE_COST_ICON_W 0
#define TRADE_COST_ICON_H 0
#define TRADE_COST_ICON_Y_OFFSET (-35)  // below the item Y1
#define TRADE_COST_TEXT_X_OFFSET 34     // cost number text X offset from icon
#define TRADE_COST_TEXT_Y_OFFSET 8      // cost number text Y offset from icon

// Hover effect lift amount (like title buttons)
#define TRADE_HOVER_OFFSET 12

// Equipped item icons (top-right corner)
#define EQUIPPED_ICON_SIZE 40
#define EQUIPPED_ICON_X 910
#define EQUIPPED_ICON_Y 550
#define EQUIPPED_ICON_SPACING 50

// Door at end of Level 3
#define DOOR_LOCKED_FRAMES 3
#define DOOR_OPEN_FRAMES 6
#define DOOR_X 4850
#define DOOR_Y LEVEL3_GROUND_Y+50
#define DOOR_W 120
#define DOOR_H 180
#define DOOR_ANIM_SPEED 6

// ============================================================
// FINAL BOSS CONFIGURATION
// ============================================================

// Boss health & hit values
#define BOSS_MAX_HEALTH 100
#define BOSS_NORMAL_HIT_DAMAGE 5     // % per X attack hit
#define BOSS_SLASHWAVE_HIT_DAMAGE 7  // % per slashwave hit
#define BOSS_INVINCIBILITY_FRAMES 30 // frames after being hit

// Boss attack damage to player (in HP points, 20 = 1 heart)
#define BOSS_SLASH_PLAYER_DAMAGE 20
#define BOSS_DASH_PLAYER_DAMAGE 20
#define BOSS_SPIKE_PLAYER_DAMAGE 20

// Phase thresholds (% of max health)
#define BOSS_PHASE2_THRESHOLD 50 // Spike unlocks below this
#define BOSS_TRAP_THRESHOLD 25   // One-time Trap at this %

// Boss positioning & movement
#define BOSS_SPAWN_X                                                           \
  700 // screen-space (BOSS_STATE has static bg, no scrolling)
#define BOSS_RAND_TELEPORT_MIN 100 // leftmost random teleport x
#define BOSS_RAND_TELEPORT_MAX 700 // rightmost random teleport x
#define BOSS_SIZE_W 128
#define BOSS_SIZE_H 128
#define BOSS_WALK_SPEED 4
#define BOSS_DASH_SPEED 10
#define BOSS_DETECTION_RANGE 500 // Player proximity to activate
#define BOSS_SLASH_RANGE 120     // Range to trigger slash

// Boss timing (in animation ticks ~50fps)
#define BOSS_IDLE_DURATION 50     // ~2 seconds idle between actions
#define BOSS_POST_SPIKE_IDLE 50    // ~1 second after spikes
#define BOSS_DEATH_RISE_HEIGHT 120 // px to float up before death anim
#define BOSS_DEATH_RISE_SPEED 2    // px per frame rising
#define BOSS_ANIM_SPEED 6          // frames per animation tick

// Boss animation frame counts
#define BOSS_IDLE_FRAMES 4
#define BOSS_WALK_FRAMES 3
#define BOSS_SLASH_FRAMES 8
#define BOSS_DASH_FRAMES 6
#define BOSS_CAST_FRAMES 6
#define BOSS_TELEPORT_FRAMES 6
#define BOSS_SPIKE_FRAMES 10
#define BOSS_TRAPIN_FRAMES 3
#define BOSS_TRAPOUT_FRAMES 3
#define BOSS_DEATH_FRAMES 9

// FireBat stats & animation
#define FIREBAT_MAX_HEALTH 2 // hits to kill
#define FIREBAT_SPEED 4
#define FIREBAT_DAMAGE 10 // 0.5 heart
#define FIREBAT_FRAMES 5
#define FIREBAT_SIZE 64
#define FIREBAT_ANIM_SPEED 6
#define FIREBAT_INVINCIBILITY 20

// Bat stats & animation (spawns after boss death)
#define BAT_MAX_HEALTH 2 // hits to kill
#define BAT_SPEED 3
#define BAT_DAMAGE 10 // 0.5 heart
#define BAT_FRAMES 3
#define BAT_DEATH_FRAMES 2
#define BAT_SIZE 48
#define BAT_ANIM_SPEED 6
#define BAT_INVINCIBILITY 20

// Max entities
#define MAX_BOSS_MINIONS 4
#define MAX_BOSS_HAZARDS 4

#endif
