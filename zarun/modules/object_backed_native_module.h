/*
 * object_backed_native_module.h
 *
 */

#ifndef ZARUN_MODULES_OBJECT_BACKED_NATIVE_MODULE_H_
#define ZARUN_MODULES_OBJECT_BACKED_NATIVE_MODULE_H_

#include <string>
#include <vector>

#include "base/bind.h"
#include "base/macros.h"
#include "v8/include/v8.h"

#include "zarun/modules/native_javascript_module.h"
#include "zarun/scoped_persistent.h"

namespace zarun {

class ScriptContext;

// An ObjectBackedNativeModule is a factory for JS objects with
// functions on
// them that map to native C++ functions. Subclasses should call RouteFunction()
// in their constructor to define functions on the created JS objects.
class ObjectBackedNativeModule : public NativeJavaScriptModule {
 public:
  explicit ObjectBackedNativeModule(ScriptContext* context);
  ~ObjectBackedNativeModule() override;

  // Create an object with bindings to the native functions defined through
  // RouteFunction().
  v8::Handle<v8::Object> NewInstance() override;

  v8::Isolate* GetIsolate() const;

 protected:
  typedef base::Callback<void(const v8::FunctionCallbackInfo<v8::Value>&)>
      HandlerFunction;

  // Installs a new 'route' from |name| to |handler_function|. This means that
  // NewInstance()s of this ObjectBackedNativeModule will have a
  // property
  // |name| which will be handled by |handler_function|.
  void RouteFunction(const std::string& name,
                     const HandlerFunction& handler_function);

  ScriptContext* context() const { return context_; }

  void Invalidate() override;

 private:
  // Callback for RouteFunction which routes the V8 call to the correct
  // base::Bound callback.
  static void Router(const v8::FunctionCallbackInfo<v8::Value>& args);

  // When RouteFunction is called we create a v8::Object to hold the data we
  // need when handling it in Router() - this is the base::Bound function to
  // route to.
  //
  // We need a v8::Object because it's possible for v8 to outlive the
  // base::Bound function; the lifetime of an ObjectBackedNativeHandler is the
  // lifetime of webkit's involvement with it, not the life of the v8 context.
  // A scenario when v8 will outlive us is if a frame holds onto the
  // contentWindow of an iframe after it's removed.
  //
  // So, we use v8::Objects here to hold that data, effectively refcounting
  // the data. When |this| is destroyed we remove the base::Bound function from
  // the object to indicate that it shoudn't be called.
  typedef std::vector<v8::UniquePersistent<v8::Object>> RouterData;
  RouterData router_data_;

  ScriptContext* context_;

  ScopedPersistent<v8::ObjectTemplate> object_template_;

  DISALLOW_COPY_AND_ASSIGN(ObjectBackedNativeModule);
};

}  // namespace zarun

#endif  // ZARUN_MODULES_OBJECT_BACKED_NATIVE_MODULE_H_
