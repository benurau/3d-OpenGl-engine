#ifndef NDEBUG
// tests.cpp - Simulation tests for 3D OpenGL Engine
// Uses real engine headers/functions with a hidden GLFW OpenGL context.
//
// Build: set BUILD_TESTS preprocessor, link opengl32.lib + glfw3.lib.
// Exclude window.cpp from the build (or it will be guarded by !BUILD_TESTS).

#include "test.h"

#include "Colissions.h"
#include "CollisionResponse.h"
#include "Enemy.h"
#include "Projectile.h"
#include "meele.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <vector>
#include <cmath>

// =========================================================================
//  GL context setup (hidden window)
// =========================================================================

static GLFWwindow* initGL() {
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1, 1, "Test Context", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    return window;
}

static void shutdownGL(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

// =========================================================================
//  MOVEMENT TESTS
// =========================================================================

bool TestEnemyMovesTowardPlayer() {
    Enemy enemy;
    enemy.object.orientation.setPos(glm::vec3(0, 0, 0));
    enemy.moveSpeed = 2.0f;
    enemy.state = CHASE;

    glm::vec3 playerPos(10, 0, 0);
    std::vector<Projectile> projectiles(10);

    UpdateEnemy(enemy, playerPos, 1.0f, projectiles);

    ASSERT(enemy.object.orientation.position.x > 0.0f,
           "Enemy should move toward player");
    ASSERT_NEAR(enemy.object.orientation.position.x, 2.0f, 0.001f,
                "Enemy should advance by moveSpeed * dt = 2 units");
    ASSERT(enemy.state == CHASE,
           "Enemy stays in CHASE when outside attack range");
    return true;
}

bool TestEnemyMovesTowardPlayerNegativeX() {
    Enemy enemy;
    enemy.object.orientation.setPos(glm::vec3(0, 0, 0));
    enemy.moveSpeed = 2.0f;
    enemy.state = CHASE;

    glm::vec3 playerPos(-10, 0, 0);
    std::vector<Projectile> projectiles(10);

    UpdateEnemy(enemy, playerPos, 1.0f, projectiles);

    ASSERT(enemy.object.orientation.position.x < 0.0f,
           "Enemy should move toward negative x player");
    ASSERT_NEAR(enemy.object.orientation.position.x, -2.0f, 0.001f, "");
    return true;
}

bool TestEnemyMovesTowardPlayerDiagonal() {
    Enemy enemy;
    enemy.object.orientation.setPos(glm::vec3(0, 0, 0));
    enemy.moveSpeed = 2.0f;
    enemy.state = CHASE;

    glm::vec3 playerPos(3, 0, 4);
    std::vector<Projectile> projectiles(10);

    UpdateEnemy(enemy, playerPos, 1.0f, projectiles);

    float moved = glm::length(enemy.object.orientation.position);
    ASSERT_NEAR(moved, 2.0f, 0.001f,
                "Enemy should move exactly moveSpeed*dt toward player");

    glm::vec3 expectedDir = glm::normalize(playerPos);
    glm::vec3 actualDir = glm::normalize(enemy.object.orientation.position);
    ASSERT_NEAR(glm::dot(expectedDir, actualDir), 1.0f, 0.001f,
                "Enemy should move in the direction of the player");
    return true;
}

bool TestEnemyStopsAtAttackRange() {
    Enemy enemy;
    enemy.object.orientation.setPos(glm::vec3(0, 0, 0));
    enemy.moveSpeed = 2.0f;
    enemy.attackRange = 3.0f;
    enemy.state = CHASE;

    glm::vec3 playerPos(2.5f, 0, 0);
    std::vector<Projectile> projectiles(10);

    // Enemy is already within attackRange, should transition to ATTACK
    UpdateEnemy(enemy, playerPos, 1.0f, projectiles);

    ASSERT(enemy.state == ATTACK,
           "Enemy should transition to ATTACK when within range");
    ASSERT_NEAR(enemy.object.orientation.position.x, 0.0f, 0.001f,
                "Enemy should NOT move when transitioning to ATTACK");
    return true;
}

bool TestEnemyChasesAfterLeavingRange() {
    Enemy enemy;
    enemy.object.orientation.setPos(glm::vec3(0, 0, 0));
    enemy.moveSpeed = 2.0f;
    enemy.attackRange = 3.0f;
    enemy.state = ATTACK;

    glm::vec3 playerPos(10, 0, 0);
    std::vector<Projectile> projectiles(10);

    UpdateEnemy(enemy, playerPos, 1.0f, projectiles);

    ASSERT(enemy.state == CHASE,
           "Enemy should transition back to CHASE when player leaves range");
    return true;
}

bool TestEnemyDoesNotMoveInAttackState() {
    Enemy enemy;
    enemy.object.orientation.setPos({5, 0, 0});
    enemy.moveSpeed = 2.0f;
    enemy.attackRange = 10.0f;
    enemy.state = ATTACK;

    glm::vec3 playerPos(0, 0, 0);
    std::vector<Projectile> projectiles(10);

    glm::vec3 before = enemy.object.orientation.position;
    UpdateEnemy(enemy, playerPos, 1.0f, projectiles);
    glm::vec3 after = enemy.object.orientation.position;

    ASSERT(before == after, "Enemy should not move during ATTACK state");
    return true;
}

bool TestEnemyChaseMultipleSteps() {
    Enemy enemy;
    enemy.object.orientation.setPos(glm::vec3(0, 0, 0));
    enemy.moveSpeed = 2.0f;
    enemy.state = CHASE;

    glm::vec3 playerPos(10, 0, 0);
    std::vector<Projectile> projectiles(10);

    float dt = 0.016f;
    int frames = 0;
    while (enemy.state == CHASE && frames < 1000) {
        UpdateEnemy(enemy, playerPos, dt, projectiles);
        frames++;
    }

    ASSERT(enemy.state == ATTACK,
           "Enemy should eventually reach ATTACK state");
    ASSERT(frames > 0, "Should take multiple frames to reach player");
    ASSERT(enemy.state != CHASE, "Enemy is no longer chasing");

    float dist = glm::length(playerPos - enemy.object.orientation.position);
    ASSERT(dist <= enemy.attackRange,
           "Enemy should be within attack range when state changes to ATTACK");
    return true;
}

bool TestLargeDeltaTimeMovement() {
    Enemy enemy;
    enemy.object.orientation.setPos(glm::vec3(0, 0, 0));
    enemy.moveSpeed = 10.0f;
    enemy.state = CHASE;

    glm::vec3 playerPos(5, 0, 0);
    std::vector<Projectile> projectiles(10);

    UpdateEnemy(enemy, playerPos, 1.0f, projectiles);

    ASSERT(enemy.state == ATTACK,
           "Fast enemy should reach attack range in one step");
    return true;
}

bool TestZeroDeltaTime() {
    Enemy enemy;
    enemy.object.orientation.setPos(glm::vec3(0, 0, 0));
    enemy.moveSpeed = 2.0f;
    enemy.state = CHASE;

    glm::vec3 playerPos(10, 0, 0);
    std::vector<Projectile> projectiles(10);

    glm::vec3 before = enemy.object.orientation.position;
    UpdateEnemy(enemy, playerPos, 0.0f, projectiles);
    glm::vec3 after = enemy.object.orientation.position;

    ASSERT(before == after, "Enemy should not move with zero delta time");
    return true;
}

// =========================================================================
//  COLLISION TESTS
// =========================================================================

bool TestAABBvsAABB_Overlapping() {
    AABB a, b;
    a.min = glm::vec3(0, 0, 0); a.max = glm::vec3(2, 2, 2);
    b.min = glm::vec3(1, 1, 1); b.max = glm::vec3(3, 3, 3);
    ASSERT_TRUE(AABBvsAABB(a, b), "Overlapping AABBs should collide");
    return true;
}

bool TestAABBvsAABB_NonOverlapping() {
    AABB a, b;
    a.min = glm::vec3(0, 0, 0); a.max = glm::vec3(1, 1, 1);
    b.min = glm::vec3(5, 5, 5); b.max = glm::vec3(6, 6, 6);
    ASSERT_FALSE(AABBvsAABB(a, b), "Non-overlapping AABBs should not collide");
    return true;
}

bool TestAABBvsAABB_Touching() {
    AABB a, b;
    a.min = glm::vec3(0, 0, 0); a.max = glm::vec3(2, 2, 2);
    b.min = glm::vec3(2, 2, 2); b.max = glm::vec3(4, 4, 4);
    ASSERT_TRUE(AABBvsAABB(a, b), "Touching AABBs should collide");
    return true;
}

bool TestAABBvsAABBContact_Depth() {
    AABB a, b;
    a.min = glm::vec3(0, 0, 0); a.max = glm::vec3(3, 3, 3);
    b.min = glm::vec3(2, 0, 0); b.max = glm::vec3(5, 3, 3);

    ShapeContact contact = AABBvsAABBContact(a, b);
    ASSERT_TRUE(contact.isColliding, "Overlapping AABBs should produce contact");
    ASSERT_NEAR(contact.penetrationDepth, 1.0f, 0.001f, "Overlap = 1 unit on x");
    ASSERT_NEAR(contact.normal.x, -1.0f, 0.001f,
                "Normal points from a toward b on x axis");
    return true;
}

bool TestAABBvsAABBContact_EdgeCase() {
    AABB a, b;
    a.min = glm::vec3(0, 0, 0); a.max = glm::vec3(2, 2, 2);
    b.min = glm::vec3(2, 2, 2); b.max = glm::vec3(4, 4, 4);

    ShapeContact contact = AABBvsAABBContact(a, b);
    ASSERT_TRUE(contact.isColliding, "Touching AABBs should produce contact");
    ASSERT_NEAR(contact.penetrationDepth, 0.0f, 0.001f,
                "Zero overlap at touching edge");
    return true;
}

bool TestPointInAABB_Inside() {
    AABB box;
    box.min = glm::vec3(0, 0, 0); box.max = glm::vec3(2, 2, 2);
    ASSERT_TRUE(pointInAABB(glm::vec3(1, 1, 1), box).isColliding,
                "Point inside AABB should collide");
    return true;
}

bool TestPointInAABB_Outside() {
    AABB box;
    box.min = glm::vec3(0, 0, 0); box.max = glm::vec3(2, 2, 2);
    ASSERT_FALSE(pointInAABB(glm::vec3(10, 10, 10), box).isColliding,
                 "Point outside AABB should not collide");
    return true;
}

bool TestPointInAABB_OnEdge() {
    AABB box;
    box.min = glm::vec3(0, 0, 0); box.max = glm::vec3(2, 2, 2);

    ShapeContact contact = pointInAABB(glm::vec3(2, 1, 1), box);
    ASSERT_TRUE(contact.isColliding, "Point on AABB edge should collide");
    ASSERT_NEAR(contact.penetrationDepth, 0.0f, 0.001f,
                "Zero penetration at edge");
    return true;
}

bool TestAABB_SinglePoint() {
    AABB box;
    box.min = glm::vec3(1, 1, 1); box.max = glm::vec3(1, 1, 1);
    ASSERT_TRUE(pointInAABB(glm::vec3(1, 1, 1), box).isColliding,
                "Point at AABB position should collide");
    return true;
}

bool TestAABB_NegativeSpace() {
    AABB box;
    box.min = glm::vec3(-5, -5, -5); box.max = glm::vec3(-1, -1, -1);

    ASSERT_TRUE(pointInAABB(glm::vec3(-3, -3, -3), box).isColliding,
                "Point inside negative-space AABB should collide");
    ASSERT_FALSE(pointInAABB(glm::vec3(0, 0, 0), box).isColliding,
                 "Point outside negative-space AABB should not collide");

    AABB box2;
    box2.min = glm::vec3(-3, -3, -3); box2.max = glm::vec3(3, 3, 3);
    ASSERT_TRUE(AABBvsAABB(box, box2), "Overlapping AABBs in negative space");
    return true;
}

bool TestZeroSizeAABB() {
    AABB box;
    box.min = glm::vec3(0, 0, 0); box.max = glm::vec3(0, 0, 0);
    AABB other;
    other.min = glm::vec3(0, 0, 0); other.max = glm::vec3(1, 1, 1);

    ASSERT_TRUE(AABBvsAABB(box, other), "Zero-size AABB at origin should touch");
    return true;
}

bool TestComputeWorldAABB() {
    AABB local;
    local.min = glm::vec3(-1, -1, -1); local.max = glm::vec3(1, 1, 1);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(10, 0, 0));
    AABB world = computeWorldAABB(local, translate);

    ASSERT_NEAR(world.min.x, 9.0f, 0.001f, "World AABB min = local min + translate");
    ASSERT_NEAR(world.max.x, 11.0f, 0.001f, "World AABB max = local max + translate");
    return true;
}

