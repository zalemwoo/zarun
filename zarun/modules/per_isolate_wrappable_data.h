/*
 * per_isolate_wrappable_data.h
 *
 */

#ifndef ZARUN_MODULES_PER_ISOLATE_WRAPPABLE_DATA_H_
#define ZARUN_MODULES_PER_ISOLATE_WRAPPABLE_DATA_H_

#include <map>

#include "base/macros.h"
#include "v8/include/v8.h"

#include "zarun/zarun_export.h"

namespace zarun {

struct WrapperInfo;

// There is one instance of PerIsolateWrappableData per v8::Isolate managed by
// Gin. This
// class stores all the Gin-related data that varies per isolate.
class ZARUN_EXPORT PerIsolateWrappableData {
 public:
  PerIsolateWrappableData(v8::Isolate* isolate);
  ~PerIsolateWrappableData();

  static PerIsolateWrappableData* From(v8::Isolate* isolate);

  // Each isolate is associated with a collection of v8::ObjectTemplates and
  // v8::FunctionTemplates. Typically these template objects are created
  // lazily.
  void SetObjectTemplate(WrapperInfo* info,
                         v8::Local<v8::ObjectTemplate> object_template);
  void SetFunctionTemplate(WrapperInfo* info,
                           v8::Local<v8::FunctionTemplate> function_template);

  // These are low-level functions for retrieving object or function templates
  // stored in this object. Because these templates are often created lazily,
  // most clients should call higher-level functions that know how to populate
  // these templates if they haven't already been created.
  v8::Local<v8::ObjectTemplate> GetObjectTemplate(WrapperInfo* info);
  v8::Local<v8::FunctionTemplate> GetFunctionTemplate(WrapperInfo* info);

  v8::Isolate* isolate() { return isolate_; }

 private:
  typedef std::map<WrapperInfo*, v8::Eternal<v8::ObjectTemplate> >
      ObjectTemplateMap;
  typedef std::map<WrapperInfo*, v8::Eternal<v8::FunctionTemplate> >
      FunctionTemplateMap;

  // PerIsolateWrappableData doesn't actually own |isolate_|. Instead,
  // the isolate is owned by the IsolateHolder.
  v8::Isolate* isolate_;
  ObjectTemplateMap object_templates_;
  FunctionTemplateMap function_templates_;

  DISALLOW_COPY_AND_ASSIGN(PerIsolateWrappableData);
};

}  // namespace zarun

#endif  // ZARUN_MODULES_PER_ISOLATE_WRAPPABLE_DATA_H_
