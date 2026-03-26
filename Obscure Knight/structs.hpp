#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include "config.hpp"

struct Player;
struct Camera;
struct Midground;
struct Background;
struct Creature;
struct TitleScreen;

enum State {
  IDLE,
  WALK,
  TURNING,
  JUMP,
  FALL,
  LANDING,

  DASHING,

  ATTACK_OVERHEAD_RECOVER,
  ATTACK_OVERHEAD_SLASHING,
  ATTACK_OVERHEAD_SLASHWAVE,

  EVADE_PRE,
  EVADE_ACTIVE,
  EVADE_RECOVER,

  DOWNSTAB_PRE,
  DOWNSTAB_ACTIVE,
  DOWNSTAB_LAND,

  DEATH = 40,

  WAKE_PART1 = 50,
  WAKE_PART2,
  WAKE_PART3,
  ROAR = 53,
  ROAR_START = 54,
  ROAR_ACTIVE = 55,
  ROAR_END = 56
};

enum CreatureState {
  CREATURE_INACTIVE,
  CREATURE_LOADING,
  CREATURE_RISING,
  CREATURE_PATROL_LEFT,
  CREATURE_PATROL_RIGHT,
  CREATURE_TURNING,
  CREATURE_ATTACKING,
  CREATURE_HOVERING,
  CREATURE_DAMAGE,
  CREATURE_DEAD
};

struct Player {
  int x, y, vy;
  int frame;
  int onGround;
  enum State state;
  int health;
  int maxHealth;
  int stamina;
  int maxStamina;
  int staminaRegenTimer;
  int deaths;
  int gameOverFrame;
  int gameOverTimer;
  int invincibilityTimer;

  int isAttacking;
  int isTrapped;

  int facingRight;
  int stateTimer;

  int hasHeart;
  int hasPotion;
  int hasPower;
  int potionTimer;
  int damageMultiplier;
  int heartDuration;
  int heartRegenTimer;
  int hasGain;
  int gainTimer;
  int gainRegenTimer;
  int hasKey;
  int hasUsedKey;

  // Trade/Fragment system
  int fragments;
  int hasSwiftness;
  int hasSoul;
  int hasKeyItem;
  int swiftnessUsed;   
  int soulUsed;        
  int keyUsed;        
  int swiftnessActive; 
  int soulActive;      
  float speedMultiplier;

  int hasPlayedBossEntry;
  int bossEntryFrame;
  int bossEntryTimer;
  int gameEndingFrame;
  int gameEndingTimer;
};

struct Camera {
  float x, y;
  float targetX, targetY;
};

struct Background {
  int x;
  unsigned int texture;
  int tunnelTransitionY;
};

struct Tile {
  int x, y;
  unsigned int texture;
  int active;
  float width, height;
  int isJumpThrough;
};

struct Midground {
  struct Tile tiles[MAX_TILES];
  int tileCount;
  unsigned int tileTexture1;
  unsigned int tileTexture2;
  unsigned int tunnelHoleTexture;
};

struct Creature {
  int x, y;
  int vx, vy;
  int frame;
  int active;
  enum CreatureState state;
  int targetX, targetY;
  int patrolStartX;
  int subStateTimer;
  int animationTimer;
  int facingRight;
  int maxHealth;
  int currentHealth;
  int invincibilityTimer;
  int damageAnimTimer;
  int damageFrame;
};

enum SentryState {
  SENTRY_INACTIVE,
  SENTRY_WAKING,
  SENTRY_IDLE_STATE,
  SENTRY_WALK_LEFT,
  SENTRY_WALK_RIGHT,
  SENTRY_RUN_LEFT,
  SENTRY_RUN_RIGHT,
  SENTRY_ATTACK_LEFT,
  SENTRY_ATTACK_RIGHT,
  SENTRY_JUMP_ATTACK,
  SENTRY_SLASH_LEFT,
  SENTRY_SLASH_RIGHT,
  SENTRY_TURNING_STATE,
  SENTRY_DAMAGE,
  SENTRY_DYING,
  SENTRY_DYING_AIR
};

struct Sentry {
  int x, y;
  int vx, vy;
  int frame;
  int active;
  enum SentryState state;
  int patrolStartX;
  int subStateTimer;
  int animationTimer;
  int facingRight;
  int maxHealth;
  int currentHealth;
  int invincibilityTimer;
  int damageAnimTimer;
  int damageFrame;
};

struct SparkleEffect {
  int x, y;
  int frame;
  int active;
  int animationTimer;
};

struct TitleScreen {
  int bgFrame;
  int bgAnimationTimer;
  int bgState;
  int bgPhaseRepeat;
  unsigned int skyTextures[TITLE_SKY_FRAMES];
  unsigned int fireTextures[TITLE_FIRE_FRAMES];
  unsigned int fireTransTextures[TITLE_TRANSITION_FRAMES];
  unsigned int skyTransTextures[TITLE_TRANSITION_FRAMES];
  unsigned int titleTexture;
  unsigned int startTexture;
  unsigned int achievementsTexture;
  unsigned int creditsTexture;
  unsigned int exitTexture;
  unsigned int cursorTexture;
  unsigned int creditsBgTexture;
  unsigned int backButtonTexture;
  unsigned int controlsBgTexture;
  unsigned int loadingTexture;
  int loadingTimer;
  int mouseX, mouseY;
};

