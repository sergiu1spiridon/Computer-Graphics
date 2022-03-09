#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/tema2_camera.h"
#include "lab_m1/tema2/Enemy.h"
#include <vector>
#include <iostream>


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;
        int** maze;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void RenderMap(float deltaTimeSeconds);
        void RenderProjectile(float deltaTimeSeconds);
        bool checkWallColision();
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, float time);
        void RenderCharacter(float deltaTimeSeconds);

     protected:
         typedef struct bullet {
             float angleOY;
             float angleOX;
             float posX;
             float posY;
             float posZ;
             float incX, incY, incZ;
             glm::vec3 forward;
             bool isActive;
             float ttl;
         }bullet_struct;

        implemented::Tema2_camera *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        bool fovType = true;
        GLfloat fieldOfView;
        bool firstPersonPerspecive;
        float characterRotationAngle;
        float hudOxAngle;
        float px, py;
        float life;
        std::vector<bullet_struct*> bullets_array;
        std::vector<Enemy> enemiesArray;
        float playerMembersRotationAngle;
        int sign;
        float timeLeft;
        float timeToMove;
        bool pressed;
        time_t timeOfStart;
        bool gameOver = false;
    };
}   // namespace m1
