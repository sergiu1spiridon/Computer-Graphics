#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"


namespace implemented
{
    class Tema2_camera
    {
     public:
        Tema2_camera()
        {
            position    = glm::vec3(0, 0, 0);
            forward     = glm::vec3(0, 0, -1);
            up          = glm::vec3(0, 1, 0);
            right       = glm::vec3(1, 0, 0);
            distanceToTarget = 3;
        }

        Tema2_camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            Set(position, center, up);
        }

        ~Tema2_camera()
        { }

        // Update camera
        void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            this->position = position;
            forward     = glm::normalize(center - position);
            right       = glm::cross(forward, up);
            this->up    = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            // Translates the camera using the `dir` vector computed from
            // `forward`. Movement will always keep the camera at the same
            // height. For example, if you rotate your head up/down, and then
            // walk forward, then you will still keep the same relative
            // distance (height) to the ground!
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            position += glm::normalize(forward) * distance;
        }

        void TranslateUpward(float distance)
        {
            position += glm::normalize(up) * distance;
        }

        void TranslateRight(float distance)
        {
            
            position += glm::normalize(right) * distance;

        }

        void RotateFirstPerson_OX(float angle)
        {
            
            glm::vec4 helper = glm::rotate(glm::mat4(1.f), angle, right) * glm::vec4(forward, 0);
            forward = glm::normalize(glm::vec3(helper));
            up = glm::cross(right, forward);

        }

        void RotateFirstPerson_OY(float angle)
        {
            
            glm::vec4 helper = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 0);
            forward = glm::normalize(glm::vec3(helper));
            helper = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 0);
            right = glm::normalize(glm::vec3(helper));
            up = glm::cross(right, forward);

        }

        void RotateFirstPerson_OZ(float angle)
        {
            
            glm::vec4 helper = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
            right = glm::normalize(glm::vec3(helper));
            helper = glm::rotate(glm::mat4(1.f), angle, forward) * glm::vec4(up, 0);
            forward = glm::normalize(glm::vec3(helper));

            up = glm::cross(right, forward);
        }

        void RotateThirdPerson_OX(float angle)
        {
            
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateForward(-distanceToTarget);

        }

        void RotateThirdPerson_OY(float angle)
        {
            
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OZ(float angle)
        {
            
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);
        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

        glm::vec3 GetPosition()
        {
            return position + forward * 0.3f;
        }

     public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
    };
}   // namespace implemented
