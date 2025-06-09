#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PLAYER_WIDTH 60
#define PLAYER_HEIGHT 20
#define PLAYER_SPEED 6

#define ALIEN_SIZE 40
#define MAX_ALIENS 30

#define BULLET_WIDTH 6
#define BULLET_HEIGHT 12
#define BULLET_SPEED 10
#define MAX_BULLETS 20

#define MAX_ALIEN_BULLETS 20
#define ALIEN_BULLET_SPEED 4

#define MAX_LIVES 3

const Color ALIEN_COLORS[] = {RED, ORANGE, PURPLE, GREEN, BLUE, MAROON, VIOLET};
#define NUM_ALIEN_COLORS (sizeof(ALIEN_COLORS)/sizeof(ALIEN_COLORS[0]))

typedef struct {
    float x, y;
    bool active;
} Bullet;

typedef struct {
    float x, y;
    bool alive;
    Color color;
} Alien;

void SpawnAlien(Alien aliens[], int maxAliens) {
    for (int i = 0; i < maxAliens; i++) {
        if (!aliens[i].alive) {
            aliens[i].x = rand() % (SCREEN_WIDTH - ALIEN_SIZE);
            aliens[i].y = -ALIEN_SIZE;
            aliens[i].alive = true;
            aliens[i].color = ALIEN_COLORS[rand() % NUM_ALIEN_COLORS];
            break;
        }
    }
}

void DrawPlayer(float x, float y) {
    DrawRectangle(x + 20, y, 20, 10, DARKBLUE);
    DrawRectangle(x, y + 10, 60, 10, BLUE);
    DrawRectangle(x + 25, y - 10, 10, 10, DARKBLUE);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Galaxian - Menu Version");
    SetTargetFPS(60);
    srand(time(NULL));

    enum GameState { MENU, GAMEPLAY, GAMEOVER } state = MENU;

    float playerX = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
    float playerY = SCREEN_HEIGHT - PLAYER_HEIGHT - 10;

    Bullet bullets[MAX_BULLETS] = {0};
    Bullet alienBullets[MAX_ALIEN_BULLETS] = {0};
    Alien aliens[MAX_ALIENS] = {0};

    int score = 0;
    int alienSpawnTimer = 0;
    int alienShootTimer = 0;
    int lives = MAX_LIVES;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (state) {
            case MENU:
                DrawText("GALAXIAN", SCREEN_WIDTH/2 - 100, 150, 40, DARKBLUE);
                DrawText("Press ENTER to Start", SCREEN_WIDTH/2 - 120, 300, 20, BLACK);
                if (IsKeyPressed(KEY_ENTER)) {
                    state = GAMEPLAY;
                    score = 0;
                    lives = MAX_LIVES;
                    for (int i = 0; i < MAX_ALIENS; i++) aliens[i].alive = false;
                    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
                    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) alienBullets[i].active = false;
                }
                break;

            case GAMEPLAY:
                if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && playerX > 0)
                    playerX -= PLAYER_SPEED;
                if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && playerX < SCREEN_WIDTH - PLAYER_WIDTH)
                    playerX += PLAYER_SPEED;

                if (IsKeyPressed(KEY_SPACE)) {
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullets[i].active) {
                            bullets[i].x = playerX + PLAYER_WIDTH / 2 - BULLET_WIDTH / 2;
                            bullets[i].y = playerY;
                            bullets[i].active = true;
                            break;
                        }
                    }
                }

                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (bullets[i].active) {
                        bullets[i].y -= BULLET_SPEED;
                        if (bullets[i].y < 0) bullets[i].active = false;
                    }
                }

                alienSpawnTimer++;
                if (alienSpawnTimer > 60) {
                    SpawnAlien(aliens, MAX_ALIENS);
                    alienSpawnTimer = 0;
                }

                alienShootTimer++;
                if (alienShootTimer > 90) {
                    for (int i = 0; i < MAX_ALIENS; i++) {
                        if (aliens[i].alive && rand() % 5 == 0) {
                            for (int j = 0; j < MAX_ALIEN_BULLETS; j++) {
                                if (!alienBullets[j].active) {
                                    alienBullets[j].x = aliens[i].x + ALIEN_SIZE / 2;
                                    alienBullets[j].y = aliens[i].y + ALIEN_SIZE;
                                    alienBullets[j].active = true;
                                    break;
                                }
                            }
                        }
                    }
                    alienShootTimer = 0;
                }

                for (int i = 0; i < MAX_ALIENS; i++) {
                    if (aliens[i].alive) {
                        aliens[i].y += 1;
                        if (aliens[i].y > SCREEN_HEIGHT) {
                            lives--;
                            aliens[i].alive = false;
                            if (lives <= 0) state = GAMEOVER;
                        }
                    }
                }

                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (bullets[i].active) {
                        for (int j = 0; j < MAX_ALIENS; j++) {
                            if (aliens[j].alive) {
                                Rectangle alienRec = {aliens[j].x, aliens[j].y, ALIEN_SIZE, ALIEN_SIZE};
                                Rectangle bulletRec = {bullets[i].x, bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT};
                                if (CheckCollisionRecs(alienRec, bulletRec)) {
                                    aliens[j].alive = false;
                                    bullets[i].active = false;
                                    score++;
                                    break;
                                }
                            }
                        }
                    }
                }

                for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
                    if (alienBullets[i].active) {
                        alienBullets[i].y += ALIEN_BULLET_SPEED;
                        if (alienBullets[i].y > SCREEN_HEIGHT) alienBullets[i].active = false;
                        Rectangle bulletRec = {alienBullets[i].x, alienBullets[i].y, BULLET_WIDTH, BULLET_HEIGHT};
                        Rectangle playerRec = {playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT};
                        if (CheckCollisionRecs(bulletRec, playerRec)) {
                            alienBullets[i].active = false;
                            lives--;
                            if (lives <= 0) state = GAMEOVER;
                        }
                    }
                }

                DrawPlayer(playerX, playerY);
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (bullets[i].active) {
                        DrawRectangle(bullets[i].x, bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT, RED);
                    }
                }
                for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
                    if (alienBullets[i].active) {
                        DrawRectangle(alienBullets[i].x, alienBullets[i].y, BULLET_WIDTH, BULLET_HEIGHT, BLACK);
                    }
                }
                for (int i = 0; i < MAX_ALIENS; i++) {
                    if (aliens[i].alive) {
                        DrawRectangle(aliens[i].x, aliens[i].y, ALIEN_SIZE, ALIEN_SIZE, aliens[i].color);
                    }
                }
                DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
                DrawText(TextFormat("Lives: %d", lives), SCREEN_WIDTH - 120, 10, 20, DARKGRAY);
                break;

            case GAMEOVER:
                DrawText("GAME OVER", SCREEN_WIDTH / 2 - 100, 200, 40, RED);
                DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH / 2 - 110, 270, 20, BLACK);
                DrawText("Press ENTER to return to Menu", SCREEN_WIDTH / 2 - 160, 350, 20, DARKGRAY);
                if (IsKeyPressed(KEY_ENTER)) {
                    state = MENU;
                }
                break;
        }

        EndDrawing();
    }


    CloseWindow();
    return 0;
}
