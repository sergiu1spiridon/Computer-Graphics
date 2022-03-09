#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>
#include <math.h>

#include "lab_m1/tema1/transform2D.h"
#include "lab_m1/tema1/tema1_object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    score = 0;
    life = 100;

    timer = 0;
    timerTotal = 5.0f;
    shootingTimer = 0;

    logicSpace.x = 0;       // logic x
    logicSpace.y = 0;       // logic y
    logicSpace.width = 8;   // logic width
    logicSpace.height = 4;  // logic height

    glm::vec3 corner = glm::vec3(0, 0, 0);
    length = 0.5f;
    //lengthO = 0.8f;
    lengthO = 0.8f;

    cx = corner.x + length / 2;
    cy = corner.y + length / 2;

    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize angularStep
    angularStep = 0;

    Mesh* square1 = tema1_object2D::CreateSquare("square1", corner, length, glm::vec3(0, 1, 1), true);
    AddMeshToList(square1);

    Mesh* obstacle = tema1_object2D::CreateObstacle("obstacle", corner, lengthO, glm::vec3(1, 0, 0), true);
    AddMeshToList(obstacle);

    Mesh* projectile = tema1_object2D::CreateObstacle("projectile", corner, length / 4, glm::vec3(1, 1, 1), true);
    AddMeshToList(projectile);

    Mesh* wall = tema1_object2D::CreateObstacle("wall", corner, 10 * lengthO, glm::vec3(0.5f), true);
    AddMeshToList(wall);

    Mesh* enemy = tema1_object2D::CreateSquare("enemy", corner, length, glm::vec3(1, 0, 0), true);
    AddMeshToList(enemy);

    obstaclesXPos = new vector<float>();
    obstaclesYPos = new vector<float>();

    wallXPos = new vector<float>();
    wallYPos = new vector<float>();

    projectilesXPos = new vector<float>();
    projectilesYPos = new vector<float>();
    projectilesAngle = new vector<float>();
    projectileInitialPosX = new vector<float>();
    projectileInitialPosY = new vector<float>();

    enemiesXPos = new vector<float>();
    enemiesYPos = new vector<float>();
    enemiesAngle = new vector<float>();

    enemiesXPos->push_back(0);
    enemiesYPos->push_back(0);
    enemiesAngle->push_back(0);
}

// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
        sx, 0.0f, tx,
        0.0f, sy, ty,
        0.0f, 0.0f, 1.0f));
}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx = 0, ty = 0, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}