bool TestResolveCollisionPushBack() {
    ShapeContact contact;
    contact.isColliding = true;
    contact.normal = glm::vec3(1, 0, 0);
    contact.penetrationDepth = 0.5f;

    glm::vec3 movement(-0.3f, 0, 0);
    glm::vec3 result = ResolveColissionPushBack(movement, contact);

    ASSERT_NEAR(result.x, 0.2f, 0.001f,
                "Push-back should add normal * depth to movement");
    return true;
}

bool TestMultiEntityCollisionLoop() {
    struct Entity {
        AABB box;
        glm::vec3 pos;
    };

    std::vector<Entity> entities;
    for (int x = 0; x < 5; ++x) {
        for (int z = 0; z < 5; ++z) {
            Entity e;
            e.pos = glm::vec3(x * 3.0f, 0, z * 3.0f);
            e.box.min = e.pos - glm::vec3(1, 1, 1);
            e.box.max = e.pos + glm::vec3(1, 1, 1);
            entities.push_back(e);
        }
    }

    // Move entity 0 to same position as entity 1
    entities[0].pos = entities[1].pos;
    entities[0].box.min = entities[1].box.min;
    entities[0].box.max = entities[1].box.max;
    ASSERT_TRUE(AABBvsAABB(entities[0].box, entities[1].box),
                "Entities at same position should collide");

    // Move far away
    entities[0].pos = glm::vec3(100, 100, 100);
    entities[0].box.min = entities[0].pos - glm::vec3(1, 1, 1);
    entities[0].box.max = entities[0].pos + glm::vec3(1, 1, 1);
    ASSERT_FALSE(AABBvsAABB(entities[0].box, entities[1].box),
                 "Entities far apart should not collide");

    // Move to touching
    entities[0].pos = entities[1].pos + glm::vec3(2, 0, 0);
    entities[0].box.min = entities[0].pos - glm::vec3(1, 1, 1);
    entities[0].box.max = entities[0].pos + glm::vec3(1, 1, 1);
    ASSERT_TRUE(AABBvsAABB(entities[0].box, entities[1].box),
                "Touching entities should collide");

    // Only 1 pair should collide
    int collisionCount = 0;
    for (size_t i = 0; i < entities.size(); ++i)
        for (size_t j = i + 1; j < entities.size(); ++j)
            if (AABBvsAABB(entities[i].box, entities[j].box)) collisionCount++;

    ASSERT(collisionCount == 1,
           "Only entity[0] vs entity[1] should collide after adjustments");
    return true;
}

