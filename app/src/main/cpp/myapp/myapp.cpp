#include "rb_base.h"
#include "rb_platform.h"

extern "C" {
    void on_app_start() {
        // Called when app starts

        // platform_on_init() must be called here
        platform_on_init("MyApp", 320, 240);
    }
    void on_app_frame() {
        // Called every frame
    }
    void on_app_stop() {
        // Called before app quits
    }

    void on_app_key_pressed(int code, int state) {
        // Handle key pressed
    }

    rgb_color app_get_palette_color(byte color, byte brightness) {
        // Return palette color
        return { 255, 0, 0 };
    }

}
