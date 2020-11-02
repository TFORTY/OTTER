//INFR 1350U Intro to Computer Graphics Practical Midterm
//Option 2: "Arkanoid/Brick Breaker"
//Aiden Gimpel - 100740094
//Tyler Wong - 100750805

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> 
#include <string> 
#include <sstream>
#include <vector>

#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <GLM/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Entity.h"
#include "Transform.h"
#include "InputHelpers.h"
#include "Texture2D.h"
#include "Texture2DData.h"

GLFWwindow* window;

bool initGLFW() {
    if (glfwInit() == GLFW_FALSE) {
        std::cout << "Failed to Initialize GLFW" << std::endl;
        return false;
    }

    //Create a new GLFW window
    window = glfwCreateWindow(1000, 800, "CG Midterm", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    return true;
}

bool initGLAD() {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
        std::cout << "Failed to initialize Glad" << std::endl;
        return false;
    }
}


const float PADDLE_SPEED = 2.0f;
const float PADDLE_BOUNDS = 2.3f;

const float ballBoxX = 0.1f;
const float ballBoxY = 0.2f;
const float paddleBoxX = 0.73f;
const float paddleBoxY = 0.1f;
const float wallBounds = 2.9f;
const float roofBounds = 4.85f;
const float brickBoxY = 0.125f;
const float brickBoxX = 0.25f;

bool win = false;
bool lose = false;
bool canMove = false;

//Function for player movement
void PlayerInput(Transform& trans, float dt)
{
    //Player Movement
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && canMove) {
        trans.SetPositionX(trans.GetPositionX() - PADDLE_SPEED * dt);
        if (trans.GetPositionX() <= -PADDLE_BOUNDS)
        {
            trans.SetPositionX(-PADDLE_BOUNDS);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && canMove) {
        trans.SetPositionX(trans.GetPositionX() + PADDLE_SPEED * dt);
        if (trans.GetPositionX() >= PADDLE_BOUNDS)
        {
            trans.SetPositionX(PADDLE_BOUNDS);
        }
    }
}

void RenderVAO(Shader::sptr& shader, Mesh& vao, Camera& camera, glm::mat4 transform)
{
    shader->SetUniformMatrix("u_ModelRotation", glm::mat3(transform));
    shader->SetUniformMatrix("u_ModelViewProjection", camera.GetViewProjection() * transform);
    shader->SetUniformMatrix("u_Model", transform);
    vao.Render();
}

void CloseGameInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void SetShaderValues(Shader::sptr& shader, glm::vec3 lightPos = glm::vec3(0.f, 0.f, 0.f), glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f), float lightAmbientPow = 0.f,
    float lightSpecularPow = 0.f, glm::vec3 ambientCol = glm::vec3(0.f, 0.f, 0.f), float ambientPow = 0.f, float shininess = 0.f)
{
    shader->SetUniform("u_LightPos", lightPos);
    shader->SetUniform("u_LightCol", lightCol);
    shader->SetUniform("u_AmbientLightStrength", lightAmbientPow);
    shader->SetUniform("u_SpecularLightStrength", lightSpecularPow);
    shader->SetUniform("u_AmbientCol", ambientCol);
    shader->SetUniform("u_AmbientStrength", ambientPow);
    shader->SetUniform("u_Shininess", shininess);
}

struct Material
{
    Texture2D::sptr Albedo;
    Texture2D::sptr Specular;
    float           Shininess;
};

