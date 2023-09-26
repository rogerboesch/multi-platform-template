#include "rb_log.h"
#include "rb_platform.h"

#include "ml_myapp.hpp"
#include "rb_file.hpp"

#include <app_framework/application.h>
#include <app_framework/gui.h>
#include <app_framework/ml_macros.h>
#include <app_framework/toolset.h>
#include <app_framework/convert.h>
#include <ml_perception.h>
#include <ml_snapshot.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <chrono>
#include <sstream>
#include <string>
#include <vector>
#include <time.h>

extern "C" {
    const char* platform_resource_file_path(const char* filename, const char* extension);
}

extern "C" {
	void on_app_start();
	void on_app_frame();
	void on_app_stop();
    void on_app_key_pressed(int code, int state);

    rgb_color app_get_palette_color(byte color, byte brightness);
}

using namespace ml::app_framework;

#define SCREEN_WIDTH 0.75f  // Width of screen, in meters
#define SCREEN_DIST 0.5f    // Screen distance from the origin
#define TEXTURE_SIZE 512    // Texture size used

RBIdeApp* s_app = nullptr;

// App ---------------------------------------------------------------------------------------------

RBIdeApp::RBIdeApp(struct android_app *state) : ml::app_framework::Application(state, USE_GUI) {
    RBTRACELOG("RBIdeApp", "RBIdeApp");

    s_app = this;

    FileReader::SetAssetManager(state->activity->assetManager);
}

RBIdeApp::~RBIdeApp() {
    RBTRACELOG("RBIdeApp", "~RBIdeApp");
    s_app = nullptr;
}

void RBIdeApp::OnStart() {
    RBTRACELOG("RBIdeApp", "OnStart");

    InitializeHeadTransform();
    InitializeInput();

    InitializeGraphics();

    on_app_start();
    on_app_frame();
    
    ClearBuffer(80);

    const auto translation = RecenterOnHead(glm::vec3(-0.25f, 0.11f, -1.5f));  //> Move the gui to the upper right corner
    GetGui().Place(m_head_rotation_horizontal, translation);

    m_gui_node->SetLocalRotation(m_head_rotation_horizontal);
    m_gui_node->SetLocalTranslation(RecenterOnHead(glm::vec3(0.75f, 0.25f, -1.5f)));  //> Move the surface away from the user
}

void RBIdeApp::OnPause() {
    RBTRACELOG("RBIdeApp", "OnPause");
}

void RBIdeApp::OnResume() {
    RBTRACELOG("RBIdeApp", "OnResume");

    GetGui().Show();
}

void RBIdeApp::OnStop() {
    RBTRACELOG("RBIdeApp", "OnStop");

    on_app_stop();
}

void RBIdeApp::OnUpdate(float) {
    UpdateGui();

    if (m_run) {
        on_app_frame();
    }   
}

void RBIdeApp::OnRenderCamera(std::shared_ptr<ml::app_framework::CameraComponent>) {
    UpdateTexture();
}

// GUI ---------------------------------------------------------------------------------------------

void RBIdeApp::UpdateGui() {
    auto &gui = GetGui();
    constexpr auto dialog_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
                                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

    gui.BeginUpdate();
    gui.BeginDialog("ML - MyApp", nullptr, dialog_flags);

    ImGui::BeginGroup();
        if (m_run) {
            if (ImGui::Button("Pause [CTRL-R]", (ImVec2){0,0})) {
                m_run = false;
            }
        }
        else {
            if (ImGui::Button("Run [CTRL-R]", (ImVec2){0,0})) {
                m_run = true;
            }
        }
        
        ImGui::SameLine(0, 20);
        if (ImGui::Button("<<", (ImVec2){0,0})) {
            on_app_key_pressed(4, true);
        }

        ImGui::SameLine(0, 5);
        if (ImGui::Button("^^", (ImVec2){0,0})) {
            on_app_key_pressed(6, true);
        }

        ImGui::SameLine(0, 5);
        if (ImGui::Button("vv", (ImVec2){0,0})) {
            on_app_key_pressed(7, true);
        }

        ImGui::SameLine(0, 5);
        if (ImGui::Button(">>", (ImVec2){0,0})) {
            on_app_key_pressed(5, true);
        }


        ImGui::SameLine(0, 20);
        if (ImGui::Button("<1>", (ImVec2){0,0})) {
            on_app_key_pressed(0, true);
        }

        ImGui::SameLine(0, 5);
        if (ImGui::Button("<2>", (ImVec2){0,0})) {
            on_app_key_pressed(1, true);
        }

        ImGui::SameLine(0, 5);
        if (ImGui::Button("<3>", (ImVec2){0,0})) {
            on_app_key_pressed(2, true);
        }

        ImGui::SameLine(0, 5);
        if (ImGui::Button("<4>", (ImVec2){0,0})) {
            on_app_key_pressed(3, true);
        }
    ImGui::EndGroup();

    gui.EndDialog();
    gui.EndUpdate();
}

// Headpose ---------------------------------------------------------------------------------------------

