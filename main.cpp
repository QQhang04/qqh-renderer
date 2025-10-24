#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

// ������Ϣ
uint32_t mDiffuseTexture{ 0 };
DirectionalLight directionalLight;
math::vec3f envLight;

// ���
Camera* cam = nullptr;
math::vec3f camPos;

// ����
Image* image;
uint32_t texture;

//�������Զ�Ӧvbo
uint32_t positionVbo = 0;
uint32_t positionVbo1 = 0;
uint32_t colorVbo = 0;
uint32_t colorVbo1 = 0;
uint32_t uvVbo = 0;

//�����ε�indices
uint32_t ebo = 0;

//������ר��vao
uint32_t vao = 0;
uint32_t vao1 = 0;

// ʹ�õ�Shader
LightShader* shader = nullptr;

//mvp�任����
math::Mat4f modelMatrix;
math::Mat4f viewMatrix;
math::Mat4f perspectiveMatrix;

float angle = 0.0f;
void transform() {
    angle += 0.01f;
    //ģ�ͱ任
    modelMatrix = math::rotate(math::Mat4f(1.0f), angle, math::vec3f(0.0f, 1.0f, 0.0f));
}

void prepare() {
    camPos = { 0, 1, 3 };
    cam = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, math::vec3f(0, 1, 0), camPos);
    app->setCamera(cam);

    image = Image::createImage("assets/textures/pika.png");
    texture = sgl->genTexture();
    sgl->bindTexture(texture);
    sgl->texImage2D(image->mWidth, image->mHeight, image->mData);
    sgl->texParameter(TEXTURE_FILTER, TEXTURE_FILTER_LINEAR);
    sgl->texParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
    sgl->texParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
    sgl->bindTexture(0);

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    };

    uint32_t indices[] = {
        0, 1, 2, 3, 4, 5,
        6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35
    };

    //����indices��Ӧebo
    ebo = sgl->genBuffer();
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 36, indices);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

    //����vao���Ұ�
    vao = sgl->genVertexArray();
    sgl->bindVertexArray(vao);

    //�ܵĽ���ʽvbo
    int interleavedVbo = sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, interleavedVbo);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 288, vertices);

    //����vao���Ұ�
    vao = sgl->genVertexArray();
    sgl->bindVertexArray(vao);
    sgl->bindBuffer(ARRAY_BUFFER, interleavedVbo);

    //position
    sgl->vertexAttributePointer(0, 3, 8 * sizeof(float), 0);

    // normal
    sgl->vertexAttributePointer(1, 3, 8 * sizeof(float), 3 * sizeof(float));

    //uv
    sgl->vertexAttributePointer(2, 2, 8 * sizeof(float), 6 * sizeof(float));

    sgl->bindBuffer(ARRAY_BUFFER, 0);
    sgl->bindVertexArray(0);

    shader = new LightShader();
    directionalLight.color = { 1.0f, 1.0f, 1.0f };
    directionalLight.direction = { -1.0f, -0.3f, -0.7f };
    envLight = { 0.1f, 0.1f, 0.1f };
    /*perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    auto cameraModelMatrix = math::translate(math::Mat4f(1.0f), math::vec3f(0.0f, 0.0f, 3.0f));
    viewMatrix = math::inverse(cameraModelMatrix);*/
}

void render() {
    transform();
    shader->mModelMatrix = modelMatrix;
    shader->mViewMatrix = cam->getViewMatrix();
    shader->mProjectionMatrix = cam->getProjectionMatrix();
    shader->mDiffuseTexture = texture;

    shader->mDirectionalLight = directionalLight;
    shader->mEnvLight = envLight;

    sgl->clear();
    sgl->useProgram(shader);

    sgl->bindVertexArray(vao);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->drawElement(DRAW_TRIANGLES, 0, 36); 
}

int APIENTRY wWinMain(
    _In_        HINSTANCE hInstance,        // ��Ӧ�ó���ʵ�������Ψһָ����ǰ����
    _In_opt_    HINSTANCE hPrevInstance,    // ������ǰһ��ʵ����һ����null
    _In_        LPWSTR    lpCmdLine,        // Ӧ�ó������в���
    _In_        int       nCmdShow          // ���������ʾ����󻯡���С�������أ����������
)
{
    if (!app->initApplication(hInstance, 800, 600)) {
        return -1;
    }

    sgl->initSurface(app->getWidth(), app->getHeight(), app->getCanvas());

    bool alive = true;
    prepare();

  
    while (alive) {
        alive = app->peekMessage();
        cam->update();
        render();
        app->show();
    }

    Image::destroyImage(image);
    sgl->deleteTexture(texture);
    delete cam;
    return 0;
}