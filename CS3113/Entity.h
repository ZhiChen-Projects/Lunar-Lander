#ifndef ENTITY_H
#define ENTITY_H

#include "cs3113.h"

enum Direction    { LEFT, UP, RIGHT, DOWN         };
enum EntityStatus { ACTIVE, INACTIVE              };
enum EntityType { PLAYER, BLOCK, PLATFORM, START_PAD, NONE };
enum AIType       { STATIC, LERPER                }; 

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions;
    
    Texture2D mTexture;
    TextureType mTextureType;
    Vector2 mSpriteSheetDimensions;
    
    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int> mAnimationIndices;
    Direction mDirection;
    int mFrameSpeed;

    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;

    bool mIsJumping = false;
    float mJumpingPower = 0.0f;

    int mSpeed;
    float mAngle;

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType   mEntityType;
    AIType       mAIType = STATIC; 

    Vector2 mStartPos;
    float mLerpTimer = 0.0f;

    void checkCollisionY(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionX(Entity *collidableEntities, int collisionCheckCount);
    void resetColliderFlags() 
    {
        mIsCollidingTop = mIsCollidingBottom = mIsCollidingRight = mIsCollidingLeft = false;
    }
    //Not sure if we can use Color class, but it pretty much allows me to have the color to visualize the good and bad platforms/blocks
    Color mColor;
    int mTextureRows = 1;
    int mTextureCols = 1;
    int mSpriteIndex = 0; //For the face
    Vector2 mImpactVelocity = { 0.0f, 0.0f };

    
public:
    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 200;
    static constexpr int   DEFAULT_FRAME_SPEED   = 14;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType);
    Entity(Vector2 position, Vector2 scale, Color color, EntityType entityType);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, 
        std::map<Direction, std::vector<int>> animationAtlas, EntityType entityType);
    ~Entity();

    void render();
    
    void aiUpdate(float deltaTime); 
    void deactivate() { mEntityStatus = INACTIVE; }
    void activate()   { mEntityStatus = ACTIVE;   }
    
    EntityType getEntityType() const { return mEntityType; }
    void setAIType(AIType type) { mAIType = type; mStartPos = mPosition; }
    void setAcceleration(Vector2 acc) { mAcceleration = acc; }
    Vector2 getPosition() const { return mPosition; }
    bool isCollidingBottom() const { return mIsCollidingBottom; }
    bool isCollidingLeft()   const { return mIsCollidingLeft;   }
    bool isCollidingRight()  const { return mIsCollidingRight;  }
    bool isActive()          const { return mEntityStatus == ACTIVE; }
    bool isColliding(Entity *other) const; 
    Vector2 getColliderDimensions() const { return mScale; }   
    void setColor(Color color) { mColor = color; } 
    Vector2 getVelocity() const { return mVelocity; }
    void setPosition(Vector2 position) { mPosition = position; }
    void setVelocity(Vector2 velocity) { mVelocity = velocity; }
    void setSpriteIndex(int index) { mSpriteIndex = index; }

    void update(float deltaTime, Entity *groupA, int countA, Entity *groupB, int countB);
    Vector2 getImpactVelocity() const { return mImpactVelocity; }
};

#endif