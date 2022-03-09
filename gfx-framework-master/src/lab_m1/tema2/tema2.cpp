#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <time.h>
#include <cmath>
#include "lab_m1/tema2/Maze.h"

using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    firstPersonPerspecive = false;
    renderCameraTarget = true;
    characterRotationAngle = 0;
    hudOxAngle = 0;
    life = 1;
    playerMembersRotationAngle = 0;
    sign = 1;
    timeToMove = 1;
    timeOfStart = time(NULL);

    Maze *myMaze = new Maze();
    enemiesArray = *(new std::vector<Enemy>());
    bullets_array = *(new std::vector<bullet_struct*>());

    maze = (int **)malloc(sizeof(int*) * N);
    for (int i = 0; i < N; i++) {
        maze[i] = (int *)malloc(sizeof(int) * N);
    }
    myMaze->getMaze(maze);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (maze[i][j] == 3) {
                Enemy newEnemy = *(new Enemy(glm::vec3(i * 3, 0.5f, j * 3)));
                enemiesArray.push_back(newEnemy);
            }
        }
    }

    camera = new implemented::Tema2_camera();
    px = myMaze->initialXPos;
    py = myMaze->initialYPos;
    camera->Set(glm::vec3(3 * px, 0.5, 3 * py), glm::vec3(3 * px, 1, 0), glm::vec3(0, 1, 0));
    px = 0;
    py = 0;

    camera->TranslateForward(-camera->distanceToTarget);

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        mesh->UseMaterials(true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }


    // Create a shader program for drawing with noise function
    {
        Shader* shader = new Shader("HomeworkShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    projectionMatrix = glm::perspective(RADIANS(fieldOfView), window->props.aspectRatio, 0.01f, 200.0f);
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    RenderMap(deltaTimeSeconds);

    if (camera->GetPosition().x >= 3 * N && !gameOver) {
        gameOver = true;
        std::cout << "You Won";
    }

    // Render the camera target. This is useful for understanding where
    // the rotation point is, when moving in third-person camera mode.
    if (renderCameraTarget)
    {
        RenderCharacter(deltaTimeSeconds);
    }
    else {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.13f));
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        projectionMatrix = glm::ortho(2.f, -2.f, -40.f, 40.f, 1.f, 200.f);
        RenderSimpleMesh(meshes["sphere"], shaders["HomeworkShader"], modelMatrix, glm::vec3(1, 1, 1), 0);
        projectionMatrix = glm::ortho(60.f, -60.f, -2.f, 2.f, 1.f, 200.f);
        RenderSimpleMesh(meshes["sphere"], shaders["HomeworkShader"], modelMatrix, glm::vec3(1, 1, 1), 0);
        projectionMatrix = glm::perspective(RADIANS(fieldOfView), window->props.aspectRatio, 0.01f, 200.f);
    }

    // black frame for time bar
    {
        projectionMatrix = glm::ortho(1.2f, -1.2f, -4.f, 60.f, 1.f, 200.f);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, hudOxAngle, glm::vec3(1, 0, 0));

        glm::vec3 color = glm::vec3(.207, .282f, .286f);
        RenderSimpleMesh(meshes["quad"], shaders["HomeworkShader"], modelMatrix, color, 0);
        projectionMatrix = glm::perspective(RADIANS(fieldOfView), window->props.aspectRatio, 0.01f, 200.f);
    }

    // time bar
    {
        if (!gameOver) {
            timeLeft = std::max((60.f - (time(NULL) - timeOfStart)) / 60.f, 0.f);

            if (timeLeft == 0 || life <= 0) {
                std::cout << "You Lose!";
                gameOver = true;
            }
        }

        projectionMatrix = glm::ortho(1.2f, -1.2f, -4.f, 60.f, 1.1f, 200.f);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, hudOxAngle, glm::vec3(1, 0, 0));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(1.f - timeLeft, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(timeLeft, 1.f, 1.f));

        glm::vec3 color = glm::vec3(0, 1, 0);
        RenderSimpleMesh(meshes["quad"], shaders["HomeworkShader"], modelMatrix, color, 0);
        
        projectionMatrix = glm::perspective(RADIANS(fieldOfView), window->props.aspectRatio, 0.01f, 200.f);
    }

    // black frame for lifebar
    {
        projectionMatrix = glm::ortho(1.2f, -1.2f, -2.f, 65.f, 1.f, 200.f);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, hudOxAngle, glm::vec3(1, 0, 0));

        glm::vec3 color = glm::vec3(.207, .282f, .286f);
        RenderSimpleMesh(meshes["quad"], shaders["HomeworkShader"], modelMatrix, color, 0);
        projectionMatrix = glm::perspective(RADIANS(fieldOfView), window->props.aspectRatio, 0.01f, 200.f);
    }

    // lifebar
    {
        projectionMatrix = glm::ortho(1.2f, -1.2f, -2.f, 65.f, 1.1f, 200.f);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, hudOxAngle, glm::vec3(1, 0, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1.f - life, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(life, 1.f, 1.f));

        RenderMesh(meshes["quad"], shaders["VertexNormal"], modelMatrix);
        projectionMatrix = glm::perspective(RADIANS(fieldOfView), window->props.aspectRatio, 0.01f, 200.f);
    }

    RenderProjectile(deltaTimeSeconds);
}

