#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <deque>
using namespace std;

Color green = {173 , 204 , 96 , 255};
Color darkGreen = {43 , 51 , 24 , 255};
Color red = {230, 41, 55, 255};

int cellSize = 30;
int cellCount = 23;
int score = 0;

double lastUpdateTime = 0;

bool EventTriggered(double interval){
    double currentTime = GetTime();

    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }

    return false;
}

bool SnakeOnFood(deque<Vector2> body , Vector2 food){
    for(Vector2 ele : body){
        if(Vector2Equals(ele , food)) return true;
    }

    return false;
}

class Snake{
public:
    deque<Vector2> body = {Vector2{6 , 9} , Vector2{5 , 9} , Vector2{4 , 9}};
    Vector2 direction = {1 , 0};
    bool addSegment = false;

    void Draw(){
        for(long long unsigned int i = 0 ; i < body.size() ; i++){
            int x = body[i].x;
            int y = body[i].y;

            if(i == 0) DrawRectangle(x * cellSize , y * cellSize , cellSize , cellSize , DARKBLUE);
            else DrawRectangle(x * cellSize , y * cellSize , cellSize , cellSize , BLUE);
        }
    }

    void Update(){
        body.push_front(Vector2Add(body[0] , direction));

        if(addSegment) addSegment = false;
        else body.pop_back();
    }

    void Reset(){
        body = {Vector2{6 , 9} , Vector2{5 , 9} , Vector2{4 , 9}};
        direction = {1 , 0};
    }
};

class Food{
public:
    Vector2 position;

    Food(deque<Vector2> body){
        position = GenerateRandom(body);
    }

    void Draw(){
        DrawRectangle(position.x * cellSize , position.y * cellSize , cellSize , cellSize , red);
    }

    Vector2 RandomPos(){
        float x = GetRandomValue(0 , cellCount - 1);
        float y = GetRandomValue(0 , cellCount - 1);

        return Vector2{x , y};
    }

    Vector2 GenerateRandom(deque<Vector2> body){
        Vector2 random = RandomPos();

        while(SnakeOnFood(body , random)){
            random = RandomPos();
        }

        return random;
    }
};

class Game{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool run = true;
    Sound sukuna;
    Sound gojo;

    Game(){
        InitAudioDevice();
        gojo = LoadSound("D:/Documents/Snake Game/src/gojo.mp3");
        sukuna = LoadSound("D:/Documents/Snake Game/src/sukuna.mp3");
    }

    ~Game(){
        UnloadSound(gojo);
        UnloadSound(sukuna);
        CloseAudioDevice();
    }

    void Draw(){
        snake.Draw();
        food.Draw();
    }

    void Update(){
        if(run){
            snake.Update();
            CollisionWithFood();
            CollisionWithEdges();
            CollisionWithBody();
        }
    }

    void CollisionWithFood(){
        if(Vector2Equals(snake.body[0] , food.position)){
            snake.addSegment = true;
            food.position = food.GenerateRandom(snake.body);
            score++;
            PlaySound(gojo);
        }
    }

    void CollisionWithEdges(){
        Vector2 head = snake.body[0];

        if(head.x == cellCount || head.x < 0 || head.y == cellCount || head.y < 0) GameOver();
    }

    void CollisionWithBody(){
        deque<Vector2> body = snake.body;
        body.pop_front();

        while(!body.empty()){
            auto front = body.front();
            body.pop_front();

            if(Vector2Equals(snake.body[0] , front)){
                GameOver();
                return;
            }
        }
    }

    void GameOver(){
        snake.Reset();
        food.position = food.GenerateRandom(snake.body);
        score = 0;
        StopSound(gojo);
        PlaySound(sukuna);
        run = false;
    }
};

int main(){
    const int window_width = cellSize * cellCount;
    const int window_height = cellSize * cellCount;

    InitWindow(window_width , window_height , "Snake Game");
    SetTargetFPS(10);

    Game game = Game();

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);

        for(int i = 1 ; i <= 22 ; i++){
            DrawLine( i * cellSize , 0 , i * cellSize , window_height , LIGHTGRAY);
            DrawLine( 0 , i * cellSize , window_width , i * cellSize , LIGHTGRAY);
        }

        if(IsKeyPressed(KEY_UP) && !(game.snake.direction == Vector2{0 , 1})) game.snake.direction = {0 , -1};
        if(IsKeyPressed(KEY_DOWN) && !(game.snake.direction == Vector2{0 , -1})) game.snake.direction = {0 , 1};
        if(IsKeyPressed(KEY_LEFT) && !(game.snake.direction == Vector2{1 , 0})) game.snake.direction = {-1 , 0};
        if(IsKeyPressed(KEY_RIGHT) && !(game.snake.direction == Vector2{-1 , 0})) game.snake.direction = {1 , 0};
        if(IsKeyPressed(KEY_ENTER)) game.run = true;

        if(!game.run){
            DrawText("Press Enter to Continue" , window_width / 4 + 20 , 0 , 25 , BLUE);
        }

        game.Update();
        game.Draw();
        DrawText(TextFormat("%i" , score) , 10 , 0 , 25 , BLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}