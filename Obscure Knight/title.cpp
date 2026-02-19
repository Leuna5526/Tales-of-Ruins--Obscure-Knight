#include "title.h"
#include <stdlib.h>  // For exit()
#include <cstdio>    // For sprintf_s
#include "glut.h"    // For GLUT constants



#define TITLE_SCREEN_STATE 0
#define PLAYING_STATE 1
#define ACHIEVEMENTS_STATE 2
#define CREDITS_STATE 3

// Extern declarations for iGraphics functions
extern unsigned int iLoadImage(char filename[]);
extern void iShowImage(int x, int y, int width, int height, unsigned int texture);

// ---------------- TITLE SCREEN STATE CONSTANTS ----------------
#define TITLE_SKY_STATE_1 0          // First sky animation cycle
#define TITLE_SKY_STATE_2 1          // Second sky animation cycle (repeat)
#define TITLE_FIRE_TRANSITION_STATE 2
#define TITLE_FIRE_STATE_1 3         // First fire animation cycle
#define TITLE_FIRE_STATE_2 4         // Second fire animation cycle (repeat)
#define TITLE_SKY_TRANSITION_STATE 5

// ---------------- TITLE SCREEN IMPLEMENTATIONS ----------------

void initTitleScreen(struct TitleScreen* title) {
    title->bgFrame = 0;
    title->bgAnimationTimer = 0;
    title->bgState = TITLE_SKY_STATE_1;
    title->bgPhaseRepeat = 0;
    title->mouseX = 0;
    title->mouseY = 0;
}

void loadTitleTextures(struct TitleScreen* title) {
    char name[64];

    // Load sky background frames
    for (int i = 0; i < TITLE_SKY_FRAMES; i++) {
        sprintf_s(name, sizeof(name), "Assets/Title Screen/Background/Sky/sky (%d).png", i + 1);
        title->skyTextures[i] = iLoadImage(name);
    }

    // Load fire background frames
    for (int i = 0; i < TITLE_FIRE_FRAMES; i++) {
        sprintf_s(name, sizeof(name), "Assets/Title Screen/Background/Fire/fire (%d).png", i + 1);
        title->fireTextures[i] = iLoadImage(name);
    }

    // Load transition frames
    for (int i = 0; i < TITLE_TRANSITION_FRAMES; i++) {
        sprintf_s(name, sizeof(name), "Assets/Title Screen/Background/FireTransition/trans (%d).png", i + 1);
        title->fireTransTextures[i] = iLoadImage(name);

        sprintf_s(name, sizeof(name), "Assets/Title Screen/Background/SkyTransition/trans (%d).png", i + 1);
        title->skyTransTextures[i] = iLoadImage(name);
    }

    // Load UI elements
    title->titleTexture = iLoadImage("Assets/Title Screen/Title/title.png");
    title->startTexture = iLoadImage("Assets/Title Screen/Start/start.png");
    title->achievementsTexture = iLoadImage("Assets/Title Screen/Achievements/achievements.png");
    title->creditsTexture = iLoadImage("Assets/Title Screen/Credits/credits.png");
    title->exitTexture = iLoadImage("Assets/Title Screen/Exit/exit.png");
    title->cursorTexture = iLoadImage("Assets/Title Screen/Cursor/cursor.png");
}

void updateTitleAnimation(struct TitleScreen* title) {
    title->bgAnimationTimer++;

    if (title->bgAnimationTimer >= TITLE_ANIMATION_SPEED) {
        title->bgAnimationTimer = 0;
        title->bgFrame++;

        // Handle state transitions with phase repeats
        switch (title->bgState) {
            case TITLE_SKY_STATE_1:
                if (title->bgFrame >= TITLE_SKY_FRAMES) {
                    title->bgFrame = 0;
                    title->bgState = TITLE_SKY_STATE_2;  // Repeat sky animation once
                }
                break;

            case TITLE_SKY_STATE_2:
                if (title->bgFrame >= TITLE_SKY_FRAMES) {
                    title->bgFrame = 0;
                    title->bgState = TITLE_FIRE_TRANSITION_STATE;
                }
                break;

            case TITLE_FIRE_TRANSITION_STATE:
                if (title->bgFrame >= TITLE_TRANSITION_FRAMES) {
                    title->bgFrame = 0;
                    title->bgState = TITLE_FIRE_STATE_1;
                }
                break;

            case TITLE_FIRE_STATE_1:
                if (title->bgFrame >= TITLE_FIRE_FRAMES) {
                    title->bgFrame = 0;
                    title->bgState = TITLE_FIRE_STATE_2;  // Repeat fire animation once
                }
                break;

            case TITLE_FIRE_STATE_2:
                if (title->bgFrame >= TITLE_FIRE_FRAMES) {
                    title->bgFrame = 0;
                    title->bgState = TITLE_SKY_TRANSITION_STATE;
                }
                break;

            case TITLE_SKY_TRANSITION_STATE:
                if (title->bgFrame >= TITLE_TRANSITION_FRAMES) {
                    title->bgFrame = 0;
                    title->bgState = TITLE_SKY_STATE_1;  // Back to start
                }
                break;
        }
    }
}