void RBIdeApp::InitializeHeadTransform() {
    RBTRACELOG("RBIdeApp", "InitialzeTransform");

    MLHandle head_handle_;
    UNWRAP_MLRESULT(MLHeadTrackingCreate(&head_handle_));
    MLHeadTrackingStaticData head_static_data_ = {};
    UNWRAP_MLRESULT(MLHeadTrackingGetStaticData(head_handle_, &head_static_data_));
    MLSnapshot *snapshot = nullptr;

    UNWRAP_MLRESULT(MLPerceptionGetSnapshot(&snapshot));
    MLTransform head_transform = {};
    UNWRAP_MLRESULT(MLSnapshotGetTransform(snapshot, &head_static_data_.coord_frame_head, &head_transform));

    m_head_rotation_horizontal = ml::app_framework::to_glm(head_transform.rotation);
    m_head_rotation_horizontal[0] = 0.0f;
    m_head_rotation_horizontal[2] = 0.0f;
    m_head_rotation_horizontal = glm::normalize(m_head_rotation_horizontal);
    m_head_translation = ml::app_framework::to_glm(head_transform.position);
    UNWRAP_MLRESULT(MLPerceptionReleaseSnapshot(snapshot));
    UNWRAP_MLRESULT(MLHeadTrackingDestroy(head_handle_));
}

glm::vec3 RBIdeApp::RecenterOnHead(const glm::vec3 &position) {
    return m_head_translation + m_head_rotation_horizontal * position;
}

// Keyboard support ---------------------------------------------------------------------------------------------

void RBIdeApp::InitializeInput() {
    RBTRACELOG("RBIdeApp", "InitializeInput");

    auto on_char = [this](InputHandler::EventArgs key_event_args) {
        OnChar(key_event_args);
    };

    auto on_key_down = [this](InputHandler::EventArgs key_event_args) {
        OnKeyDown(key_event_args);
    };

    auto on_key_up = [this](InputHandler::EventArgs key_event_args) {
        OnKeyUp(key_event_args);
    };

    auto inputHandler = GetInputHandler();
    inputHandler->SetEventHandlers(on_key_down, on_char, on_key_up);
}

void RBIdeApp::OnChar(InputHandler::EventArgs key_event_args) {
    RBLOG_NUM1("OnChar", key_event_args.key_char);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(key_event_args.key_char);
}

void RBIdeApp::OnKeyDown(InputHandler::EventArgs key_event_args) {
    RBLOG_NUM1("OnKeyDown", key_event_args.key_code);

    if (key_event_args.key_code == 133) m_ctrl_pressed = true;
    else {
        if (key_event_args.key_code == 46) m_run = !m_run;                      // R
    }

    if (m_run) {
        if (key_event_args.key_code == 19) on_app_key_pressed(6, true);     // Up
        if (key_event_args.key_code == 20) on_app_key_pressed(7, true);     // Down
        if (key_event_args.key_code == 21) on_app_key_pressed(4, true);     // Left
        if (key_event_args.key_code == 22) on_app_key_pressed(5, true);     // Right
        if (key_event_args.key_code ==  8) on_app_key_pressed(0, true);     // 1
        if (key_event_args.key_code ==  9) on_app_key_pressed(1, true);     // 2
        if (key_event_args.key_code == 10) on_app_key_pressed(2, true);     // 3
        if (key_event_args.key_code == 11) on_app_key_pressed(3, true);     // 4
    }
    else {
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[key_event_args.key_code] = true;
    }
}

void RBIdeApp::OnKeyUp(InputHandler::EventArgs key_event_args) {
    RBLOG_NUM1("OnKeyUp", key_event_args.key_code);

    if (m_run) {
        if (key_event_args.key_code == 19) on_app_key_pressed(6, false);     // Up
        if (key_event_args.key_code == 20) on_app_key_pressed(7, false);     // Down
        if (key_event_args.key_code == 21) on_app_key_pressed(4, false);     // Left
        if (key_event_args.key_code == 22) on_app_key_pressed(5, false);     // Right
        if (key_event_args.key_code ==  8) on_app_key_pressed(0, false);     // 1
        if (key_event_args.key_code ==  9) on_app_key_pressed(1, false);     // 2
        if (key_event_args.key_code == 10) on_app_key_pressed(2, false);     // 3
        if (key_event_args.key_code == 11) on_app_key_pressed(3, false);     // 4
    }
    else {
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[key_event_args.key_code] = false;
    }
}

// Pixel buffer ---------------------------------------------------------------------------------------------

void RBIdeApp::InitializeGraphics() {
    RBTRACELOG("RBIdeApp", "InitializeGraphics");

    // Creation of standard OGL 2D texture
    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto tex = std::make_shared<Texture>(GL_TEXTURE_2D, m_texture_id, TEXTURE_SIZE, TEXTURE_SIZE, true);
    auto quad = Registry::GetInstance()->GetResourcePool()->GetMesh<QuadMesh>();
    auto gui_mat = std::make_shared<TexturedMaterial>(tex);
    gui_mat->SetPolygonMode(GL_FILL);
    auto gui_renderable = std::make_shared<RenderableComponent>(quad, gui_mat);
    m_gui_node = std::make_shared<Node>();
    m_gui_node->SetLocalTranslation({1.0f, 0.5f, -0.75f});  //> Move the surface away from the user
    m_gui_node->SetLocalScale({static_cast<float>(TEXTURE_SIZE) / TEXTURE_SIZE, 1.f, 1.f}); //> Rescale the surface to the video's aspect ratio
    m_gui_node->SetLocalRotation({0.0f, -1.f, 0.0f, 0.0f});  //> Quaternion for 180deg rotation in X axis
    m_gui_node->AddComponent(gui_renderable);
    GetRoot()->AddChild(m_gui_node);
}

