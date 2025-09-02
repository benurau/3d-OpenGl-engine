#ifndef CAMERA_H
#define CAMERA_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float JUMPHEIGHT = 1.0f;
const float GRAVITY = -0.5f;
const float SMASHSPEED = -1.0f;

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 movement = {0,0,0};
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float JumpHeight;
    bool airborne;
    float verticalVelocity;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 position = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), JumpHeight(JUMPHEIGHT)
    {
        position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position, position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
        
        if (direction == FORWARD)
        {
            movement += flatFront * velocity;
        }
        if (direction == BACKWARD)
        {
            movement += -flatFront * velocity;
        }
        if (direction == LEFT)
        {
            movement += -Right * velocity;
        }
        if (direction == RIGHT)
        {
            movement += Right * velocity;
        }
        if (direction == UP && !airborne)
        {
            verticalVelocity = JumpHeight;
        }
        if (direction == DOWN && airborne)
        {
            verticalVelocity = SMASHSPEED;;
        }
    }


    void applyGravity(float deltaTime) {
        if (airborne) verticalVelocity += GRAVITY * deltaTime;
        movement.y = verticalVelocity * deltaTime;           
    }

    bool isGrounded(const HitBox box) {
        glm::vec3 camBottom = position;
        float groundThreshold = 0.2f;
        if (camBottom.x >= box.min.x && camBottom.x <= box.max.x &&
            camBottom.z >= box.min.z && camBottom.z <= box.max.z &&
            camBottom.y+0.15f >= box.max.y && camBottom.y+0.15f <= box.max.y + groundThreshold)
        {           
            return true;
        }     
        return false;
    }

    bool isGrounded(const AABB box) {
        glm::vec3 camBottom = position;
        float groundThreshold = 0.2f;
        if (camBottom.x >= box.min.x && camBottom.x <= box.max.x &&
            camBottom.z >= box.min.z && camBottom.z <= box.max.z &&
            camBottom.y+0.15f >= box.max.y && camBottom.y+0.15f <= box.max.y + groundThreshold)
        {           
            return true;
        }     
        return false;
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        Yaw += xoffset;
        Pitch += yoffset;
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }
        updateCameraVectors();       
    }


private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif