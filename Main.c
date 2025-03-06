#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 600
#define SQUARE_SIZE 20
#define MAX_LENGTH 1000

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SNAKE");
    SetTargetFPS(10);

    int snakeX[MAX_LENGTH] = {(SCREEN_WIDTH / (SQUARE_SIZE * 2)) * SQUARE_SIZE};
    int snakeY[MAX_LENGTH] = {(SCREEN_HEIGHT / (SQUARE_SIZE * 2)) * SQUARE_SIZE};
    int snakeLength = 1;
    int speed = SQUARE_SIZE;
    int direction = 0;
    int score = 0;
    bool gameOver = false;

    srand(time(NULL));
    int foodX = (rand() % (SCREEN_WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
    int foodY = (rand() % (SCREEN_HEIGHT / SQUARE_SIZE)) * SQUARE_SIZE;
    printf("Food Spawned: (%d, %d)\n", foodX, foodY);

    while (!WindowShouldClose()) {
        if (!gameOver) {
            if (IsKeyDown(KEY_RIGHT) && direction != 1) direction = 0;
            if (IsKeyDown(KEY_LEFT) && direction != 0) direction = 1;
            if (IsKeyDown(KEY_UP) && direction != 3) direction = 2;
            if (IsKeyDown(KEY_DOWN) && direction != 2) direction = 3;

            for (int i = snakeLength; i > 0; i--) {
                snakeX[i] = snakeX[i - 1];
                snakeY[i] = snakeY[i - 1];
            }

            if (direction == 0) snakeX[0] += speed;
            if (direction == 1) snakeX[0] -= speed;
            if (direction == 2) snakeY[0] -= speed;
            if (direction == 3) snakeY[0] += speed;

            if (snakeX[0] < 0 || snakeX[0] >= SCREEN_WIDTH || snakeY[0] < 0 || snakeY[0] >= SCREEN_HEIGHT) {
                gameOver = true;
            }

            for (int i = 1; i < snakeLength; i++) {
                if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
                    gameOver = true;
                }
            }

            if (snakeX[0] == foodX && snakeY[0] == foodY) {
                if (snakeLength < MAX_LENGTH) snakeLength++;
                foodX = (rand() % (SCREEN_WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
                foodY = (rand() % (SCREEN_HEIGHT / SQUARE_SIZE)) * SQUARE_SIZE;
                printf("Food Spawned: (%d, %d)\n", foodX, foodY);
                printf("EATEN!\n");
                score++;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (gameOver) {
            DrawText("Game Over! Press ESC to Exit", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 20, RED);
        } else {
            for (int i = 0; i < snakeLength; i++) {
                if (i == 0) {
                    DrawRectangle(snakeX[i], snakeY[i], SQUARE_SIZE, SQUARE_SIZE, DARKGREEN);
                } else {
                    DrawRectangle(snakeX[i], snakeY[i], SQUARE_SIZE, SQUARE_SIZE, GREEN);
                }
            }
            DrawRectangle(foodX, foodY, SQUARE_SIZE, SQUARE_SIZE, RED);
            DrawText(TextFormat("score: %i", score), 5, 5, 20, WHITE);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}