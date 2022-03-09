#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void DrawScene(glm::mat3 visMatrix);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void DrawPojectiles(glm::mat3 visMatrix, float deltaTimeSeconds);
        int CheckColisionsWithObstacles(float length, float posx, float posy);
        void DrawEnemies(glm::mat3 visMatrix, float deltaTimeSeconds);

        // Sets the logic space and view space
       // logicSpace: { x, y, width, height }
       // viewSpace: { x, y, width, height }
        glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);


    protected:
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;

        float length;
        float lengthO;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::mat3 visMatrix;

        // TODO(student): If you need any other class variables, define them here.
        std::vector<float>* obstaclesXPos;
        std::vector<float>* obstaclesYPos;

        std::vector<float>* wallXPos;
        std::vector<float>* wallYPos;
 
        std::vector<float>* projectilesXPos;
        std::vector<float>* projectilesYPos;
        std::vector<float>* projectilesAngle;
        std::vector<float>* projectileInitialPosX;
        std::vector<float>* projectileInitialPosY;

        std::vector<float>* enemiesXPos;
        std::vector<float>* enemiesYPos;
        std::vector<float>* enemiesAngle;

        float shootingTimer;
        float timer;
        float timerTotal;

        float score;
        float life;
    };
}   // namespace m1