void Tema2::RenderCharacter(float deltaTimeSeconds) {

    if (pressed) {
        if (timeToMove > 1) {
            if (playerMembersRotationAngle >= M_PI / 4 || playerMembersRotationAngle <= -M_PI / 4) {
                sign *= -1;
            }
            timeToMove -= .1f;
            playerMembersRotationAngle += window->KeyHold(GLFW_KEY_LEFT_SHIFT) ? .5 * sign : .3 * sign;
        }

        timeToMove += deltaTimeSeconds;
    }

    glm::mat4 modelMatrix;
    glm::vec3 offset;
    glm::vec3 offFromGround = glm::vec3(0, .5f, 0);

    // body
    {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + offFromGround);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(.5f, .8f, .5f));
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        glm::vec3 color = glm::vec3(10.f / 255, 81.f / 255, 99.f / 255);
        RenderSimpleMesh(meshes["box"], shaders["HomeworkShader"], modelMatrix, color, 0);
    }
    // head
    {
        offset = glm::vec3(0, .6f, 0);
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + offset + offFromGround);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(.3f, .3f, .3f));
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        glm::vec3 color = glm::vec3(106.f / 255, 101.f / 255, 69.f / 255);
        RenderSimpleMesh(meshes["box"], shaders["HomeworkShader"], modelMatrix, color, 0);
    }
    // hand left
    {
        offset = glm::vec3(-0.35f, 0, 0);
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, offset + offFromGround);

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, .35, 0));
        modelMatrix = glm::rotate(modelMatrix, playerMembersRotationAngle, glm::vec3(1, 0, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -.35, 0));

        modelMatrix = glm::scale(modelMatrix, glm::vec3(.2f, .7f, .1f));
        glm::vec3 color = glm::vec3(10.f / 255, 81.f / 255, 99.f / 255);
        RenderSimpleMesh(meshes["box"], shaders["HomeworkShader"], modelMatrix, color, 0);
    }
    // hand right
    {
        offset = glm::vec3(+0.35f, 0, 0);
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, offset + offFromGround);

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, .35, 0));
        modelMatrix = glm::rotate(modelMatrix, -playerMembersRotationAngle, glm::vec3(1, 0, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -.35, 0));

        modelMatrix = glm::scale(modelMatrix, glm::vec3(.2f, .7f, .1f));
        glm::vec3 color = glm::vec3(10.f / 255, 81.f / 255, 99.f / 255);
        RenderSimpleMesh(meshes["box"], shaders["HomeworkShader"], modelMatrix, color, 0);
    }
    // leg left
    {
        offset = glm::vec3(-0.12f, -0.7f, 0);
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, offset + offFromGround);

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, .35, 0));
        modelMatrix = glm::rotate(modelMatrix, -playerMembersRotationAngle, glm::vec3(1, 0, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -.35, 0));

        modelMatrix = glm::scale(modelMatrix, glm::vec3(.3f, .7f, .3f));
        glm::vec3 color = glm::vec3(49.f / 255, 4.f / 255, 73.f / 255);
        RenderSimpleMesh(meshes["box"], shaders["HomeworkShader"], modelMatrix, color, 0);
    }
    // leg right
    {
        offset = glm::vec3(+0.12f, -0.7f, 0);
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::rotate(modelMatrix, characterRotationAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, offset + offFromGround);

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, .35, 0));
        modelMatrix = glm::rotate(modelMatrix, playerMembersRotationAngle, glm::vec3(1, 0, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -.35, 0));

        modelMatrix = glm::scale(modelMatrix, glm::vec3(.3f, .7f, .3f));
        glm::vec3 color = glm::vec3(49.f / 255, 4.f / 255, 73.f / 255);
        RenderSimpleMesh(meshes["box"], shaders["HomeworkShader"], modelMatrix, color, 0);
    }
}

