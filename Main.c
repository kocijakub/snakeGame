#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PLAYER_WIDTH 60
#define PLAYER_HEIGHT 20
#define PLAYER_SPEED 6

#define ALIEN_SIZE 40
#define ALIEN_ROWS 4
#define ALIEN_COLS 10
#define ALIEN_SPEED 1

#define BULLET_WIDTH 6
#define BULLET_HEIGHT 12
#define BULLET_SPEED 10

typedef struct {
    float x, y;
    bool active;
} Bullet;

typedef struct {
    float x, y;
    bool alive;
} Alien;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Galaxian - Shooting Version");
    SetTargetFPS(60);

    float playerX = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
    float playerY = SCREEN_HEIGHT - PLAYER_HEIGHT - 10;

    Bullet bullet = {0};
    bullet.active = false;

    Alien aliens[ALIEN_ROWS * ALIEN_COLS] = {0};
    for (int row = 0; row < ALIEN_ROWS; row++) {
        for (int col = 0; col < ALIEN_COLS; col++) {
            aliens[row * ALIEN_COLS + col].x = col * (ALIEN_SIZE + 10) + 35;
            aliens[row * ALIEN_COLS + col].y = row * (ALIEN_SIZE + 10) + 50;
            aliens[row * ALIEN_COLS + col].alive = true;
        }
    }

    int score = 0;
    int frameCounter = 0;

    while (!WindowShouldClose()) {
        // Pohyb hráče WSAD + šipky
        if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && playerX > 0)
            playerX -= PLAYER_SPEED;
        if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && playerX < SCREEN_WIDTH - PLAYER_WIDTH)
            playerX += PLAYER_SPEED;

        // Střelba - SPACE, pokud není bullet aktivní
        if (IsKeyPressed(KEY_SPACE) && !bullet.active) {
            bullet.x = playerX + PLAYER_WIDTH / 2 - BULLET_WIDTH / 2;
            bullet.y = playerY;
            bullet.active = true;
        }

        // Pohyb bulletu nahoru
        if (bullet.active) {
            bullet.y -= BULLET_SPEED;
            if (bullet.y < 0) bullet.active = false;
        }

        // Pohyb alienů dolů
        for (int i = 0; i < ALIEN_ROWS * ALIEN_COLS; i++) {
            if (aliens[i].alive) {
                aliens[i].y += ALIEN_SPEED;
                // Pokud alien dorazí příliš dolů, můžeš přidat logiku game over atd.
                if (aliens[i].y > SCREEN_HEIGHT) aliens[i].y = 0; // jednoduchý reset nahoru
            }
        }

        // Kolize bulletu s alieny
        if (bullet.active) {
            for (int i = 0; i < ALIEN_ROWS * ALIEN_COLS; i++) {
                if (aliens[i].alive) {
                    Rectangle alienRec = {aliens[i].x, aliens[i].y, ALIEN_SIZE, ALIEN_SIZE};
                    Rectangle bulletRec = {bullet.x, bullet.y, BULLET_WIDTH, BULLET_HEIGHT};
                    if (CheckCollisionRecs(alienRec, bulletRec)) {
                        aliens[i].alive = false;
                        bullet.active = false;
                        score++;
                        break;
                    }
                }
            }
        }

        // Zvýšení frameCounter pro blikání
        frameCounter++;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Hráč
        DrawRectangleV((Vector2){playerX, playerY}, (Vector2){PLAYER_WIDTH, PLAYER_HEIGHT}, DARKBLUE);
        DrawRectangleLines(playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT, BLUE);

        // Aliens
        for (int i = 0; i < ALIEN_ROWS * ALIEN_COLS; i++) {
            if (aliens[i].alive) {
                DrawRectangle(aliens[i].x, aliens[i].y, ALIEN_SIZE, ALIEN_SIZE, GREEN);
                DrawRectangleLines(aliens[i].x, aliens[i].y, ALIEN_SIZE, ALIEN_SIZE, DARKGREEN);
            }
        }

        // Bullet bliká zlatě/oranžově podle frameCounter
        if (bullet.active) {
            Color bulletColor = (frameCounter % 20 < 10) ? GOLD : ORANGE;
            DrawRectangle(bullet.x, bullet.y, BULLET_WIDTH, BULLET_HEIGHT, bulletColor);
        }

        // Score
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