void RBIdeApp::UpdateTexture(void) {
    if (m_pixels == nullptr) return;

    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RBIdeApp::SetPixel(int x, int y, byte color, int brightness) {
    if (m_pixels == nullptr) return;

    if (x > m_width || x <= 0) return;
    if (y > m_height || y <= 0) return;

    rgb_color rgb = app_get_palette_color(color, brightness);
    
    int height = TEXTURE_SIZE;
 
    m_pixels[(y * height + x) * 4] = rgb.r;
    m_pixels[(y * height + x) * 4 + 1] = rgb.g;
    m_pixels[(y * height + x) * 4 + 2] = rgb.b;
    m_pixels[(y * height + x) * 4 + 3] = 255;
}

void RBIdeApp::DrawLine(int x1, int y1, int x2, int y2, byte color) {
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1; dy = y2 - y1;
    dx1 = abs(dx); dy1 = abs(dy);
    px = 2 * dy1 - dx1;    py = 2 * dx1 - dy1;

    if (dy1 <= dx1) {
        if (dx >= 0)
            { x = x1; y = y1; xe = x2; }
        else
            { x = x2; y = y2; xe = x1;}

        SetPixel(x, y, color, BRIGHTNESS_OFF);
        
        for (i = 0; x<xe; i++) {
            x = x + 1;
            
            if (px<0)
                px = px + 2 * dy1;
            else {
                if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
                px = px + 2 * (dy1 - dx1);
            }
            
            SetPixel(x, y, color, BRIGHTNESS_OFF);
        }
    }
    else {
        if (dy >= 0)
            { x = x1; y = y1; ye = y2; }
        else
            { x = x2; y = y2; ye = y1; }

        SetPixel(x, y, color, BRIGHTNESS_OFF);

        for (i = 0; y<ye; i++) {
            y = y + 1;
            
            if (py <= 0)
                py = py + 2 * dx1;
            else {
                if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
                py = py + 2 * (dx1 - dy1);
            }
            
            SetPixel(x, y, color, BRIGHTNESS_OFF);
        }
    }
}

void RBIdeApp::ClearBuffer(byte gray) {
    if (m_pixels == nullptr) return;
    memset(m_pixels, gray, TEXTURE_SIZE*TEXTURE_SIZE*4);
}

void RBIdeApp::CreateBuffer(int width, int height) {
    RBTRACELOG("RBIdeApp", "CreateBuffer");

    m_pixels = (byte*)malloc(TEXTURE_SIZE*TEXTURE_SIZE*4);

    m_width = width;
    m_height = height;  

    RBLOG_NUM1(" Buffer width", m_width);
    RBLOG_NUM1("       height", m_height);
}

// Callback support for engine -------------------------------------------------------------------------------

extern "C" {    
    char s_path[512];

    void platform_set_window_title(const char* title) {
    }

    void platform_clear_buffer(byte color) {
        s_app->ClearBuffer(127);
    }

    void platform_set_pixel(int x, int y, byte color, int brightness) {
        s_app->SetPixel(x, y, color, brightness);
    }

    void platform_draw_line(int x1, int y1, int x2, int y2, byte color, int invert) {
        if (invert) {
            y1 = s_app->GetHeight() - y1;
            y2 = s_app->GetHeight() - y2;
        }

        s_app->DrawLine(x1, y1, x2, y2, color);

#if 0
        s_app->DrawLine(1, 1, s_app->GetWidth(), 1, 3, 0);
        s_app->DrawLine(s_app->GetWidth(), 1, s_app->GetWidth(), s_app->GetHeight(), 3, 0);
        s_app->DrawLine(s_app->GetWidth(), s_app->GetHeight(), 1, s_app->GetHeight(), 3, 0);
        s_app->DrawLine(1, s_app->GetHeight(), 1, 1, 3, 0);
#endif
    }

    byte platform_get_input(byte code) {
        return 0;
    }

    byte platform_get_control_state(byte code) {
        return 0;
    }

    const char* platform_resource_file_path(const char* filename, const char* extension) {
        sprintf(s_path, "%s.%s", filename, extension);
        return s_path;
    }

    double platform_get_ms(void) {
        struct timeval  tv;
        gettimeofday(&tv, NULL);

        double time_in_ms = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; // convert tv_sec & tv_usec to millisecond
        
        return time_in_ms;        
    }

    // Events
    void platform_on_init(const char* name, int width, int height) {
        platform_set_window_title(name);
        s_app->CreateBuffer(width, height);
    }

    void platform_on_frame() {
        s_app->ClearBuffer(0);
    }
}