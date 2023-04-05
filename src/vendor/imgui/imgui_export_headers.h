
#ifndef IMGUI_API_H
#define IMGUI_API_H

#ifdef IMGUI_STATIC_DEFINE
#  define IMGUI_API
#  define IMGUI_NO_EXPORT
#else
#  ifndef IMGUI_API
#    ifdef imgui_EXPORTS
        /* We are building this library */
#      define IMGUI_API 
#    else
        /* We are using this library */
#      define IMGUI_API 
#    endif
#  endif

#  ifndef IMGUI_NO_EXPORT
#    define IMGUI_NO_EXPORT 
#  endif
#endif

#ifndef IMGUI_DEPRECATED
#  define IMGUI_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef IMGUI_DEPRECATED_EXPORT
#  define IMGUI_DEPRECATED_EXPORT IMGUI_API IMGUI_DEPRECATED
#endif

#ifndef IMGUI_DEPRECATED_NO_EXPORT
#  define IMGUI_DEPRECATED_NO_EXPORT IMGUI_NO_EXPORT IMGUI_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef IMGUI_NO_DEPRECATED
#    define IMGUI_NO_DEPRECATED
#  endif
#endif

#endif /* IMGUI_API_H */