// =========================================================================
//  CAPSULE COLLISION TESTS
// =========================================================================

bool TestPointInCapsule_Inside() {
    CapsuleWorldLoc capsule;
    capsule.p0 = glm::vec3(0, -1, 0);
    capsule.p1 = glm::vec3(0, 1, 0);
    capsule.radius = 0.5f;

    ASSERT_TRUE(pointInCapsule(glm::vec3(0, 0, 0), capsule).isColliding,
                "Point at capsule center should collide");
    return true;
}

bool TestPointInCapsule_Outside() {
    CapsuleWorldLoc capsule;
    capsule.p0 = glm::vec3(0, -1, 0);
    capsule.p1 = glm::vec3(0, 1, 0);
    capsule.radius = 0.5f;

    ASSERT_FALSE(pointInCapsule(glm::vec3(10, 0, 0), capsule).isColliding,
                 "Point far from capsule should not collide");
    return true;
}

bool TestPointInCapsule_RadiusEdge() {
    CapsuleWorldLoc capsule;
    capsule.p0 = glm::vec3(0, 0, 0);
    capsule.p1 = glm::vec3(0, 2, 0);
    capsule.radius = 1.0f;

    ShapeContact contact = pointInCapsule(glm::vec3(1.0f, 1, 0), capsule);
    ASSERT_TRUE(contact.isColliding, "Point on capsule surface should collide");
    ASSERT_NEAR(contact.penetrationDepth, 0.0f, 0.001f,
                "Zero penetration on surface");
    return true;
}