void Tema2::RenderProjectile(float deltaTimeSeconds) {

    for (int i = 0; i < bullets_array.size(); i++) {
        bullet_struct *bullet = bullets_array[i];
        if (bullet->isActive) {
            glm::mat4 modelMatrix = glm::mat4(1);
            bullet->posX += (glm::normalize(bullet->forward) * deltaTimeSeconds * 10.f)[0];
            bullet->posY += (glm::normalize(bullet->forward) * deltaTimeSeconds * 10.f)[1];
            bullet->posZ += (glm::normalize(bullet->forward) * deltaTimeSeconds * 10.f)[2];
            modelMatrix = glm::translate(modelMatrix, glm::vec3(bullet->posX, bullet->posY, bullet->posZ));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
            RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);

            for (int i = 0; i < enemiesArray.size(); i++) {
                if (enemiesArray.at(i).isActive)
                    if (sqrt(pow(bullet->posX - enemiesArray[i].GetPosition()[0], 2) +
                        pow(bullet->posZ - enemiesArray[i].GetPosition()[2], 2)) < 0.45f &&
                        bullet->posY > 0 && bullet->posY < 1) {
                        enemiesArray.at(i).isActive = false;
                        bullet->isActive = false;
                    }
            }

            bullet->ttl -= deltaTimeSeconds;
            if (bullet->ttl <= 0) {
                bullet->isActive = false;
            }
            
        }
    }
}

void Tema2::RenderMap(float deltaTimeSeconds) {

    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(30.f));
    RenderMesh(meshes["plane"], shaders["Simple"], modelMatrix);

    // render obstacles
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (maze[i][j] == 1) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(3 * i, 1.5f, 3 * j));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(3.f));

                RenderSimpleMesh(meshes["box"], shaders["HomeworkShader"], modelMatrix, glm::vec3(79.f / 255, 92.f / 255, 69.f / 255), 0);
            }
        }
    }

    // render enemies
    {
        glm::vec3 cameraPos = firstPersonPerspecive ? camera->GetPosition() : camera->GetTargetPosition();
        for (int i = 0; i < enemiesArray.size(); i++) {
            if (enemiesArray.at(i).isActive) {
                enemiesArray.at(i).SetNewPosition(deltaTimeSeconds);

                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, enemiesArray.at(i).GetPosition());
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f));

                RenderSimpleMesh(meshes["sphere"], shaders["HomeworkShader"], modelMatrix, glm::vec3(1, 0, 1), 0.1f);


                if (sqrt(
                    pow(enemiesArray.at(i).GetPosition()[0] - cameraPos[0], 2) +
                    pow(enemiesArray.at(i).GetPosition()[2] - cameraPos[2], 2)) < 1.f) {
                    enemiesArray.at(i).isActive = false;
                    if (!gameOver) {
                        life -= .1f;
                    }

                }
            }
            else {
                if (enemiesArray.at(i).GetTime() > 0) {
                    enemiesArray.at(i).DecreaseTime();

                    glm::mat4 modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, enemiesArray.at(i).GetPosition());
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f));

                    RenderSimpleMesh(meshes["sphere"], shaders["HomeworkShader"], modelMatrix, glm::vec3(1, 0, 1), rand() % 1000);
                }
            }
        }
    }
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

// set time to 0 if there is no need for animation
void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, float time)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glUniform1f(glGetUniformLocation(shader->program, "time"), time);
    // set color
    glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));

    // Draw the object
    mesh->Render();
}

