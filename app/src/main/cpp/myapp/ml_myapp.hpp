
#include <app_framework/application.h>
#include <app_framework/gui.h>
#include <app_framework/ml_macros.h>
#include <app_framework/toolset.h>
#include <app_framework/material/textured_material.h>
#include <ml_hand_tracking.h>
#include <ml_perception.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <thread>
#include <chrono>

#include "rb_base.h"

using namespace ml::app_framework;

// TODO. Refactor in different classes and also change visibilty
class RBIdeApp : public ml::app_framework::Application {
public:
    RBIdeApp(struct android_app *state);
    ~RBIdeApp();

// App
public:
    void OnStart() override;
    void OnPause() override;
    void OnStop() override;
    void OnResume() override;
    void OnUpdate(float) override;
    void OnRenderCamera(std::shared_ptr<ml::app_framework::CameraComponent> cam) override;

// TODO: Refactor all below in sub-systems
// Headpose
private:
    void InitializeHeadTransform();
    glm::vec3 RecenterOnHead(const glm::vec3 &position);

// Keyboard support
private:
    void InitializeInput();
    void OnChar(InputHandler::EventArgs key_event_args);
    void OnKeyDown(InputHandler::EventArgs key_event_args);
    void OnKeyUp(InputHandler::EventArgs key_event_args);

// Pixel buffer
public:
    void SetPixel(int x, int y, byte color, int brightness);
    void DrawLine(int x1, int y1, int x2, int y2, byte color);
    void ClearBuffer(byte gray);
    void CreateBuffer(int width, int height);
    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }

private:
    void InitializeGraphics();
    void UpdateTexture();
    rgb_color GetColor(byte color, byte brightness);

private:
    byte* m_pixels = nullptr;
    GLuint m_texture_id;
    int m_width;
    int m_height;
    std::shared_ptr<ml::app_framework::Node> m_gui_node;
    glm::quat m_head_rotation_horizontal;
    glm::vec3 m_head_translation;

private:
    bool m_run = false;
    bool m_ctrl_pressed = false;

// GUI
private:
    void UpdateGui();
};