enum ItemType { ITEM_HEART, ITEM_POTION, ITEM_POWER, ITEM_GAIN, ITEM_KEY };

struct Pickup {
  int x, y;
  int active;
  enum ItemType type;
};

struct GlowProjectile {
  int x, y;
  int targetX, targetY;
  int active;
  int frame;
  int animTimer;
  int targetCreatureIdx;
  int targetSentryIdx;
};

enum NPCState {
  NPC_STILL,
  NPC_APPROACHING,
  NPC_IDLE_LEFT,
  NPC_IDLE_RIGHT,
  NPC_TURNING,
  NPC_TALKING,
  NPC_SUMMON_START,
  NPC_SUMMONING,
  NPC_RETREAT,
  NPC_DONE
};

struct NPC {
  int x, y;
  int frame;
  enum NPCState state;
  int stateTimer;
  int dialogueIndex;
  int showPrompt;
  int isInteracting;
  int initialized;
};

enum BossState {
  BOSS_IDLE_STATE,
  BOSS_WALK_STATE,
  BOSS_SLASH_STATE,
  BOSS_DASH_STATE,
  BOSS_CAST_STATE,
  BOSS_TELEPORT_OUT_STATE,
  BOSS_TELEPORT_IN_STATE,
  BOSS_SPIKE_STATE,
  BOSS_TRAP_STATE,
  BOSS_DEATH_RISE_STATE,
  BOSS_DEATH_ANIM_STATE,
  BOSS_WAIT_MINION_STATE 
};

struct Boss {
  int x, y;
  int frame;
  int animTimer;
  int active;
  int facingRight;
  enum BossState state;
  int stateTimer;
  int currentHealth;
  int maxHealth;
  int hasUsedTrap;
  int teleportTargetX;
  int phase; 
  int riseY; 
  int invincibilityTimer;
  enum BossState intendedNextState;
};

enum MinionType { MINION_FIREBAT, MINION_BAT };

struct BossMinion {
  int x, y;
  int active;
  int frame;
  int animTimer;
  int facingRight;
  int health;
  int invincibilityTimer;
  int isDying; 
  int deathTimer;
  enum MinionType type;
};

enum HazardType { HAZARD_SPIKE, HAZARD_TRAP };

struct BossHazard {
  int x, y;
  int active;
  int frame;
  int animTimer;
  int hasHit; 
  enum HazardType type;
};

enum TraderState {
  TRADER_IDLE_STATE,
  TRADER_WALK_TO_PLAYER,
  TRADER_TURN_STATE,
  TRADER_PROMPT_INTERACT,
  TRADER_SHOW_KEY,
  TRADER_TRADING,
  TRADER_TRADE_MENU,
  TRADER_WALK_AWAY,
  TRADER_WALK_BACK,
  TRADER_DONE
};

struct TraderNPC {
  int x, y;
  int frame;
  int animTimer;
  enum TraderState state;
  int stateTimer;
  int facingRight;
  int active;
  int traded;
  int initialX;
  int tradeMenuOpen;
  int mouseX, mouseY;
  int hoveredItem;    
  int tradedSwiftness;
  int tradedSoul;
  int tradedKey;
};

struct BossDoor {
  int x, y;
  int frame;
  int animTimer;
  int locked;       
  int opening;      
  int opened;      
};

enum GrimState {
  GRIM_INACTIVE,
  GRIM_IDLE_STATE,
  GRIM_TURN_STATE,
  GRIM_DASH_ANTICIPATE,
  GRIM_DASHING,
  GRIM_THROW_ANTICIPATE,
  GRIM_THROWING,
  GRIM_TELEPORT_OUT_STATE,
  GRIM_TELEPORT_OUT_PILLAR,
  GRIM_TELEPORT_IN_PILLAR,
  GRIM_TELEPORT_IN_STATE,
  GRIM_DYING,
  GRIM_RETURNING,
  GRIM_DEAD
};

struct GrimMaster {
  int x, y;
  int frame;
  int animTimer;
  int active;
  int facingRight;
  enum GrimState state;
  int stateTimer;
  int subStateTimer;
  int currentHealth;
  int maxHealth;
  int invincibilityTimer;
  int damageAnimTimer;
  int damageFrame;
  int teleportTargetX;
  int spawnX, spawnY;
};

struct GrimFireball {
  float x, y;
  int targetX, targetY;
  float vx, vy;
  int active;
  int frame;
  int animTimer;
  int exploding;
  int explodeFrame;
  int explodeTimer;
};

enum GreetState {
  GREET_IDLE,
  GREET_ANIMATING,
  GREET_DONE
};

struct GreetNPC {
  int x, y;
  int frame;
  int animTimer;
  enum GreetState state;
  int active;   
  int done;     
};

#endif