void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    float cameraSpeed = 4.0f;
    if (window->KeyHold(GLFW_KEY_LEFT_SHIFT)) {
        cameraSpeed = 10.f;
    }
    pressed = false;
    {
        glm::vec3 oldPos = camera->GetPosition();

        if (window->KeyHold(GLFW_KEY_W) || window->KeyHold(GLFW_KEY_UP)) {
            
            camera->MoveForward(deltaTime * cameraSpeed);
            if (checkWallColision()) {
                camera->MoveForward(-deltaTime * cameraSpeed);
            }
            pressed = true;
        }

        if (window->KeyHold(GLFW_KEY_A) || window->KeyHold(GLFW_KEY_LEFT)) {
            
            camera->TranslateRight(-deltaTime * cameraSpeed);
            if (checkWallColision()) {
                camera->TranslateRight(deltaTime * cameraSpeed);
            }
            pressed = true;
        }

        if (window->KeyHold(GLFW_KEY_S) || window->KeyHold(GLFW_KEY_DOWN)) {
            
            camera->MoveForward(-deltaTime * cameraSpeed);
            if (checkWallColision()) {
                camera->MoveForward(deltaTime * cameraSpeed);
            }
            pressed = true;
        }

        if (window->KeyHold(GLFW_KEY_D) || window->KeyHold(GLFW_KEY_RIGHT)) {
            
            camera->TranslateRight(deltaTime * cameraSpeed);
            if (checkWallColision()) {
                camera->TranslateRight(-deltaTime * cameraSpeed);
            }
            pressed = true;
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            
            camera->TranslateUpward(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            
            camera->TranslateUpward(deltaTime * cameraSpeed);
        }

    }

    if (!pressed) {
        playerMembersRotationAngle = 0;
    }

}

bool Tema2::checkWallColision() {

    int x = camera->GetTargetPosition()[0];
    int y = camera->GetTargetPosition()[2];
    int height = camera->GetTargetPosition()[1];

    if (firstPersonPerspecive) {
        x = camera->GetPosition()[0];
        y = camera->GetPosition()[2];
        height = camera->GetPosition()[1];
    }

    if (height >= 4) {
        return false;
    }

    if ((int)(std::round(x + 1) / 3) >= 0 &&
        (int)(std::round(x + 2) / 3) < N &&
        (int)(std::round(y + 1) / 3) >= 0 &&
        (int)(std::round(y + 2) / 3) < N) {

        bool a = (maze
            [(int)(std::round(x + 1) / 3)]
            [(int)(std::round(y + 1) / 3)] == 1);
        bool b = (maze
            [(int)(std::round(x + 2) / 3)]
            [(int)(std::round(y + 2) / 3)] == 1);
        
        if (a || b) {
    
            return true;
        }
    }

    return false;

}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }

    if (key == GLFW_KEY_LEFT_CONTROL)
    {
        firstPersonPerspecive = !firstPersonPerspecive;

        if (firstPersonPerspecive) {
            camera->TranslateForward(camera->distanceToTarget);
            renderCameraTarget = false;
            camera->TranslateUpward(1);
        }
        else
        {
            renderCameraTarget = true;
            camera->TranslateForward(-camera->distanceToTarget);
            camera->TranslateUpward(-1);
        }
    }

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (firstPersonPerspecive) {
   
            camera->RotateFirstPerson_OX(-deltaY * sensivityOX);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOY);

        }

        if (!firstPersonPerspecive) {
            
            camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOY);

        }
        characterRotationAngle -= deltaX * sensivityOY;
        hudOxAngle -= deltaY * sensivityOX;
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (firstPersonPerspecive && button == 1) {

        bullet_struct* bullet = (bullet_struct *)calloc(sizeof(bullet_struct), 1);

        if (bullet != NULL) {
            bullet->posX = camera->GetPosition()[0];
            bullet->posY = camera->GetPosition()[1];
            bullet->posZ = camera->GetPosition()[2];
            bullet->incZ = 1;
            bullet->incX = 0;
            bullet->incY = 0;
            bullet->forward = glm::normalize(camera->GetTargetPosition() - camera->GetPosition());
            bullet->isActive = true;
            bullet->ttl = 3;

            bullets_array.push_back(bullet);
        }
    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