//MAIN//
int main()
{
    Logger::Init();

    //Initialize GLFW
    if (!initGLFW())
        return 1;

    //Initialize GLAD
    if (!initGLAD())
        return 1;

    glEnable(GL_TEXTURE_2D);

    //Background Plane
    auto background = Entity::Create();
    Mesh bgMesh = background.Add<Mesh>("GameObjects/TestPlane.obj");

    //Paddle Entity and components
    auto playerEnt = Entity::Create();   
    Mesh playerMesh = playerEnt.Add<Mesh>("GameObjects/Paddle.obj");   
  
    Transform paddleTrans = playerEnt.Add<Transform>();

    paddleTrans.SetRotationY(90.0f);
    paddleTrans.SetPosition(glm::vec3(0.0f, -2.0f, 1.0f));
      
    //Ball Entity and components
    auto ballEnt = Entity::Create();
    Mesh ballMesh = ballEnt.Add<Mesh>("GameObjects/Ball.obj");
    Transform ballTrans = ballEnt.Add<Transform>();

    ballTrans.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));

    //Game over entity
    auto loseEnt = Entity::Create();
    Mesh loseMesh = loseEnt.Add<Mesh>("Text/GameOver.obj");

    //Win entity
    auto winEnt = Entity::Create();
    Mesh winMesh = winEnt.Add<Mesh>("Text/Winner.obj");

    Mesh life0("Text/Lives0.obj");
    Mesh life1("Text/Lives1.obj");
    Mesh life2("Text/Lives2.obj");
    Mesh life3("Text/Lives3.obj");

    //Wall Entities
    auto roofEnt = Entity::Create();
    Mesh roofMesh = roofEnt.Add<Mesh>("GameObjects/Wall.obj");

    //Loads in the score objects to be displayed
    std::vector<Mesh> scoreMeshes;
    for (int i = 0; i < 49; ++i)
    {
        std::stringstream ss;
        ss << "Scores/Score" << i << ".obj";
        std::string name;
        ss >> name; 
        Mesh temp(name);
        scoreMeshes.push_back(temp);
    }

    int playerLives = 3;
    int score = 0;

    //Brick Entitiy(s)
    bool isAlive[36];

    for (int i = 0; i < 36; ++i)
    {
        isAlive[i] = true;
    }

    int numLives[36] = { 1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1 };

    Entity bricks[36];
    Mesh brickMesh("GameObjects/Brick.obj");
    std::vector<Mesh> meshes;
    std::vector<Transform> transforms;

    for (int i = 0; i < 36; i++)
    {      
        auto& temp2 = bricks[i].Add<Mesh>(brickMesh);
        meshes.push_back(temp2);      
        Transform tempTrans = bricks[i].Add<Transform>();
        transforms.push_back(tempTrans);

        transforms[i].SetScale(glm::vec3(0.25f));
        transforms[i].SetRotationX(-45.0f);
    }

    transforms[0].SetPosition(glm::vec3(-2.75f, 3.0f, 1.0f));
    transforms[12].SetPosition(glm::vec3(-2.75f, 2.6f, 1.0f));
    transforms[24].SetPosition(glm::vec3(-2.75f, 2.2f, 1.0f));

    for (int i = 1; i < 12; ++i)
    {
        transforms[i].SetPosition(glm::vec3(transforms[i-1].GetPositionX() + 0.5f, transforms[i-1].GetPositionY(), 1.0f));
    }
    for (int i = 13; i < 24; ++i)
    {
        transforms[i].SetPosition(glm::vec3(transforms[i-1].GetPositionX() + 0.5f, transforms[i-1].GetPositionY(), 1.0f));
    }
    for (int i = 25; i < 36; ++i)
    {
        transforms[i].SetPosition(glm::vec3(transforms[i-1].GetPositionX() + 0.5f, transforms[i-1].GetPositionY(), 1.0f));
    }

