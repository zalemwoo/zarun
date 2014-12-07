/*
 * module_macros.h
 *
 */

#ifndef ZARUN_MODULES_MODULE_MACROS_H_
#define ZARUN_MODULES_MODULE_MACROS_H_

// Start declare ThinNativeModule class
#define DECLARE_THIN_MODULE(class_name) \
  class class_name : public ThinNativeModule<class_name>

// Declare ThinNativeModule class member
#define DECLARE_THIN_MODULE_START(class_name, module_name)                    \
  {                                                                           \
   public:                                                                    \
    static constexpr const char* kModuleName = module_name;                   \
    static WrapperInfo kWrapperInfo;                                          \
                                                                              \
   private:                                                                   \
    class_name(ScriptContext* context);                                       \
    ~class_name() override;                                                   \
    gin::ObjectTemplateBuilder GetObjectTemplateBuilder(v8::Isolate* isolate) \
        override;                                                             \
    void Invalidate() override;                                               \
    friend base::DefaultDeleter<class_name>;                                  \
    friend ThinNativeModule<class_name>;

// End declare ThinNativeModule
#define DECLARE_THIN_MODULE_END \
  }                             \
  ;  // end class declaration

// Start declare WrappableNativeObject class
#define DECLARE_NATIVE_OBJECT(class_name) \
  class class_name : public WrappableNativeObject<class_name>

// Declare WrappableNativeObject class member
#define DECLARE_NATIVE_OBJECT_START(class_name)                               \
  {                                                                           \
   public:                                                                    \
    static WrapperInfo kWrapperInfo;                                          \
                                                                              \
   private:                                                                   \
    ~class_name() override;                                                   \
    gin::ObjectTemplateBuilder GetObjectTemplateBuilder(v8::Isolate* isolate) \
        override;                                                             \
    friend WrappableNativeObject<class_name>;

// End declare WrappableNativeObject
#define DECLARE_NATIVE_OBJECT_END \
  }                               \
  ;  // end class declaration

// In C++ file, define the kWrapperInfo declared prev.
#define DEFINE_WRAPPER_INFO(class_name) \
  gin::WrapperInfo class_name::kWrapperInfo = {gin::kEmbedderNativeGin}

#endif  // ZARUN_MODULES_MODULE_MACROS_H_