void renderTitleScreen(struct TitleScreen* title) {
    unsigned int currentBgTexture = 0;

    // Select current background texture based on state
    switch (title->bgState) {
        case TITLE_SKY_STATE_1:
        case TITLE_SKY_STATE_2:
            currentBgTexture = title->skyTextures[title->bgFrame];
            break;
        case TITLE_FIRE_TRANSITION_STATE:
            currentBgTexture = title->fireTransTextures[title->bgFrame];
            break;
        case TITLE_FIRE_STATE_1:
        case TITLE_FIRE_STATE_2:
            currentBgTexture = title->fireTextures[title->bgFrame];
            break;
        case TITLE_SKY_TRANSITION_STATE:
            currentBgTexture = title->skyTransTextures[title->bgFrame];
            break;
    }

    // Draw background first
    if (currentBgTexture != 0) {
        iShowImage(0, 0, SCREEN_W, SCREEN_H, currentBgTexture);
    }

    // Draw title logo on top with shadow effect for better visibility
    if (title->titleTexture != 0) {
        // Draw shadow effect by drawing title multiple times with slight offsets
        // This creates a subtle border around the white text
        iShowImage(1, 1, SCREEN_W, SCREEN_H, title->titleTexture);   // Shadow
        iShowImage(-1, -1, SCREEN_W, SCREEN_H, title->titleTexture); // Shadow
        iShowImage(1, -1, SCREEN_W, SCREEN_H, title->titleTexture);  // Shadow
        iShowImage(-1, 1, SCREEN_W, SCREEN_H, title->titleTexture);  // Shadow
        iShowImage(0, 0, SCREEN_W, SCREEN_H, title->titleTexture);   // Main title
    }

    // Draw buttons (all buttons are displayed but only start and exit work)
    if (title->startTexture != 0) {
        iShowImage(0, 0, SCREEN_W, SCREEN_H, title->startTexture);
    }
    if (title->achievementsTexture != 0) {
        iShowImage(0, 0, SCREEN_W, SCREEN_H, title->achievementsTexture);
    }
    if (title->creditsTexture != 0) {
        iShowImage(0, 0, SCREEN_W, SCREEN_H, title->creditsTexture);
    }
    if (title->exitTexture != 0) {
        iShowImage(0, 0, SCREEN_W, SCREEN_H, title->exitTexture);
    }

    // Draw custom cursor
    if (title->cursorTexture != 0) {
        iShowImage(title->mouseX, title->mouseY - 32, 32, 32, title->cursorTexture);
    }
}

void handleTitleMouseMove(struct TitleScreen* title, int mx, int my) {
    title->mouseX = mx;
    title->mouseY = my;
}

void handleTitleMouseClick(struct TitleScreen* title, int button, int state, int mx, int my, int* gameState) {
    // Only handle left mouse button clicks
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

    // Button detection - adjust these coordinates to match your actual button image positions
    // The areas below are estimates - you may need to fine-tune them

    // Start button - center area where start button image is located
    // Adjust these coordinates to match your start button's visual position
    if (mx >= 400 && mx <= 600 && my >= 200 && my <= 280) {
        // Start the game
        *gameState = PLAYING_STATE;
    }

    // Exit button - bottom area where exit button image is located
    // Adjust these coordinates to match your exit button's visual position
    if (mx >= 400 && mx <= 600 && my >= 40 && my <= 100) {
        exit(0); // Exit the program
    }

    // Other buttons (achievements, credits) are not functional yet
    // Uncomment and adjust coordinates when you implement these features

    // Achievement button - adjust to match achievement button position
    if (mx >= 400 && mx <= 600 && my >= 140 && my <= 200) {
		*gameState = ACHIEVEMENTS_STATE;
    //     // Achievement functionality - implement later
    }

    // Credits button - adjust to match credits button position
    if (mx >= 400 && mx <= 600 && my >= 80 && my <= 140) {
		*gameState = CREDITS_STATE;
    //     // Credits functionality - implement later
    }

    // Title area - NO CLICK DETECTION (title is just a decorative image)
    // The title should not respond to clicks
}