#pragma region Shader Stuff
    Shader::sptr shader = Shader::Create();
    shader->LoadShaderPartFromFile("Shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
    shader->LoadShaderPartFromFile("Shaders/frag_shader.glsl", GL_FRAGMENT_SHADER);
    shader->Link();

    SetShaderValues(shader, glm::vec3(0.f, 5.f, 5.f), glm::vec3(1.0f), 0.0f, 0.f, glm::vec3(0.56f, 0.93f, 0.56f), 0.9f, 6.f);

    Shader::sptr ballShader = Shader::Create();
    ballShader->LoadShaderPartFromFile("Shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
    ballShader->LoadShaderPartFromFile("Shaders/frag_textured_shader.glsl", GL_FRAGMENT_SHADER);
    ballShader->Link();

    SetShaderValues(ballShader, glm::vec3(0.f, 0.f, 2.f), glm::vec3(1.f, 1.f, 1.f), 0.5f, 0.f, glm::vec3(1.0f), 1.0f, 8.f);

    float lightLinearFalloff = 0.09f;
    float lightQuadraticFalloff = 0.032f;

    ballShader->SetUniform("u_LightAttenuationConstant", 1.0f);
    ballShader->SetUniform("u_LightAttenuationLinear", lightLinearFalloff);
    ballShader->SetUniform("u_LightAttenuationQuadratic", lightQuadraticFalloff);

    Shader::sptr paddleShader = Shader::Create();
    paddleShader->LoadShaderPartFromFile("Shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
    paddleShader->LoadShaderPartFromFile("Shaders/frag_textured_shader.glsl", GL_FRAGMENT_SHADER);
    paddleShader->Link();

    SetShaderValues(paddleShader, glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.0f, 1.0f, 1.0f), 0.7f, 0.5f, glm::vec3(1.0f), 0.1f, 8.f);

    paddleShader->SetUniform("u_LightAttenuationConstant", 1.0f);
    paddleShader->SetUniform("u_LightAttenuationLinear", lightLinearFalloff);
    paddleShader->SetUniform("u_LightAttenuationQuadratic", lightQuadraticFalloff);

    Shader::sptr brickShader = Shader::Create();
    brickShader->LoadShaderPartFromFile("Shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
    brickShader->LoadShaderPartFromFile("Shaders/frag_textured_shader.glsl", GL_FRAGMENT_SHADER);
    brickShader->Link();

    SetShaderValues(brickShader, glm::vec3(0.f, 0.f, 2.f), glm::vec3(1.f, 1.f, 1.f), 0.5f, 0.f, glm::vec3(1.0f), 1.0f, 8.f);

    brickShader->SetUniform("u_LightAttenuationConstant", 1.0f);
    brickShader->SetUniform("u_LightAttenuationLinear", lightLinearFalloff);
    brickShader->SetUniform("u_LightAttenuationQuadratic", lightQuadraticFalloff);

    Shader::sptr brickShader2 = Shader::Create();
    brickShader2->LoadShaderPartFromFile("Shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
    brickShader2->LoadShaderPartFromFile("Shaders/frag_textured_shader.glsl", GL_FRAGMENT_SHADER);
    brickShader2->Link();

    SetShaderValues(brickShader2, glm::vec3(0.f, 0.f, 2.f), glm::vec3(1.f, 1.f, 1.f), 0.5f, 0.f, glm::vec3(1.0f), 1.0f, 8.f);

    brickShader2->SetUniform("u_LightAttenuationConstant", 1.0f);
    brickShader2->SetUniform("u_LightAttenuationLinear", lightLinearFalloff);
    brickShader2->SetUniform("u_LightAttenuationQuadratic", lightQuadraticFalloff);
#pragma endregion

    glEnable(GL_DEPTH_TEST);

#pragma region Texture Stuff
    Texture2DData::sptr paddleMap = Texture2DData::LoadFromFile("Textures/paddleTex.png");
    Texture2DData::sptr ballMap = Texture2DData::LoadFromFile("Textures/BallTex.png");
    Texture2DData::sptr brickMap = Texture2DData::LoadFromFile("Textures/BrickTex.png");
    Texture2DData::sptr brickMap2 = Texture2DData::LoadFromFile("Textures/CrackedBrick.png");

    Texture2D::sptr paddleDif = Texture2D::Create();
    paddleDif->LoadData(paddleMap);

    Texture2D::sptr brickDif = Texture2D::Create();
    brickDif->LoadData(brickMap);

    Texture2D::sptr brickDif2 = Texture2D::Create();
    brickDif2->LoadData(brickMap2);

    Texture2D::sptr ballDif = Texture2D::Create();
    ballDif->LoadData(ballMap);

    Texture2DDescription desc = Texture2DDescription();
    desc.Width = 1;
    desc.Height = 1;
    desc.Format = InternalFormat::RGB8;
    Texture2D::sptr texture2 = Texture2D::Create(desc);
    texture2->Clear();

    Material paddleMat;
    paddleMat.Albedo = paddleDif;
    paddleMat.Shininess = 64.0f;

    Material ballMat;
    ballMat.Albedo = ballDif;
    ballMat.Shininess = 64.0f;

    Material brickMat;
    brickMat.Albedo = brickDif;
    brickMat.Shininess = 64.0f;

    Material brickMat2;
    brickMat2.Albedo = brickDif2;
    brickMat2.Shininess = 64.0f;
#pragma endregion

#pragma region Camera Stuff
    //Camera Entity
    auto camEntity = Entity::Create();
    Camera camera = camEntity.Add<Camera>();

    camera.SetPosition(glm::vec3(0, 3, 3)); // Set initial position
    camera.SetUp(glm::vec3(0, 0, -1)); // Use a z-up coordinate system
    camera.LookAt(glm::vec3(0.0f)); // Look at center of the screen
    camera.SetFovDegrees(90.0f); // Set an initial FOV
#pragma endregion
   
    double lastFrame = glfwGetTime();
    float BALL_SPEED_X = 0.0f;
    float BALL_SPEED_Y = 0.0f;  
    float rotS = 0.0f;

    glm::mat4 transformBricks[36];
    glm::mat4 transformScore = glm::mat4(1.0f);
    glm::mat4 translateScore = glm::translate(transformScore, glm::vec3(2.0f, 4.0f, 1.0f));
    
    /////////////
    //GAME LOOP//
    /////////////
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.98f, 0.5f, 0.45f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double thisFrame = glfwGetTime();
        float dt = static_cast<float>(thisFrame - lastFrame);
     
        rotS = rotS + 0.1;

        CloseGameInput(window);

        //Basic Model Matrix
        glm::mat4 baseTransform = glm::mat4(1.0f);

        //Our player's model matrix
        glm::mat4 transformPlayer = paddleTrans.GetModelMatrix();
        paddleTrans.SetRotationX(glm::radians(-45.0f));
        //Our ball's model matrix
        glm::mat4 transformBall = ballTrans.GetModelMatrix(); 
        
        if (score >= 48)
        {
            win = true;
            paddleTrans.SetPositionX(0.0f);
            ballTrans.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
            canMove = false;
        }

        if (playerLives <= 0)
        {
            lose = true;
            paddleTrans.SetPositionX(0.0f);
            ballTrans.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
            canMove = false;
        }

        for (int i = 0; i < 36; ++i)
        {
            transformBricks[i] = transforms[i].GetModelMatrix();
        }

        //Our roof's transform
        glm::mat4 transformRoofMove = glm::translate(baseTransform, glm::vec3(0.0f, 5.2f, 1.0f));
        glm::mat4 transformRoofRot = glm::rotate(transformRoofMove, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));  

        //Transforms for the score
        glm::mat4 scoreRotate = glm::rotate(translateScore, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 scoreSpin = glm::rotate(scoreRotate, glm::radians(rotS), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 scoreFinal = glm::scale(scoreSpin, glm::vec3(4.0f, 4.5f, 4.0f));

        //Transformations for Game over and Win
        glm::mat4 gameRot = glm::rotate(baseTransform, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 gameSpin = glm::rotate(gameRot, glm::radians(rotS), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 lifeTrans = glm::translate(baseTransform, glm::vec3(-2.0f, 4.0f, 1.0f));
        glm::mat4 lifeRot = glm::rotate(lifeTrans, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lifeScale = glm::scale(lifeRot, glm::vec3(0.5f));

        PlayerInput(paddleTrans, dt);

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && (!lose && !win))
        {
            if (BALL_SPEED_X == 0.0f && BALL_SPEED_Y == 0.0f)
            {
                canMove = true;
                BALL_SPEED_Y = -2.5f;
            }
        }

        //Restarts Game When it ends
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (lose || win))
        {
            playerLives = 3;
            score = 0;
            win = false;
            lose = false;
            canMove = false;
            BALL_SPEED_X = 0.0f;
            BALL_SPEED_Y = 0.0f;

            ballTrans.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
            paddleTrans.SetPositionX(0.0f);

            for (int i = 0; i < 36; ++i)
            {
                isAlive[i] = true;
                if (i < 12 || i > 23)
                    numLives[i] = 1;
                else
                    numLives[i] = 2;
            }
        }

        ballTrans.SetPosition(glm::vec3(ballTrans.GetPositionX() + BALL_SPEED_X * dt, ballTrans.GetPositionY() + BALL_SPEED_Y  * dt, 1.0f));

#pragma region CollisionDetection
        //Ball on Paddle Collision
       if ((ballTrans.GetPositionY() - ballBoxY <= paddleTrans.GetPositionY() + paddleBoxY) && (ballTrans.GetPositionY() - ballBoxY >= paddleTrans.GetPositionY()) &&
            (ballTrans.GetPositionX() + ballBoxX > paddleTrans.GetPositionX() - paddleBoxX) && (ballTrans.GetPositionX() - ballBoxX < paddleTrans.GetPositionX() + paddleBoxX))
        {
           ballTrans.SetPositionY(-1.69f);
            BALL_SPEED_Y = 1.5f;

            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= -0.8f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < -0.7f)
            {
                BALL_SPEED_X = -1.5f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= -0.7f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < -0.6f)
            {
                BALL_SPEED_X = -1.3f;
                BALL_SPEED_Y = BALL_SPEED_Y + 0.3f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= -0.6f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < -0.5f)
            {
                BALL_SPEED_X = -1.1f;
                BALL_SPEED_Y = BALL_SPEED_Y + 0.6f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= -0.5f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < -0.4f)
            {
                BALL_SPEED_X = -0.9f;
                BALL_SPEED_Y = BALL_SPEED_Y + 0.9f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= -0.4f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < -0.3f)
            {
                BALL_SPEED_X = -0.7f;
                BALL_SPEED_Y = BALL_SPEED_Y + 1.2f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= -0.3f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < -0.2f)
            {
                BALL_SPEED_X = -0.5f;
                BALL_SPEED_Y = BALL_SPEED_Y + 1.5f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= -0.2f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < -0.1f)
            {
                BALL_SPEED_X = -0.3f;
                BALL_SPEED_Y = BALL_SPEED_Y + 1.8f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= -0.1f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < 0.0f)
            {
                BALL_SPEED_X = -0.1f;
                BALL_SPEED_Y = BALL_SPEED_Y + 2.1f;

            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= 0.0f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < 0.1f)
            {
                BALL_SPEED_X = 0.1f;
                BALL_SPEED_Y = BALL_SPEED_Y + 2.1f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= 0.1f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < 0.2f)
            {
                BALL_SPEED_X = 0.3f;
                BALL_SPEED_Y = BALL_SPEED_Y + 1.8f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= 0.2f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < 0.3f)
            {
                BALL_SPEED_X = 0.5f;
                BALL_SPEED_Y = BALL_SPEED_Y + 1.5f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= 0.3f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < 0.4f)
            {
                BALL_SPEED_X = 0.7f;
                BALL_SPEED_Y = BALL_SPEED_Y + 1.2f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= 0.4f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < 0.5f)
            {
                BALL_SPEED_X = 0.9f;
                BALL_SPEED_Y = BALL_SPEED_Y + 0.9f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= 0.5f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < 0.6f)
            {
                BALL_SPEED_X = 1.1f;
                BALL_SPEED_Y = BALL_SPEED_Y + 0.6f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= 0.6f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < 0.7f)
            {
                BALL_SPEED_X = 1.3f;
                BALL_SPEED_Y = BALL_SPEED_Y + 0.3f;
            }
            if (ballTrans.GetPositionX() - paddleTrans.GetPositionX() >= 0.7f && ballTrans.GetPositionX() - paddleTrans.GetPositionX() < 0.8f)
            {
                BALL_SPEED_X = 1.5f;

            }
        }

       //Top Wall Collision
       if (ballTrans.GetPositionY() >= roofBounds)
       {
            ballTrans.SetPositionY(4.84f);
            BALL_SPEED_Y = -BALL_SPEED_Y;
       }

        //Left Wall and Right Wall Collision
       if (ballTrans.GetPositionX() >= wallBounds || ballTrans.GetPositionX() <= -wallBounds)
       {
           if (ballTrans.GetPositionX() >= wallBounds)
               ballTrans.SetPositionX(2.89f);
           if (ballTrans.GetPositionX() <= -wallBounds)
               ballTrans.SetPositionX(-2.89f);

            BALL_SPEED_X = -BALL_SPEED_X;
       }


        for (int i = 0; i < 36; ++i)
        {
            if (BALL_SPEED_Y > 0.0f && ballTrans.GetPositionY() + ballBoxY >= transforms[i].GetPositionY() - brickBoxY
                && ballTrans.GetPositionY() + ballBoxY <= transforms[i].GetPositionY() + brickBoxY
                && ballTrans.GetPositionX() >= transforms[i].GetPositionX() - brickBoxX
                && ballTrans.GetPositionX() <= transforms[i].GetPositionX() + brickBoxX && isAlive[i])
            {
                BALL_SPEED_Y = -BALL_SPEED_Y;
                numLives[i] = numLives[i] - 1;
                score++;
            }

            if (BALL_SPEED_Y < 0.0f && ballTrans.GetPositionY() - ballBoxY <= transforms[i].GetPositionY() + brickBoxY
                && ballTrans.GetPositionY() - ballBoxY >= transforms[i].GetPositionY() - brickBoxY
                && ballTrans.GetPositionX() >= transforms[i].GetPositionX() - brickBoxX
                && ballTrans.GetPositionX() <= transforms[i].GetPositionX() + brickBoxX && isAlive[i])
            {
                BALL_SPEED_Y = -BALL_SPEED_Y;
                numLives[i] = numLives[i] - 1;
                score++;
            }

           if (BALL_SPEED_X > 0.0f && ballTrans.GetPositionX() + ballBoxX >= transforms[i].GetPositionX() - brickBoxX
               && ballTrans.GetPositionX() + ballBoxX <= transforms[i].GetPositionX() + brickBoxX
               && ballTrans.GetPositionY() >= transforms[i].GetPositionY() - brickBoxY
               && ballTrans.GetPositionY() <= transforms[i].GetPositionY() + brickBoxY && isAlive[i])
            {
                BALL_SPEED_X = -BALL_SPEED_X;
                numLives[i] = numLives[i] - 1;
                score++;
            }

           if (BALL_SPEED_X < 0.0f && ballTrans.GetPositionX() - ballBoxX <= transforms[i].GetPositionX() + brickBoxX
               && ballTrans.GetPositionX() - ballBoxX >= transforms[i].GetPositionX() - brickBoxX
               && ballTrans.GetPositionY() >= transforms[i].GetPositionY() - brickBoxY
               && ballTrans.GetPositionY() <= transforms[i].GetPositionY() + brickBoxY && isAlive[i])
           {
               BALL_SPEED_X = -BALL_SPEED_X;
               numLives[i] = numLives[i] - 1;
               score++;
           }
        }
#pragma endregion

        //Death Logic
        if (ballTrans.GetPositionY() <= -3.5f)
        {
            ballTrans.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
            BALL_SPEED_X = 0.0f;
            BALL_SPEED_Y = 0.0f;
            paddleTrans.SetPositionX(0.0f);
            canMove = false;
            playerLives--;
        }
     
        paddleShader->Bind();
        //Renders the first obj in the correct position with rotation
        paddleShader->SetUniform("u_CamPos", camera.GetPosition());

        paddleShader->SetUniform("s_Diffuse", 0);
        paddleMat.Albedo->Bind(0);
        paddleShader->SetUniform("u_Shininess", paddleMat.Shininess);

        RenderVAO(paddleShader, playerMesh, camera, transformPlayer);

        //Renders each brick mesh in its correct location
        for (int i = 0; i < 36; i++)
        {
            if (numLives[i] < 1)
                isAlive[i] = false;

            if (isAlive[i])
            {
                if (numLives[i] == 2)
                {
                    brickShader->Bind();
                    //Renders the first obj in the correct position with rotation
                    brickShader->SetUniform("u_CamPos", camera.GetPosition());

                    brickShader->SetUniform("s_Diffuse", 0);
                    brickMat.Albedo->Bind(0);
                    brickShader->SetUniform("u_Shininess", brickMat.Shininess);

                    RenderVAO(brickShader, meshes[i], camera, transformBricks[i]);
                }
                else if (numLives[i] == 1)
                {
                    brickShader2->Bind();
                    //Renders the first obj in the correct position with rotation
                    brickShader2->SetUniform("u_CamPos", camera.GetPosition());

                    brickShader2->SetUniform("s_Diffuse", 0);
                    brickMat2.Albedo->Bind(0);
                    brickShader2->SetUniform("u_Shininess", brickMat2.Shininess);

                    RenderVAO(brickShader2, meshes[i], camera, transformBricks[i]);
                }
            }
        }

        shader->Bind();
        shader->SetUniform("u_CamPos", camera.GetPosition());

        //Renders the roof
        RenderVAO(shader, roofMesh, camera, transformRoofRot);
        //Renders the score
        RenderVAO(shader, scoreMeshes[score], camera, scoreFinal);
        //Renders the You Win
        if (win)
            RenderVAO(shader, winMesh, camera, gameSpin);
        //Renders the Game Over
        if (lose)
            RenderVAO(shader, loseMesh, camera, gameSpin);

        //Renders the player lives
        if (playerLives == 3)
            RenderVAO(shader, life3, camera, lifeScale);
        else if (playerLives == 2)
            RenderVAO(shader, life2, camera, lifeScale);
        else if (playerLives == 1)
            RenderVAO(shader, life1, camera, lifeScale);
        else if (playerLives == 0)
            RenderVAO(shader, life0, camera, lifeScale);

        ballShader->Bind();
        //Renders the first obj in the correct position with rotation
        ballShader->SetUniform("u_CamPos", camera.GetPosition());

        ballShader->SetUniform("s_Diffuse", 0);
        ballMat.Albedo->Bind(0);
        ballShader->SetUniform("u_Shininess", ballMat.Shininess);
        //Renders the ball
        RenderVAO(ballShader, ballMesh, camera, transformBall);

        glfwSwapBuffers(window);
        lastFrame = thisFrame;
    }

    Logger::Uninitialize();
    return 0;
}