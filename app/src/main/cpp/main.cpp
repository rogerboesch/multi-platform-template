#include "ml_myapp.hpp"
#include "rb_log.h"

void android_main(struct android_app *state) {
#ifndef ML_LUMIN
    RBLOG("This app is not supported on ZI!");
#else

    RBLOG("");
    RBLOG("=================================================");
    RBLOG("android_main()");
  
    RBIdeApp app(state);
    app.RunApp();
#endif
}