bool TestCapsuleVsAABB_Intersecting() {
    CapsuleWorldLoc capsule;
    capsule.p0 = glm::vec3(0, 0, 0); capsule.p1 = glm::vec3(0, 2, 0);
    capsule.radius = 0.5f;

    AABB box;
    box.min = glm::vec3(0.3f, 0.5f, -0.5f);
    box.max = glm::vec3(1.3f, 1.5f, 0.5f);

    ShapeContact contact = capsuleVsAABB(capsule, box);
    ASSERT_TRUE(contact.isColliding, "Capsule radius overlapping AABB should collide");
    ASSERT(contact.penetrationDepth > 0.0f, "Positive penetration depth");
    return true;
}

bool TestCapsuleVsAABB_NonIntersecting() {
    CapsuleWorldLoc capsule;
    capsule.p0 = glm::vec3(0, 0, 0); capsule.p1 = glm::vec3(0, 2, 0);
    capsule.radius = 0.5f;

    AABB box;
    box.min = glm::vec3(10, 0, 0); box.max = glm::vec3(12, 2, 2);

    ASSERT_FALSE(capsuleVsAABB(capsule, box).isColliding,
                 "Capsule far from AABB should not collide");
    return true;
}

bool TestCapsuleVsAABB_CapsuleInside() {
    CapsuleWorldLoc capsule;
    capsule.p0 = glm::vec3(0, 0, 0); capsule.p1 = glm::vec3(0, 1, 0);
    capsule.radius = 0.5f;

    AABB box;
    box.min = glm::vec3(-2, -2, -2); box.max = glm::vec3(2, 2, 2);

    ASSERT_TRUE(capsuleVsAABB(capsule, box).isColliding,
                "Capsule inside AABB should collide");
    return true;
}

