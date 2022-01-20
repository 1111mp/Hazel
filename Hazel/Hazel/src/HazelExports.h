
#ifndef HazelExports_H
#define HazelExports_H

#ifdef HAZEL_LIB_STATIC_DEFINE
#  define HazelExports
#  define HAZEL_LIB_NO_EXPORT
#else
#  ifndef HazelExports
#    ifdef hazel_lib_EXPORTS
        /* We are building this library */
#      define HazelExports __declspec(dllexport)
#    else
        /* We are using this library */
#      define HazelExports __declspec(dllimport)
#    endif
#  endif

#  ifndef HAZEL_LIB_NO_EXPORT
#    define HAZEL_LIB_NO_EXPORT 
#  endif
#endif

#ifndef HAZEL_LIB_DEPRECATED
#  define HAZEL_LIB_DEPRECATED __declspec(deprecated)
#endif

#ifndef HAZEL_LIB_DEPRECATED_EXPORT
#  define HAZEL_LIB_DEPRECATED_EXPORT HazelExports HAZEL_LIB_DEPRECATED
#endif

#ifndef HAZEL_LIB_DEPRECATED_NO_EXPORT
#  define HAZEL_LIB_DEPRECATED_NO_EXPORT HAZEL_LIB_NO_EXPORT HAZEL_LIB_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef HAZEL_LIB_NO_DEPRECATED
#    define HAZEL_LIB_NO_DEPRECATED
#  endif
#endif

#endif /* HazelExports_H */
