#ifndef IMAGE_EXPORT_H
#define IMAGE_EXPORT_H

#if defined(_WIN32)
#define IMAGE_EXPORT __declspec(dllexport)
#define IMAGE_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
#define IMAGE_EXPORT __attribute__ ((visibility("default")))
#define IMAGE_IMPORT __attribute__ ((visibility("default")))
#else
#define IMAGE_EXPORT
#define IMAGE_IMPORT
#endif

#if defined(IMAGE_BUILD_NODEDEF_DLL)
#define IMAGE_DECL IMAGE_EXPORT
#else
#define IMAGE_DECL IMAGE_IMPORT
#endif

#endif