void Tema1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();

    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);

    // Compute uniform 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    //DrawScene(visMatrix);

    viewSpace = ViewportSpace(
        resolution.x / 9, 
        resolution.y / 9, 
        ((7 * resolution.x) / 9), 
        ((7 * resolution.y) / 9));

    SetViewportArea(viewSpace, glm::vec3(0.2f), true);

    // Compute uniform 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    DrawScene(visMatrix);

    modelMatrix = visMatrix * transform2D::Translate(logicSpace.width / 2 - length / 2 + translateX, logicSpace.height /2 + translateY);

    glm::vec2 positionCursor = window->GetCursorPosition();
    GLfloat dy = (resolution.y - positionCursor.y) - (resolution.y / 2);
    GLfloat dx = positionCursor.x - (resolution.x / 2);

    angularStep = atan2(dy, dx);


    modelMatrix *= transform2D::Translate(length / 2, length / 2);

    
    modelMatrix *= transform2D::Rotate(3 * M_PI / 2);
    modelMatrix *= transform2D::Rotate(angularStep);

    modelMatrix *= transform2D::Translate(-length / 2, -length / 2);

    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    DrawEnemies(visMatrix, deltaTimeSeconds);
    DrawPojectiles(visMatrix, deltaTimeSeconds);

    Mesh* lifebar = tema1_object2D::CreateLifebar("lifebar", glm::vec3(0, 0, 1), life, true);
    modelMatrix = visMatrix * transform2D::Translate(logicSpace.width - 1.3 + translateX, logicSpace.height - 0.3 + translateY);
    RenderMesh(lifebar, shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawPojectiles(glm::mat3 visMatrix, float deltaTimeSeconds) {

    shootingTimer += deltaTimeSeconds;

    for (int i = 0; i < projectileInitialPosX->size(); i++) {
        if (projectileInitialPosX->at(i) != -1) {
            if ((sqrt(pow(projectilesXPos->at(i), 2) + pow(projectilesYPos->at(i), 2)) -
                sqrt(pow(projectileInitialPosX->at(i), 2) + pow(projectileInitialPosY->at(i), 2))) < 2) {
                float translateInX = projectilesXPos->at(i);
                float translateInY = projectilesYPos->at(i);

                // up right
                if (projectilesAngle->at(i) <= M_PI / 2 && projectilesAngle->at(i) >= 0) {
                    translateInX += 1 / sqrt(pow(tan(projectilesAngle->at(i)), 2) + 1) * deltaTimeSeconds * 2;
                    translateInY += 1 / sqrt(pow(tan(projectilesAngle->at(i)), 2) + 1) * tan(projectilesAngle->at(i)) * deltaTimeSeconds * 2;
                }
                // up left
                if (projectilesAngle->at(i) >= M_PI / 2 && projectilesAngle->at(i) <= M_PI) {
                    translateInX -= 1 / sqrt(pow(tan(projectilesAngle->at(i)), 2) + 1) * deltaTimeSeconds * 2;
                    translateInY -= 1 / sqrt(pow(tan(projectilesAngle->at(i)), 2) + 1) * tan(projectilesAngle->at(i)) * deltaTimeSeconds * 2;
                }
                // down left
                if (projectilesAngle->at(i) >= -M_PI && projectilesAngle->at(i) <= -M_PI / 2) {
                    translateInX -= 1 / sqrt(pow(tan(projectilesAngle->at(i)), 2) + 1) * deltaTimeSeconds * 2;
                    translateInY -= 1 / sqrt(pow(tan(projectilesAngle->at(i)), 2) + 1) * tan(projectilesAngle->at(i)) * deltaTimeSeconds * 2;
                }
                //down right
                if (projectilesAngle->at(i) >= -M_PI / 2 && projectilesAngle->at(i) <= 0) {
                    translateInX += 1 / sqrt(pow(tan(projectilesAngle->at(i)), 2) + 1) * deltaTimeSeconds * 2;
                    translateInY += 1 / sqrt(pow(tan(projectilesAngle->at(i)), 2) + 1) * tan(projectilesAngle->at(i)) * deltaTimeSeconds * 2;
                }

                projectilesXPos->at(i) = translateInX;
                projectilesYPos->at(i) = translateInY;

                if (!CheckColisionsWithObstacles(length / 4, projectilesXPos->at(i), projectilesYPos->at(i))) {
                    projectileInitialPosX->at(i) = -1;
                }
                else
                {
                    modelMatrix = visMatrix * transform2D::Translate(translateInX, translateInY);
                    modelMatrix *= transform2D::Rotate(projectilesAngle->at(i));
                    RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);
                }
            }
        }
    }
}