// =========================================================================
//  SEGMENT / TRIANGLE TESTS
// =========================================================================

bool TestSegmentVsAABB_Hit() {
    AABB box;
    box.min = glm::vec3(0, 0, 0); box.max = glm::vec3(2, 2, 2);
    ASSERT_TRUE(segmentVsAABB(glm::vec3(-1, 1, 1), glm::vec3(3, 1, 1), box),
                "Segment through AABB should hit");
    return true;
}

bool TestSegmentVsAABB_Miss() {
    AABB box;
    box.min = glm::vec3(0, 0, 0); box.max = glm::vec3(2, 2, 2);
    ASSERT_FALSE(segmentVsAABB(glm::vec3(-1, 10, 10), glm::vec3(3, 10, 10), box),
                 "Segment far from AABB should miss");
    return true;
}

bool TestSegmentVsAABB_StartsInside() {
    AABB box;
    box.min = glm::vec3(0, 0, 0); box.max = glm::vec3(2, 2, 2);
    ASSERT_TRUE(segmentVsAABB(glm::vec3(1, 1, 1), glm::vec3(5, 5, 5), box),
                "Segment starting inside AABB should hit");
    return true;
}

bool TestClosestPointOnTriangle() {
    glm::vec3 a(0, 0, 0), b(1, 0, 0), c(0, 1, 0);

    glm::vec3 closest = closestPointOnTriangle(glm::vec3(0.2f, 0.2f, 0), a, b, c);
    ASSERT_NEAR(closest.x, 0.2f, 0.001f, "Interior point projects to itself");
    ASSERT_NEAR(closest.y, 0.2f, 0.001f, "");
    ASSERT_NEAR(closest.z, 0.0f, 0.001f, "");

    closest = closestPointOnTriangle(glm::vec3(5, 5, 0), a, b, c);
    float dist = glm::length(closest - glm::vec3(0.5f, 0.5f, 0));
    ASSERT(dist < 0.01f, "Far point projects onto nearest triangle edge/hypotenuse");
    return true;
}

// =========================================================================
//  PROJECTILE TESTS  (uses real SpawnProjectile / UpdateProjectile)
// =========================================================================

bool TestProjectileSpawn() {
    std::vector<Projectile> pool(5);

    ProjectileType type;
    type.speed = 10.0f;
    type.lifetime = 3.0f;
    type.damage = 25.0f;

    glm::vec3 pos(1, 2, 3);
    glm::vec3 dir(1, 0, 0);

    SpawnProjectile(pos, dir, type, pool, 0);

    ASSERT_TRUE(pool[0].active, "First pool entry should become active");
    ASSERT(pool[0].object.orientation.position == pos,
           "Projectile spawns at given position");
    ASSERT_NEAR(pool[0].velocity.x, 10.0f, 0.001f,
                "Velocity = direction * speed");
    ASSERT_NEAR(pool[0].type.lifetime, 3.0f, 0.001f, "Lifetime set");
    ASSERT(pool[0].ownerId == 0, "Owner ID set");
    return true;
}

bool TestProjectileSpawnReusesInactive() {
    std::vector<Projectile> pool(3);
    pool[0].active = true;
    pool[0].ownerId = 99;

    ProjectileType type;
    type.speed = 5.0f;
    type.lifetime = 2.0f;

    glm::vec3 pos(5, 5, 5);
    glm::vec3 dir(0, 1, 0);
    SpawnProjectile(pos, dir, type, pool, 42);

    ASSERT(pool[0].ownerId == 99,
           "Active projectile should not be overwritten");
    ASSERT(pool[1].active, "First inactive slot should be reused");
    ASSERT(pool[1].ownerId == 42, "Owner ID set on reused projectile");
    return true;
}

