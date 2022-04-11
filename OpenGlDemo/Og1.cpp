#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <conio.h>
#include "myglshader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_cb(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInputWindow(GLFWwindow* window);
void linkAttributePointer();


// settings
GLFWwindow* window;
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
float lastX = 400, lastY = 300;//������ĵ�
float yaw = 0.0f;
float pitch = 0.0f;
float sams = 0.5f;
bool firstMouse = true;

float observer = 0.0f;
//��ͷ�ƶ��ٶȲ���
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

//OpenGL�ĺ���ģʽҪ������ʹ��VAO��������֪������δ������ǵĶ������롣������ǰ�VAOʧ�ܣ�OpenGL��ܾ������κζ�����
unsigned int VAO;//Vertex Arrays Object-�����������.�κ����Ķ������Ե��ö��ᴢ�������VAO
unsigned int VBO;//Vertex Buffer Object-���㻺�����
unsigned int IBO;//�����������,�����������е�λ,�������������⣬������Ⱦ��Դ����

unsigned int lightVAO;

unsigned int vertexShader;//������ɫ��
unsigned int fragmentShader;//Ƭ����ɫ��
unsigned int shaderProgramObject;//��ɫ���������
unsigned int vertexAttribPointer;//���Ӷ�������

//������������������ٷ�ΧͼƬrepeat���٣�(x1-x2)/tx = �ظ���ͼƬ��
//���Ƕ���
//float vertices[] = {
//    //     ---- λ�� ----       ---- ��ɫ ----     - �������� -
//        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // ����
//        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // ����
//       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
//       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ����
//};

float vertices[] = {
    //     ---- λ�� ----      - �������� -
        0.1f,  0.1f, -0.1f,      // 0
        0.1f, -0.1f, -0.1f,      // 1
       -0.1f, -0.1f, -0.1f,      // 2
       -0.1f,  0.1f, -0.1f,      // 3

        0.1f,  0.1f, 0.1f,        // 4
        0.1f, -0.1f, 0.1f,        // 5
       -0.1f, -0.1f, 0.1f,        // 6
       -0.1f,  0.1f, 0.1f,        // 7

        0.1f, -0.1f, 0.1f,       // 8
       -0.1f,  0.1f, 0.1f,        // 9
        0.1f,  0.1f, 0.1f,        // 10
       -0.1f, -0.1f, -0.1f,       // 11
        0.1f, -0.1f, -0.1f,       // 12
       -0.1f, -0.1f, 0.1f,        // 13
       -0.1f,  0.1f, 0.1f,       // 14
        0.1f,  0.1f, 0.1f,       // 15
};

// ����Ҳ��ʹ��ID���õ�
unsigned int texTure;
unsigned int texTure2;

unsigned int indices[] = {
    0,1,3,1,2,3,
    0,15,3,15,7,3,
    0,1,4,1,5,4,
    6,8,9,8,9,10,
    6,8,11,8,11,12,
    2,13,14,2,14,3
    //,1,3,2,0,2,4
};//�����֮��


glm::vec3 cubePositions[] = {   
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(0.2f,  0.5f, -1.5f),
  glm::vec3(-0.15f, -0.22f, -0.25f),
  glm::vec3(-0.35f, -0.2, -1.2f),
  glm::vec3(0.24, -0.04f, -0.35f),
  glm::vec3(-0.17f,  0.03f, -0.75f),
  glm::vec3(0.13f, -0.2f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};//����10��������λ��


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

int success;
char infoLog[512];

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_cb);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    Shader boxShader("D://tempData//light//light-1//boxShader.vs", "D://tempData//light//light-1//boxShader.fs");
    Shader lightingShader("D://tempData//light//light-1//lightShader.vs", "D://tempData//light//light-1//lightShader.fs");
    glm::vec3 boxColor(0.8f, 0.3f, 0.5f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(0.5f, 0.3f, 1.0f);   
    
    linkAttributePointer();
    glEnable(GL_DEPTH_TEST);
    //����������
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    boxShader.use();
    lightingShader.use();
    
     // ����ʹ����ɫ��������
    boxShader.setBoxColor(boxColor);
    boxShader.setLightColors(lightColor);
   
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//���������������
    // render loop
    // -----------
   
    while (!glfwWindowShouldClose(window))
    {
        // outside input��������
        // -----
        processInputWindow(window);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        //int _ch;//fetch ASCII
        
        // render
        // ------
        // �����ɫ����
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
       
        // bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texTure);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texTure2);

        //0.9.9�����ϰ汾glm::mat4 transform = glm::mat4(1.0f);
        //glm::mat4 transform = glm::mat4(1.0f);
        //radians���Ƕ�ת��Ϊ����
        //transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f));
        //transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        // render container
        
        
        //ourShader.setScale(transform);

        
        //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));

        //����z���ƶ�Զ��,�����ӿڴ�С
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

        view =
            glm::lookAt(
                cameraPos,
                cameraPos + cameraFront,
                cameraUp);
        //ͶӰ����
        //1. ��Ұ(Field of View)��
        //2. ��߱�w/h
        //3. ƽ��ͷ��Ľ���Զƽ�� ������Ϊ0.1f
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
        
        boxShader.use();
        glBindVertexArray(VAO);
        glm::mat4 model = glm::mat4(1.0f);
        boxShader.setBoxColor(boxColor);
        boxShader.setLightColors(lightColor);
        boxShader.set3DModel(model);
        boxShader.set3DView(view);
        boxShader.set3DProjection(projection);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //����Shader һ������
        lightingShader.use();
        glBindVertexArray(lightVAO);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightingShader.set3DModel(model);
        lightingShader.set3DView(view);
        lightingShader.set3DProjection(projection);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //for (unsigned int i = 0; i < 10; i++)
        //{
        //    model = glm::translate(model, cubePositions[i]);
        //    //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
        //    
        //    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //}
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        // �������岢��ѯIO�¼�

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void linkAttributePointer() {
    //�Ѷ������鸴�Ƶ������й�OpenGLʹ��
    //��VAO,VBO,IBO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);    
    //����������±�
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    
    
    //---------->���ö���<-----------
    //1.���õĶ�������0 1 2 3
    //2.ָ���������ԵĴ�С
    //3.ָ�����ݵ�����
    //4.��׼��(Normalize),���ŵ�(0,1]��Χ
    //5.����(Stride)�������������������Ķ���������֮��ļ��
    //6.�����ڻ�������ʼλ�õ�ƫ����(Offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //COLOR
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    //TEXTURE
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);


    //--------------->�����ǻ����ͼ�������ϲ�����fShader�������<--------------
    
    //��������
    glGenTextures(1, &texTure);
    //ͼƬ1
    //������
    glBindTexture(GL_TEXTURE_2D, texTure);
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    /*int width, height, nrChannel;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("D://tempData//doge.jpg", &width, &height, &nrChannel, 0);
    
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);*/

    //for merge two pic together,both of the option must on!
    glEnable(GL_BLEND);//�򿪻��ɫ����
    glDisable(GL_DEPTH_TEST);//�ر���Ȳ��Թ���

    //of couse the second operation�� ͼƬ2���
    glGenTextures(1, &texTure2);
    
    glBindTexture(GL_TEXTURE_2D, texTure2);
    
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //int width, height, nrChannel;
    /*stbi_set_flip_vertically_on_load(true);
    data = stbi_load("D://tempData//douni.png", &width, &height, &nrChannel, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);*/
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInputWindow(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        cameraPos.y = 0.2f;
    }
    else
    {
        cameraPos.y = 0.3f;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_cb(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}