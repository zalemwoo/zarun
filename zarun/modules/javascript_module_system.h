/*
 * javascript_module_system.h
 *
 */

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ZARUN_MODULES_JAVASCRIPT_MODULE_SYSTEM_H_
#define ZARUN_MODULES_JAVASCRIPT_MODULE_SYSTEM_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "base/compiler_specific.h"
#include "base/memory/linked_ptr.h"
#include "base/memory/scoped_ptr.h"
#include "v8/include/v8.h"

#include "zarun/modules/module_registry_observer.h"
#include "zarun/modules/native_javascript_module.h"
#include "zarun/modules/object_backed_native_module.h"

namespace zarun {

class ScriptContext;

// A module system for JS similar to node.js' require() function.
// Each module has three variables in the global scope:
//   - exports, an object returned to dependencies who require() this
//     module.
//   - require, a function that takes a module name as an argument and returns
//     that module's exports object.
//   - requireNative, a function that takes the name of a registered
//     NativeModule and returns an object that contains the functions the
//     NativeModule defines.
//
// Each module in a ModuleSystem is executed at most once and its exports
// object cached.
//
// Note that a ModuleSystem must be used only in conjunction with a single
// v8::Context.
class JavaScriptModuleSystem : public ObjectBackedNativeModule,
                               public zarun::ModuleRegistryObserver {
 public:
  class SourceMap {
   public:
    virtual ~SourceMap() {}
    virtual v8::Handle<v8::Value> GetSource(v8::Isolate* isolate,
                                            const std::string& name) = 0;
    virtual bool Contains(const std::string& name) = 0;
  };

  class ExceptionHandler {
   public:
    virtual ~ExceptionHandler() {}
    virtual void HandleUncaughtException(const v8::TryCatch& try_catch) = 0;

   protected:
    // Formats |try_catch| as a nice string.
    std::string CreateExceptionString(const v8::TryCatch& try_catch);
  };

  // Enables native bindings for the duration of its lifetime.
  class NativesEnabledScope {
   public:
    explicit NativesEnabledScope(JavaScriptModuleSystem* module_system);
    ~NativesEnabledScope();

   private:
    JavaScriptModuleSystem* module_system_;
    DISALLOW_COPY_AND_ASSIGN(NativesEnabledScope);
  };

  // |source_map| is a weak pointer.
  JavaScriptModuleSystem(ScriptContext* context, SourceMap* source_map);
  ~JavaScriptModuleSystem() override;

  // Require the specified module. This is the equivalent of calling
  // require('module_name') from the loaded JS files.
  v8::Handle<v8::Value> Require(const std::string& module_name);
  void Require(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Run |code| in the current context with the name |name| used for stack
  // traces.
  v8::Handle<v8::Value> RunString(v8::Handle<v8::String> code,
                                  v8::Handle<v8::String> name);

  // Calls the specified method exported by the specified module. This is
  // equivalent to calling require('module_name').method_name() from JS.
  v8::Local<v8::Value> CallModuleMethod(const std::string& module_name,
                                        const std::string& method_name);
  v8::Local<v8::Value> CallModuleMethod(
      const std::string& module_name,
      const std::string& method_name,
      std::vector<v8::Handle<v8::Value> >* args);
  v8::Local<v8::Value> CallModuleMethod(const std::string& module_name,
                                        const std::string& method_name,
                                        int argc,
                                        v8::Handle<v8::Value> argv[]);

  // Register |native_handler| as a potential target for requireNative(), so
  // calls to requireNative(|name|) from JS will return a new object created by
  // |native_handler|.
  void RegisterNativeModule(const std::string& name,
                            scoped_ptr<NativeJavaScriptModule> native_handler);

  // Causes requireNative(|name|) to look for its module in |source_map_|
  // instead of using a registered native handler. This can be used in unit
  // tests to mock out native modules.
  void OverrideNativeModuleForTest(const std::string& name);

  // Executes |code| in the current context with |name| as the filename.
  v8::Handle<v8::Value> RunString(const std::string& code,
                                  const std::string& name);

  // Make |object|.|field| lazily evaluate to the result of
  // require(|module_name|)[|module_field|].
  //
  // TODO(kalman): All targets for this method are ObjectBackedNativeModules,
  //               move this logic into those classes (in fact, the chrome
  //               object is the only client, only that needs to implement it).
  void SetLazyField(v8::Handle<v8::Object> object,
                    const std::string& field,
                    const std::string& module_name,
                    const std::string& module_field);

  void SetLazyField(v8::Handle<v8::Object> object,
                    const std::string& field,
                    const std::string& module_name,
                    const std::string& module_field,
                    v8::AccessorGetterCallback getter);

  // Make |object|.|field| lazily evaluate to the result of
  // requireNative(|module_name|)[|module_field|].
  // TODO(kalman): Same as above.
  void SetNativeLazyField(v8::Handle<v8::Object> object,
                          const std::string& field,
                          const std::string& module_name,
                          const std::string& module_field);

  // Passes exceptions to |handler| rather than console::Fatal.
  void SetExceptionHandlerForTest(scoped_ptr<ExceptionHandler> handler) {
    exception_handler_ = handler.Pass();
  }

 protected:
  friend class ScriptContext;
  void Invalidate() override;

 private:
  typedef std::map<std::string, linked_ptr<NativeJavaScriptModule> >
      NativeModuleMap;

  // Retrieves the lazily defined field specified by |property|.
  static void LazyFieldGetter(v8::Local<v8::String> property,
                              const v8::PropertyCallbackInfo<v8::Value>& info);
  // Retrieves the lazily defined field specified by |property| on a native
  // object.
  static void NativeLazyFieldGetter(
      v8::Local<v8::String> property,
      const v8::PropertyCallbackInfo<v8::Value>& info);

  // Called when an exception is thrown but not caught.
  void HandleException(const v8::TryCatch& try_catch);

  void RequireForJs(const v8::FunctionCallbackInfo<v8::Value>& args);
  v8::Local<v8::Value> RequireForJsInner(v8::Handle<v8::String> module_name);

  typedef v8::Handle<v8::Value>(JavaScriptModuleSystem::*RequireFunction)(
      const std::string&);
  // Base implementation of a LazyFieldGetter which uses |require_fn| to require
  // modules.
  static void LazyFieldGetterInner(
      v8::Local<v8::String> property,
      const v8::PropertyCallbackInfo<v8::Value>& info,
      RequireFunction require_function);

  // Return the named source file stored in the source map.
  // |args[0]| - the name of a source file in source_map_.
  v8::Handle<v8::Value> GetSource(const std::string& module_name);

  // Return an object that contains the native methods defined by the named
  // NativeModule.
  // |args[0]| - the name of a native handler object.
  v8::Handle<v8::Value> RequireNativeFromString(const std::string& native_name);
  void RequireNative(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Return a promise for a requested module.
  // |args[0]| - the name of a module.
  void RequireAsync(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Wraps |source| in a (function(define, require, requireNative, ...) {...}).
  v8::Handle<v8::String> WrapSource(v8::Handle<v8::String> source);

  // NativeModule implementation which returns the private area of an Object.
  void Private(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Loads and runs a Javascript module.
  v8::Handle<v8::Value> LoadModule(const std::string& module_name);

  // Invoked when a module is loaded in response to a requireAsync call.
  // Resolves |resolver| with |value|.
  void OnModuleLoaded(
      scoped_ptr<v8::UniquePersistent<v8::Promise::Resolver> > resolver,
      v8::Handle<v8::Value> value);

  // zarun::ModuleRegistryObserver overrides.
  void OnDidAddPendingModule(
      const std::string& id,
      const std::vector<std::string>& dependencies) override;

  ScriptContext* context_;

  // A map from module names to the JS source for that module. GetSource()
  // performs a lookup on this map.
  SourceMap* source_map_;

  // A map from native module names to native modules.
  NativeModuleMap native_module_map_;

  // When 0, natives are disabled, otherwise indicates how many callers have
  // pinned natives as enabled.
  int natives_enabled_;

  // Called when an exception is thrown but not caught in JS. Overridable by
  // tests.
  scoped_ptr<ExceptionHandler> exception_handler_;

  std::set<std::string> overridden_native_handlers_;

  base::WeakPtrFactory<JavaScriptModuleSystem> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(JavaScriptModuleSystem);
};

}  // namespace zarun

#endif  // ZARUN_MODULES_JAVASCRIPT_MODULE_SYSTEM_H_