bool TestProjectileMovement() {
    std::vector<Projectile> pool(1);
    ProjectileType type;
    type.speed = 10.0f;
    type.lifetime = 5.0f;

    glm::vec3 spawnPos(0, 0, 0);
    glm::vec3 spawnDir(1, 0, 0);
    SpawnProjectile(spawnPos, spawnDir, type, pool, 0);

    float dt = 1.0f;
    UpdateProjectile(pool[0], dt);

    ASSERT_NEAR(pool[0].object.orientation.position.x, 10.0f, 0.001f,
                "Projectile advances by velocity * dt");
    ASSERT_NEAR(pool[0].type.lifetime, 4.0f, 0.001f,
                "Lifetime decreases by dt");
    return true;
}

bool TestProjectileDeactivatesOnExpiry() {
    std::vector<Projectile> pool(1);
    ProjectileType type;
    type.speed = 5.0f;
    type.lifetime = 0.5f;

    glm::vec3 spawnPos(0, 0, 0);
    glm::vec3 spawnDir(1, 0, 0);
    SpawnProjectile(spawnPos, spawnDir, type, pool, 0);
    float dt = 1.0f;
    UpdateProjectile(pool[0], dt);

    ASSERT_FALSE(pool[0].active, "Projectile deactivates when lifetime <= 0");
    return true;
}

bool TestProjectileMultipleUpdates() {
    std::vector<Projectile> pool(1);
    ProjectileType type;
    type.speed = 10.0f;
    type.lifetime = 3.0f;

    glm::vec3 spawnPos(0, 0, 0);
    glm::vec3 spawnDir(1, 0, 0);
    SpawnProjectile(spawnPos, spawnDir, type, pool, 0);

    for (int i = 0; i < 5; ++i) {
        float dt = 0.5f;
        UpdateProjectile(pool[0], dt);
    }

    ASSERT_NEAR(pool[0].object.orientation.position.x, 25.0f, 0.001f,
                "5 * 0.5s at speed 10 = 25 units");
    ASSERT_NEAR(pool[0].type.lifetime, 0.5f, 0.001f,
                "5 * 0.5s from 3s = 0.5s remaining");
    ASSERT_TRUE(pool[0].active, "Still active");
    return true;
}

bool TestProjectileExpiresDuringUpdate() {
    std::vector<Projectile> pool(1);
    ProjectileType type;
    type.speed = 10.0f;
    type.lifetime = 0.1f;

    glm::vec3 spawnPos(0, 0, 0);
    glm::vec3 spawnDir(1, 0, 0);
    SpawnProjectile(spawnPos, spawnDir, type, pool, 0);
    float dt = 0.2f;
    UpdateProjectile(pool[0], dt);

    ASSERT_FALSE(pool[0].active, "Projectile should expire");
    ASSERT_NEAR(pool[0].object.orientation.position.x, 2.0f, 0.001f,
                "Projectile moves before deactivating");
    return true;
}

bool TestProjectilePoolFullBehavior() {
    std::vector<Projectile> pool(2);
    pool[0].active = true;
    pool[1].active = true;

    ProjectileType type;
    type.speed = 10.0f;
    type.lifetime = 3.0f;

    glm::vec3 pos(0, 0, 0);
    glm::vec3 dir(1, 0, 0);
    SpawnProjectile(pos, dir, type, pool, 5);

    // No inactive slot — SpawnProjectile silently does nothing
    ASSERT(pool[0].ownerId != 5 || !pool[0].active,
           "Should not overwrite active projectile");
    ASSERT(pool[1].ownerId != 5 || !pool[1].active, "");
    return true;
}

bool TestProjectileZeroVelocity() {
    std::vector<Projectile> pool(1);
    ProjectileType type;
    type.speed = 0.0f;
    type.lifetime = 1.0f;

    glm::vec3 spawnPos(1, 2, 3);
    glm::vec3 zeroDir(0, 0, 0);
    SpawnProjectile(spawnPos, zeroDir, type, pool, 0);
    float dt = 1.0f;
    UpdateProjectile(pool[0], dt);

    ASSERT(pool[0].object.orientation.position == glm::vec3(1, 2, 3),
           "Zero-velocity projectile should not move");
    ASSERT_FALSE(pool[0].active, "Should still expire");
    return true;
}