void Tema1::DrawScene(glm::mat3 visMatrix)
{

    modelMatrix = visMatrix * transform2D::Translate(5, 5);
    RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
    obstaclesXPos->push_back(5);
    obstaclesYPos->push_back(5);

    modelMatrix = visMatrix * transform2D::Translate(0, 3);
    RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
    obstaclesXPos->push_back(0);
    obstaclesYPos->push_back(3);

    modelMatrix = visMatrix * transform2D::Translate(3, 3);
    RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
    obstaclesXPos->push_back(3);
    obstaclesYPos->push_back(3);

    modelMatrix = visMatrix * transform2D::Translate(4, 4);
    RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
    obstaclesXPos->push_back(4);
    obstaclesYPos->push_back(4);

    modelMatrix = visMatrix * transform2D::Translate(-10 * lengthO, 0);
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], modelMatrix);
    wallXPos->push_back(-10 * lengthO);
    wallYPos->push_back(0);

    modelMatrix = visMatrix * transform2D::Translate(0, -10 * lengthO);
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], modelMatrix);
    wallXPos->push_back(0);
    wallYPos->push_back(-10 * lengthO);

    modelMatrix = visMatrix * transform2D::Translate(10 * lengthO, 0);
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], modelMatrix);
    wallXPos->push_back(10 * lengthO);
    wallYPos->push_back(0);

    modelMatrix = visMatrix * transform2D::Translate(0, 10 * lengthO);
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], modelMatrix);
    wallXPos->push_back(0);
    wallYPos->push_back(10 * lengthO);

    modelMatrix = visMatrix * transform2D::Translate(-10 * lengthO, -10 * lengthO);
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], modelMatrix);
    wallXPos->push_back(-10 * lengthO);
    wallYPos->push_back(-10 * lengthO);

    modelMatrix = visMatrix * transform2D::Translate(-10 * lengthO, 10 * lengthO);
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], modelMatrix);
    wallXPos->push_back(-10 * lengthO);
    wallYPos->push_back(10 * lengthO);

    modelMatrix = visMatrix * transform2D::Translate(10 * lengthO, -10 * lengthO);
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], modelMatrix);
    wallXPos->push_back(10 * lengthO);
    wallYPos->push_back(-10 * lengthO);

    modelMatrix = visMatrix * transform2D::Translate(10 * lengthO, 10 * lengthO);
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], modelMatrix);
    wallXPos->push_back(10 * lengthO);
    wallYPos->push_back(10 * lengthO);




}

void Tema1::DrawEnemies(glm::mat3 visMatrix, float deltaTimeSeconds) {
    timer += deltaTimeSeconds;
    glm::ivec2 resolution = window->GetResolution();

    if (timer > timerTotal) {
        enemiesXPos->push_back(rand() % (int)(10 * lengthO));
        enemiesYPos->push_back(rand() % (int)(10 * lengthO));
        enemiesAngle->push_back(0);

        timer -= timerTotal;
    }

    float cx = logicSpace.width / 2 - length / 2 + translateX;
    float cy = logicSpace.height / 2 + translateY;
    GLfloat dy;
    GLfloat dx;

    for (int i = 0; i < enemiesXPos->size(); i++) {
        if (enemiesXPos->at(i) != -1) {
            dy = cy - enemiesYPos->at(i);
            dx = cx - enemiesXPos->at(i);

            enemiesAngle->at(i) = atan2(dy, dx);

            float translateInX = enemiesXPos->at(i);
            float translateInY = enemiesYPos->at(i);

            int notHit = 1;
            for (int j = 0; j < projectilesXPos->size(); j++) {
                if (projectileInitialPosX->at(j) != -1) {
                    if (abs(translateInX - projectilesXPos->at(j)) < length / 2 &&
                        abs(translateInY - projectilesYPos->at(j)) < length / 2) {
                        // enemy was shot
                        score += 10;
                        projectileInitialPosX->at(j) = -1;
                        life += 10;
                        notHit = 0;
                        printf("Your score is: %d\n", (int)score);
                        break;
                    }
                }
            }

            // enemy hits player
            if (abs(translateInX - cx) < length / 2 && abs(translateInY - cy) < length / 2 || !notHit) {
                enemiesXPos->at(i) = -1;
                life -= 10;
                if (life == 0) {
                    exit(0);
                }
            }
            else
            {
                // up right
                if (enemiesAngle->at(i) <= M_PI / 2 && enemiesAngle->at(i) >= 0) {
                    translateInX += 1 / sqrt(pow(tan(enemiesAngle->at(i)), 2) + 1) * deltaTimeSeconds * 2;
                    translateInY += 1 / sqrt(pow(tan(enemiesAngle->at(i)), 2) + 1) * tan(enemiesAngle->at(i)) * deltaTimeSeconds * 2;
                }
                // up left
                if (enemiesAngle->at(i) >= M_PI / 2 && enemiesAngle->at(i) <= M_PI) {
                    translateInX -= 1 / sqrt(pow(tan(enemiesAngle->at(i)), 2) + 1) * deltaTimeSeconds * 2;
                    translateInY -= 1 / sqrt(pow(tan(enemiesAngle->at(i)), 2) + 1) * tan(enemiesAngle->at(i)) * deltaTimeSeconds * 2;
                }
                // down left
                if (enemiesAngle->at(i) >= -M_PI && enemiesAngle->at(i) <= -M_PI / 2) {
                    translateInX -= 1 / sqrt(pow(tan(enemiesAngle->at(i)), 2) + 1) * deltaTimeSeconds * 2;
                    translateInY -= 1 / sqrt(pow(tan(enemiesAngle->at(i)), 2) + 1) * tan(enemiesAngle->at(i)) * deltaTimeSeconds * 2;
                }
                //down right
                if (enemiesAngle->at(i) >= -M_PI / 2 && enemiesAngle->at(i) <= 0) {
                    translateInX += 1 / sqrt(pow(tan(enemiesAngle->at(i)), 2) + 1) * deltaTimeSeconds * 2;
                    translateInY += 1 / sqrt(pow(tan(enemiesAngle->at(i)), 2) + 1) * tan(enemiesAngle->at(i)) * deltaTimeSeconds * 2;
                }

                enemiesXPos->at(i) = translateInX;
                enemiesYPos->at(i) = translateInY;

                modelMatrix = visMatrix * transform2D::Translate(enemiesXPos->at(i), enemiesYPos->at(i));
                modelMatrix *= transform2D::Rotate(3 * M_PI / 2);
                modelMatrix *= transform2D::Rotate(enemiesAngle->at(i));
                RenderMesh2D(meshes["enemy"], shaders["VertexColor"], modelMatrix);
            }
        }
    }
}


