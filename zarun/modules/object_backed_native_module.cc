/*
 * object_backed_native_module.cc
 *
 */

#include "zarun/modules/object_backed_native_module.h"
#include "zarun/script_context.h"
#include "zarun/console.h"

namespace zarun {

namespace {
// Key for the base::Bound routed function.
const char* kHandlerFunction = "handler_function";
}  // namespace

ObjectBackedNativeJavaScriptModule::ObjectBackedNativeJavaScriptModule(
    ScriptContext* context)
    : router_data_(context->v8_context()->GetIsolate()),
      context_(context),
      object_template_(
          v8::ObjectTemplate::New(context->v8_context()->GetIsolate())) {
}

ObjectBackedNativeJavaScriptModule::~ObjectBackedNativeJavaScriptModule() {
  Invalidate();
}

v8::Handle<v8::Object> ObjectBackedNativeJavaScriptModule::NewInstance() {
  return object_template_.NewHandle(v8::Isolate::GetCurrent())->NewInstance();
}

v8::Isolate* ObjectBackedNativeJavaScriptModule::GetIsolate() const {
  return context_->isolate();
}

// static
void ObjectBackedNativeJavaScriptModule::Router(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::HandleScope handle_scope(args.GetIsolate());
  v8::Handle<v8::Object> data = args.Data().As<v8::Object>();

  v8::Handle<v8::Value> handler_function_value =
      data->Get(v8::String::NewFromUtf8(args.GetIsolate(), kHandlerFunction));
  // See comment in header file for why we do this.
  if (handler_function_value.IsEmpty() ||
      handler_function_value->IsUndefined()) {
    console::Error(args.GetIsolate()->GetCallingContext(),
                   "Extension view no longer exists");
    return;
  }

  DCHECK(handler_function_value->IsExternal());
  static_cast<HandlerFunction*>(
      handler_function_value.As<v8::External>()->Value())->Run(args);
}

void ObjectBackedNativeJavaScriptModule::RouteFunction(
    const std::string& name,
    const HandlerFunction& handler_function) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::Context::Scope context_scope(context_->v8_context());

  v8::Local<v8::Object> data = v8::Object::New(isolate);

  data->Set(v8::String::NewFromUtf8(isolate, kHandlerFunction),
            v8::External::New(isolate, new HandlerFunction(handler_function)));
  v8::Handle<v8::FunctionTemplate> function_template =
      v8::FunctionTemplate::New(isolate, Router, data);
  object_template_.NewHandle(isolate)
      ->Set(isolate, name.c_str(), function_template);
  router_data_.Append(data);
}

void ObjectBackedNativeJavaScriptModule::Invalidate() {
  if (!is_valid())
    return;
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::Context::Scope context_scope(context_->v8_context());

  for (size_t i = 0; i < router_data_.Size(); i++) {
    v8::Handle<v8::Object> data = router_data_.Get(i);
    v8::Handle<v8::Value> handler_function_value =
        data->Get(v8::String::NewFromUtf8(isolate, kHandlerFunction));
    CHECK(!handler_function_value.IsEmpty());
    delete static_cast<HandlerFunction*>(
        handler_function_value.As<v8::External>()->Value());
    data->Delete(v8::String::NewFromUtf8(isolate, kHandlerFunction));
  }
  router_data_.Clear();
  object_template_.reset();
  context_ = NULL;
  NativeJavaScriptModule::Invalidate();
}

}  // namespace zarun
