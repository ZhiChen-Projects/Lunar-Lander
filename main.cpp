#include "CS3113/Entity.h"
/**
* Author: [Zhi Chen]
* Assignment: Lunar Lander
* Date due: [3/14/2026]
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr char    BG_COLOUR[] = "#e2a471";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr int   NUMBER_OF_TILES         = 20,
                NUMBER_OF_BLOCKS        = 2;
constexpr float TILE_DIMENSION          = 50.0f,
                ACCELERATION_OF_GRAVITY = 981.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f,
                END_GAME_THRESHOLD      = 800.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Entity* gPlayer;
Entity* gTiles;
Entity* gBlocks;
float gFuel = 50;
bool gGameEnded = false;
std::string gEndText = "";


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}

void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander");
    SetTargetFPS(FPS);

    gTiles = new Entity[NUMBER_OF_TILES];
    gBlocks = new Entity[NUMBER_OF_BLOCKS];

    gPlayer = new Entity({ 50.0f, 40.0f }, { 30, 30 }, "assets/blocks.png", PLAYER); 
    gPlayer->setSpriteIndex(2); // Start with Blue
    
    gPlayer->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    //Initalize the platforms
    if (gBlocks != nullptr) {
        gBlocks[1] = Entity({ 50.0f, 70.0f }, { 60, 10 }, WHITE, START_PAD);
        gBlocks[0] = Entity({ 400.0f, 200.0f }, { 30, 20 }, YELLOW, PLATFORM);
        gBlocks[0].setAIType(LERPER);
    }

    // Mountains
    float heights[NUMBER_OF_TILES] = {
        550, 500, 450, 400, 350, 300, 250, 300, 350, 400,
        500, 550, 550, 500, 450, 400, 350, 300, 250, 200 
    };

    for (int i = 0; i < NUMBER_OF_TILES - 1; i++) {
        float height = SCREEN_HEIGHT - heights[i]; 
        gTiles[i] = Entity(
            {(float)i * TILE_DIMENSION + 25, heights[i] + (height / 2.0f)}, 
            {TILE_DIMENSION, height}, 
            BROWN, 
            BLOCK
        );
    }
    //Winning Platform as well
    gTiles[NUMBER_OF_TILES - 1] = Entity({ 975.0f, 580.0f }, { 30, 20 }, GREEN, PLATFORM);
}

void processInput() {
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    //Reset
    if (IsKeyPressed(KEY_R)) {
        if (gPlayer != nullptr) {
            gPlayer->setPosition({ 50.0f, 40.0f });
            gPlayer->setVelocity({ 0.0f, 0.0f });
            gPlayer->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
        }
        gGameEnded = false;
        gFuel = 50.0f;
        gEndText = "";
        if (gPlayer != nullptr) {
            gPlayer->setPosition({ 50.0f, 40.0f });
            gPlayer->setVelocity({ 0.0f, 0.0f });
            gPlayer->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
            gPlayer->setSpriteIndex(2); // reset the face
        }

        if (gBlocks != nullptr) {
            //WHITE = STARTING PLATFORM, YELLOW = MOVING PLATFORM 
            gBlocks[1] = Entity({ 50.0f, 70.0f }, { 60, 10 }, WHITE, START_PAD);
            gBlocks[0] = Entity({ 400.0f, 200.0f }, { 30, 20 }, YELLOW, PLATFORM);
            gBlocks[0].setAIType(LERPER);   
        }
        //GREEN IS ANOTHER WINNING PLATFORM
        gTiles[NUMBER_OF_TILES - 1] = Entity({ 975.0f, 580.0f }, { 30, 20 }, GREEN, PLATFORM);


        if (gBlocks != nullptr) {
            gBlocks[0] = Entity({ 400.0f, 200.0f }, { 30, 20 }, YELLOW, PLATFORM);
            gBlocks[0].setAIType(LERPER);
        }
        float heights[NUMBER_OF_TILES] = {
            550, 500, 450, 400, 350, 300, 250, 300, 350, 400,
            500, 550, 550, 500, 450, 400, 350, 300, 250, 200 
        };

        for (int i = 0; i < NUMBER_OF_TILES - 1; i++) {
            float height = SCREEN_HEIGHT - heights[i]; 
            gTiles[i] = Entity(
                {(float)i * TILE_DIMENSION + 25, heights[i] + (height / 2.0f)}, 
                {TILE_DIMENSION, height}, 
                BROWN, 
                BLOCK
            );
        }
        
        gTiles[NUMBER_OF_TILES - 1] = Entity({ 975.0f, 580.0f }, { 30, 20 }, GREEN, PLATFORM);

        if (gBlocks != nullptr) {
            gBlocks[0] = Entity({400.0f, 200.0f}, {30, 20}, YELLOW, PLATFORM);
            gBlocks[0].setAIType(LERPER);
        }
            
         return; 
    }

    if (gGameEnded) return;
    Vector2 currentAcc = { 0.0f, ACCELERATION_OF_GRAVITY }; 

    //THe Thrust
    float hThrust = 400.0f;  
    float vThrust = 1600.0f; 
    //Velocity
    if (gFuel > 0) {
        bool isThrusting = false;

        if (IsKeyDown(KEY_LEFT)) {
            currentAcc.x = -hThrust;
            isThrusting = true;
        } else if (IsKeyDown(KEY_RIGHT)) {
            currentAcc.x = hThrust;
            isThrusting = true;
        }

        if (IsKeyDown(KEY_UP)) {
        currentAcc.y = ACCELERATION_OF_GRAVITY - vThrust; 
        isThrusting = true;
        gPlayer->setSpriteIndex(3);// Purple
        } else {
            gPlayer->setSpriteIndex(2); // Blue
        }
        if (isThrusting) {
            gFuel -= 10.0f * GetFrameTime(); 
        }
    }
    
    gPlayer->setAcceleration(currentAcc);
}
void update() {
    if (gGameEnded || gPlayer == nullptr) return;

    float deltaTime = GetFrameTime();

    if (gBlocks != nullptr) {
        for (int i = 0; i < NUMBER_OF_BLOCKS; i++) {
            gBlocks[i].aiUpdate(deltaTime);
        }
    }
    gPlayer->update(deltaTime, gTiles, NUMBER_OF_TILES, gBlocks, NUMBER_OF_BLOCKS); 

    //Doesn't let the block to go outside of screen
    Vector2 pos = gPlayer->getPosition();
    if (pos.x < 0 || pos.x > SCREEN_WIDTH) {
        gGameEnded = true;
        gEndText = "MISSION FAILED: LOST?";
        gPlayer->setSpriteIndex(1); // GREEN SAD FACE
        return;
    }
    //Checks the winning platform
   for (int i = 0; i < NUMBER_OF_BLOCKS; i++) {
        if (gPlayer->isColliding(&gBlocks[i])) { 
            if (gBlocks[i].getEntityType() == START_PAD) continue; 

            gGameEnded = true;
            if (gBlocks[i].getEntityType() == PLATFORM && gPlayer->isCollidingBottom()) {
                // USE IMPACT VELOCITY HERE
                if (gPlayer->getImpactVelocity().y < 350.0f) {
                    gEndText = "MISSION ACCOMPLISHED!";
                    gPlayer->setSpriteIndex(0); 
                } else {
                    gEndText = "MISSION FAILED: HARD IMPACT";
                    gPlayer->setSpriteIndex(1); 
                }
            } else {
                gEndText = "MISSION FAILED: CRASHED";
                gPlayer->setSpriteIndex(1);
            }
            return;
        }
    }

    //Checks the static blocks 
    for (int i = 0; i < NUMBER_OF_TILES; i++) {
        if (gPlayer->isColliding(&gTiles[i])) {
            gGameEnded = true;
            if (gTiles[i].getEntityType() == PLATFORM && gPlayer->isCollidingBottom()) {
                // USE IMPACT VELOCITY HERE
                if (gPlayer->getImpactVelocity().y < 350.0f) {
                    gEndText = "MISSION ACCOMPLISHED!";
                    gPlayer->setSpriteIndex(0);
                } else {
                    gEndText = "MISSION FAILED: HARD IMPACT";
                    gPlayer->setSpriteIndex(1);
                }
            } else {
                gEndText = "MISSION FAILED: CRASHED";
                gPlayer->setSpriteIndex(1);
            }
            return;
        }
    }
}
void render() {
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    if (gPlayer != nullptr) {
        gPlayer->render();
        
        Vector2 v = gPlayer->getVelocity();
        DrawText(TextFormat("H-SPEED: %0.2f", v.x), 20, 50, 20, (fabs(v.x) > 100 ? RED : GREEN));
        DrawText(TextFormat("V-SPEED: %0.2f", v.y), 20, 80, 20, (v.y > 150 ? RED : GREEN));
    }
    if (gTiles) {
        for (int i = 0; i < NUMBER_OF_TILES; i++) {
            gTiles[i].render();
        }
    }

    if (gBlocks) {
        for (int i = 0; i < NUMBER_OF_BLOCKS; i++) {
            gBlocks[i].render();
        }
    }
    
    DrawText(TextFormat("FUEL: %.1f", gFuel), 20, 20, 20, RAYWHITE);

    Vector2 v = gPlayer->getVelocity();
    DrawText(TextFormat("H-SPEED: %0.2f", v.x), 20, 50, 20, (fabs(v.x) > 100 ? RED : GREEN));
    DrawText(TextFormat("V-SPEED: %0.2f", v.y), 20, 80, 20, (v.y > 150 ? RED : GREEN));

    if (gGameEnded) {
        DrawText(gEndText.c_str(), SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2, 30, YELLOW);
        DrawText("PRESS 'R' TO RESTART", SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 + 50, 20, RAYWHITE);
    }

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow();
}

