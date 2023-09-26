#ifdef __APPLE__

#include "CoreFoundation/CoreFoundation.h"

int mac_get_bundle_path(char *path, const char* filename, const char* extension) {
    CFBundleRef mainBundle = CFBundleGetMainBundle();

    CFStringRef cfFilename = CFStringCreateWithCString(kCFAllocatorDefault, filename, kCFStringEncodingMacRoman);
    CFStringRef cfExtension = CFStringCreateWithCString(kCFAllocatorDefault, extension, kCFStringEncodingMacRoman);

    CFURLRef fileUrl = CFBundleCopyResourceURL(mainBundle, cfFilename, cfExtension, NULL);
    if (fileUrl == NULL) {
        return 0;
    }
    
    CFStringRef filePath = CFURLCopyFileSystemPath(fileUrl, kCFURLPOSIXPathStyle);
    if (filePath == NULL) {
        return 0;
    }

    sprintf(path, "%s", CFStringGetCStringPtr(filePath, kCFStringEncodingMacRoman));

    CFRelease(cfExtension);
    CFRelease(cfFilename);
    
    return 1;
}

#endif