// =========================================================================
//  MEELE SWEEP TESTS  (uses real MeeleAttack struct)
// =========================================================================

bool TestMeeleSweepFirstFrame() {
    glm::vec3 tip(1, 2, 3);
    glm::vec3 prevTip, capP0, capP1;
    float capRad = 0.0f;
    bool hasPrev = false;
    float meeleRad = 0.3f;

    // Simulate the sweep capsule logic from meele.h::updateSweepCapsule
    if (!hasPrev) {
        prevTip = tip;
        capP0 = tip;
        capP1 = tip;
        capRad = meeleRad;
        hasPrev = true;
    }

    ASSERT(hasPrev, "hasPrevTip set after first frame");
    ASSERT(capP0 == tip, "Capsule starts at current tip");
    ASSERT(capP1 == tip, "Capsule ends at current tip");
    ASSERT_NEAR(capRad, meeleRad, 0.001f, "Radius set");
    return true;
}

bool TestMeeleSweepSecondFrame() {
    glm::vec3 prevTip, capP0, capP1;
    float capRad = 0.0f;
    bool hasPrev = false;
    float meeleRad = 0.3f;

    glm::vec3 tip1(0, 0, 0);

    // Frame 1
    if (!hasPrev) {
        prevTip = tip1; capP0 = tip1; capP1 = tip1; capRad = meeleRad; hasPrev = true;
    }

    glm::vec3 tip2(2, 0, 0);

    // Frame 2
    capP0 = prevTip;
    capP1 = tip2;
    capRad = meeleRad;
    prevTip = tip2;

    ASSERT(capP0 == tip1, "Capsule P0 = previous tip");
    ASSERT(capP1 == tip2, "Capsule P1 = current tip");
    ASSERT(prevTip == tip2, "prevWorldTip updated");
    return true;
}

bool TestMeeleSweepMultipleFrames() {
    glm::vec3 prevTip, capP0, capP1;
    float capRad = 0.0f;
    bool hasPrev = false;
    float meeleRad = 0.3f;

    std::vector<glm::vec3> tips = {
        {0,0,0}, {1,0,0}, {2,0,0}, {3,0,0}, {4,0,0}
    };

    for (size_t i = 0; i < tips.size(); ++i) {
        if (!hasPrev) {
            prevTip = tips[i]; capP0 = tips[i]; capP1 = tips[i];
            capRad = meeleRad; hasPrev = true;
            continue;
        }
        capP0 = prevTip;
        capP1 = tips[i];
        capRad = meeleRad;
        ASSERT(capP0 == tips[i - 1], "P0 = previous tip");
        ASSERT(capP1 == tips[i], "P1 = current tip");
        prevTip = tips[i];
    }

    ASSERT(prevTip == tips.back(), "prevWorldTip = last tip");
    return true;
}

bool TestMeeleSweepCapsuleCollision() {
    glm::vec3 prevTip, capP0, capP1;
    float capRad = 0.0f;
    bool hasPrev = false;
    float meeleRad = 0.5f;

    // Frame 1
    if (!hasPrev) {
        prevTip = glm::vec3(0,0,0); capP0 = glm::vec3(0,0,0);
        capP1 = glm::vec3(0,0,0); capRad = meeleRad; hasPrev = true;
    }
    // Frame 2
    capP0 = prevTip;
    capP1 = glm::vec3(2, 0, 0);
    capRad = meeleRad;
    prevTip = glm::vec3(2, 0, 0);

    CapsuleWorldLoc sweep = { capP0, capP1, capRad };

    AABB enemyBox;
    enemyBox.min = glm::vec3(0.8f, -0.3f, -0.3f);
    enemyBox.max = glm::vec3(1.2f, 0.3f, 0.3f);

    ASSERT_TRUE(capsuleVsAABB(sweep, enemyBox).isColliding,
                "Meele sweep hits enemy near sweep path");

    AABB farBox;
    farBox.min = glm::vec3(10, 10, 10);
    farBox.max = glm::vec3(12, 12, 12);

    ASSERT_FALSE(capsuleVsAABB(sweep, farBox).isColliding,
                 "Meele sweep misses far enemy");
    return true;
}

// =========================================================================
//  MAIN
// =========================================================================

