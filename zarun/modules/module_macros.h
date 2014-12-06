/*
 * module_macros.h
 *
 */

#ifndef MODULE_MACROS_H_
#define MODULE_MACROS_H_

#define DECLARE_THIN_MODULE(mod_name)                  \
  class mod_name : public ThinNativeModule<mod_name> { \
   public:                                             \
    static const char kModuleName[];                   \
    static WrapperInfo kWrapperInfo;

#define DECLARE_THIN_MODULE_END(mod_name) \
 private:                                 \
  friend ThinNativeModule<mod_name>;      \
  }                                       \
  ;

#endif  // MODULE_MACROS_H_
