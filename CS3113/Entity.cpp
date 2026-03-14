#include "Entity.h"

Entity::Entity() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
                   mScale {30, 30},
                   mColliderDimensions {30, 30}, 
                   mTexture { 0 },
                   mTextureType {SINGLE}, mAngle {0.0f},
                   mEntityType {NONE}, mColor {WHITE} { }
                   

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
    EntityType entityType) : mPosition {position}, mVelocity {0.0f, 0.0f}, 
    mAcceleration {0.0f, 0.0f}, mScale {scale}, mMovement {0.0f, 0.0f}, 
    mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
    mTextureType {SINGLE}, mDirection {RIGHT}, mAnimationAtlas {{}}, 
    mAnimationIndices {}, mFrameSpeed {0}, mSpeed {DEFAULT_SPEED}, 
    mAngle {0.0f}, mEntityType {entityType} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction, 
        std::vector<int>> animationAtlas, EntityType entityType) : 
        mPosition {position}, mVelocity {0.0f, 0.0f}, 
        mAcceleration {0.0f, 0.0f}, mMovement { 0.0f, 0.0f }, mScale {scale},
        mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
        mTextureType {ATLAS}, mSpriteSheetDimensions {spriteSheetDimensions},
        mAnimationAtlas {animationAtlas}, mDirection {RIGHT},
        mAnimationIndices {animationAtlas.at(RIGHT)}, 
        mFrameSpeed {DEFAULT_FRAME_SPEED}, mAngle { 0.0f }, 
        mSpeed { DEFAULT_SPEED }, mEntityType {entityType} { };

Entity::Entity(Vector2 position, Vector2 scale, Color color, EntityType entityType) 
    : mPosition {position}, mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f}, 
      mScale {scale}, mMovement {0.0f, 0.0f}, mColliderDimensions {scale}, 
      mColor {color}, mEntityType {entityType}, mTexture {0} // No texture
{ 
    mEntityStatus = ACTIVE;
}

Entity::~Entity() { UnloadTexture(mTexture); };

/**
 * Iterates through a list of collidable entities, checks for collisions with
 * the player entity, and resolves any vertical overlap by adjusting the 
 * player's position and velocity accordingly.
 * 
 * @param collidableEntities An array of pointers to `Entity` objects that 
 * represent the entities that the current `Entity` instance can potentially
 * collide with. The `collisionCheckCount` parameter specifies the number of
 * entities in the `collidableEntities` array that need to be checked for
 * collision.
 * @param collisionCheckCount The number of entities that the current entity
 * (`Entity`) should check for collisions with. This parameter specifies how
 * many entities are in the `collidableEntities` array that need to be checked
 * for collisions with the current entity.
 */void Entity::checkCollisionY(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {
            float yDistance = fabs(mPosition.y - collidableEntity->getPosition().y);
            float yOverlap  = fabs(yDistance - (mScale.y / 2.0f) - 
                              (collidableEntity->getColliderDimensions().y / 2.0f));
            
            if (mVelocity.y > 0) // Falling down
            {
                mPosition.y -= yOverlap;
                mVelocity.y  = 0;
                mIsCollidingBottom = true;
            } 
            else if (mVelocity.y < 0) // Jumping up/Hitting head
            {
                mPosition.y += yOverlap;
                mVelocity.y  = 0;
                mIsCollidingTop = true;

                // Requirement 3 logic: If we hit a BLOCK from below, deactivate it
                if (collidableEntity->getEntityType() == BLOCK) {
                    collidableEntity->deactivate(); 
                }
            }
        }
    }
}

void Entity::checkCollisionX(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {   float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (collidableEntity->mColliderDimensions.y / 2.0f));

            // Skip if barely touching vertically (standing on platform)
            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(xDistance - (mColliderDimensions.x / 2.0f) - (collidableEntity->mColliderDimensions.x / 2.0f));

            if (mVelocity.x > 0) {
                mPosition.x     -= xOverlap;
                mVelocity.x      = 0;

                // Collision!
                mIsCollidingRight = true;
            } else if (mVelocity.x < 0) {
                mPosition.x    += xOverlap;
                mVelocity.x     = 0;
 
                // Collision!
                mIsCollidingLeft = true;
            }
        }
    }
}

/**
 * Checks if two entities are colliding based on their positions and collider 
 * dimensions.
 * 
 * @param other represents another Entity with which you want to check for 
 * collision. It is a pointer to the Entity class.
 * 
 * @return returns `true` if the two entities are colliding based on their
 * positions and collider dimensions, and `false` otherwise.
 */
bool Entity::isColliding(Entity *other) const 
{
    if (!other->isActive()) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    // detect objects that are touching
    if (xDistance <= 0.0f && yDistance <= 0.0f) return true;

    return false;
}

/**
 * Updates the current frame index of an entity's animation based on the 
 * elapsed time and frame speed.
 * 
 * @param deltaTime represents the time elapsed since the last frame update.
 */
void Entity::aiUpdate(float deltaTime) {
    if (mAIType == LERPER) {
        mLerpTimer += deltaTime;

        mPosition.x = mStartPos.x + sinf(mLerpTimer * 2.0f) * 100.0f;
    }
}
void Entity::update(float deltaTime, Entity *groupA, int countA, Entity *groupB, int countB)
{
    if (mEntityStatus == INACTIVE) return;

    resetColliderFlags();

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    // Speed limiting
    float maxHorizontalSpeed = 450.0f; 
    if (mVelocity.x > maxHorizontalSpeed)  mVelocity.x = maxHorizontalSpeed;
    if (mVelocity.x < -maxHorizontalSpeed) mVelocity.x = -maxHorizontalSpeed;

    mPosition.y += mVelocity.y * deltaTime;

    mImpactVelocity = mVelocity; 

    if (groupA) checkCollisionY(groupA, countA);
    if (groupB) checkCollisionY(groupB, countB);

    mPosition.x += mVelocity.x * deltaTime;
    if (groupA) checkCollisionX(groupA, countA);
    if (groupB) checkCollisionX(groupB, countB);
}
void Entity::render()
{
    if (mEntityStatus == INACTIVE) return;

    if (mTexture.id == 0) {
        Rectangle rec = { mPosition.x - mScale.x / 2.0f, mPosition.y - mScale.y / 2.0f, mScale.x, mScale.y };
        DrawRectangleRec(rec, mColor);
    } 
    else {
        Rectangle sourceRec = getUVRectangle(&mTexture, mSpriteIndex, 2, 2);
        Rectangle destRec = { mPosition.x, mPosition.y, mScale.x, mScale.y };
        Vector2 origin = { mScale.x / 2.0f, mScale.y / 2.0f };

        DrawTexturePro(mTexture, sourceRec, destRec, origin, 0.0f, WHITE);
    }
}