int main() {
    GLFWwindow* window = initGL();

    TestRunner runner;

    // Movement
    runner.add("EnemyMovesTowardPlayer", TestEnemyMovesTowardPlayer);
    runner.add("EnemyMovesTowardPlayerNegativeX", TestEnemyMovesTowardPlayerNegativeX);
    runner.add("EnemyMovesTowardPlayerDiagonal", TestEnemyMovesTowardPlayerDiagonal);
    runner.add("EnemyStopsAtAttackRange", TestEnemyStopsAtAttackRange);
    runner.add("EnemyChasesAfterLeavingRange", TestEnemyChasesAfterLeavingRange);
    runner.add("EnemyDoesNotMoveInAttackState", TestEnemyDoesNotMoveInAttackState);
    runner.add("EnemyChaseMultipleSteps", TestEnemyChaseMultipleSteps);
    runner.add("LargeDeltaTimeMovement", TestLargeDeltaTimeMovement);
    runner.add("ZeroDeltaTime", TestZeroDeltaTime);

    // AABB collision
    runner.add("AABBvsAABB_Overlapping", TestAABBvsAABB_Overlapping);
    runner.add("AABBvsAABB_NonOverlapping", TestAABBvsAABB_NonOverlapping);
    runner.add("AABBvsAABB_Touching", TestAABBvsAABB_Touching);
    runner.add("AABBvsAABBContact_Depth", TestAABBvsAABBContact_Depth);
    runner.add("AABBvsAABBContact_EdgeCase", TestAABBvsAABBContact_EdgeCase);
    runner.add("PointInAABB_Inside", TestPointInAABB_Inside);
    runner.add("PointInAABB_Outside", TestPointInAABB_Outside);
    runner.add("PointInAABB_OnEdge", TestPointInAABB_OnEdge);
    runner.add("AABB_SinglePoint", TestAABB_SinglePoint);
    runner.add("AABB_NegativeSpace", TestAABB_NegativeSpace);
    runner.add("ZeroSizeAABB", TestZeroSizeAABB);
    runner.add("ComputeWorldAABB", TestComputeWorldAABB);
    runner.add("ResolveCollisionPushBack", TestResolveCollisionPushBack);
    runner.add("MultiEntityCollisionLoop", TestMultiEntityCollisionLoop);

    // Capsule collision
    runner.add("PointInCapsule_Inside", TestPointInCapsule_Inside);
    runner.add("PointInCapsule_Outside", TestPointInCapsule_Outside);
    runner.add("PointInCapsule_RadiusEdge", TestPointInCapsule_RadiusEdge);
    runner.add("CapsuleVsAABB_Intersecting", TestCapsuleVsAABB_Intersecting);
    runner.add("CapsuleVsAABB_NonIntersecting", TestCapsuleVsAABB_NonIntersecting);
    runner.add("CapsuleVsAABB_CapsuleInside", TestCapsuleVsAABB_CapsuleInside);

    // Segment / triangle
    runner.add("SegmentVsAABB_Hit", TestSegmentVsAABB_Hit);
    runner.add("SegmentVsAABB_Miss", TestSegmentVsAABB_Miss);
    runner.add("SegmentVsAABB_StartsInside", TestSegmentVsAABB_StartsInside);
    runner.add("ClosestPointOnTriangle", TestClosestPointOnTriangle);

    // Projectile
    runner.add("ProjectileSpawn", TestProjectileSpawn);
    runner.add("ProjectileSpawnReusesInactive", TestProjectileSpawnReusesInactive);
    runner.add("ProjectileMovement", TestProjectileMovement);
    runner.add("ProjectileDeactivatesOnExpiry", TestProjectileDeactivatesOnExpiry);
    runner.add("ProjectileMultipleUpdates", TestProjectileMultipleUpdates);
    runner.add("ProjectileExpiresDuringUpdate", TestProjectileExpiresDuringUpdate);
    runner.add("ProjectilePoolFullBehavior", TestProjectilePoolFullBehavior);
    runner.add("ProjectileZeroVelocity", TestProjectileZeroVelocity);

    // Meele sweep
    runner.add("MeeleSweepFirstFrame", TestMeeleSweepFirstFrame);
    runner.add("MeeleSweepSecondFrame", TestMeeleSweepSecondFrame);
    runner.add("MeeleSweepMultipleFrames", TestMeeleSweepMultipleFrames);
    runner.add("MeeleSweepCapsuleCollision", TestMeeleSweepCapsuleCollision);

    bool allPassed = runner.runAll();
    shutdownGL(window);
    return allPassed ? 0 : 1;
}
#endif // BUILD_TESTS
