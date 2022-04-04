#ifndef _UEXPORT_H_
#define _UEXPORT_H_

/* replace _EXPORT_XXX_ to library name */

#if UOS == WIN
  #ifdef _EXPORT_XXX_
    #define xapi __declspec(dllexport)
  #elif _NOTEXPORT_XXX_
    #define xapi
  #else
    #define xapi __declspec(dllimport)
  #endif

  #ifdef _EXPORT_XXX_
    #define xapi_tpl __declspec(dllexport)
  #elif _NOTEXPORT_XXX_
    #define xapi_tpl
  #else
    #ifdef uapi_tpl_decl
      #define xapi_tpl __declspec(dllimport)
    #else
      #define xapi_tpl
    #endif
  #endif
  #define xcall __stdcall
#else
  #define xapi
  #define xapi_tpl
  #define xcall
#endif

#endif