void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    float px = logicSpace.x;
    float py = logicSpace.y;
    float trax = translateX;
    float tray = translateY;

    // TODO(student): Move the logic window with W, A, S, D (up, left, down, right)
    if (window->KeyHold(GLFW_KEY_W)) {
        logicSpace.y += 2 * deltaTime;
        translateY += 2 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        logicSpace.y -= 2 * deltaTime;
        translateY -= 2 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        logicSpace.x -= 2 * deltaTime;
        translateX -= 2 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        logicSpace.x += 2 * deltaTime;
        translateX += 2 * deltaTime;
    }

    // TODO(student): Zoom in and zoom out logic window with Z and X
    float posx = logicSpace.width / 2 - length / 2 + translateX;
    float posy = logicSpace.height / 2 + translateY;

    if (!CheckColisionsWithObstacles(length, posx, posy)) {
        logicSpace.x = px;
        logicSpace.y = py;
        translateX = trax;
        translateY = tray;
    }

    if (window->KeyHold(GLFW_KEY_Z)) {
        logicSpace.width -= deltaTime;
        logicSpace.height -= deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_X)) {
        logicSpace.width += deltaTime;
        logicSpace.height += deltaTime;
    }
}

int Tema1::CheckColisionsWithObstacles(float length, float posx, float posy) {
    int posObstX, posObstY;

    for (int i = 0; i < obstaclesXPos->size(); i++) {
        posObstX = obstaclesXPos->at(i);
        posObstY = obstaclesYPos->at(i);
        if ((posx + length >= posObstX && posx <= posObstX + lengthO) &&
            (posy + length >= posObstY && posy <= posObstY + lengthO)) {

            return 0;
        }
    }

    for (int i = 0; i < wallXPos->size(); i++) {
        posObstX = wallXPos->at(i);
        posObstY = wallYPos->at(i);
        if ((posx + length >= posObstX && posx <= posObstX + 10 * lengthO) &&
            (posy + length >= posObstY && posy <= posObstY + 10 * lengthO)) {

            return 0;
        }
    }

    return 1;
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
    if (shootingTimer > 0.3) {
        projectileInitialPosX->push_back(logicSpace.width / 2 + translateX);
        projectileInitialPosY->push_back(logicSpace.height / 2 + length / 2 + translateY);
        projectilesAngle->push_back(angularStep);
        projectilesXPos->push_back(logicSpace.width / 2 + translateX);
        projectilesYPos->push_back(logicSpace.height / 2 + length / 2 + translateY);

        shootingTimer = 0;
    }


}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
