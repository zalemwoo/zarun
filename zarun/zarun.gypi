{
  'variables': {
    'zarun_sources': [
        './utils/file_util.cc',
        './utils/path_util.cc',
        './utils/standard_out.cc',
        
        './backend/backend_context_delegate.cc',
        './backend/backend_thread.cc',
        './backend/backend_application.cc',
        
        "./modules/native_object.cc',
        "./modules/native_module.cc',
        "./modules/native_source_map.cc',
        "./modules/common_module_system.cc',
        "./modules/module_provider.cc',
        "./modules/cpp/internal/system_env.cc',
        "./modules/cpp/os.cc',
        "./modules/cpp/process.cc',
        "./modules/cpp/subprocess.cc',
        
        './console.cc',
        './switches.cc',
        './line_editor.cc',
        './safe_builtins.cc',
        './script_context.cc',
        './environment.cc',
        './zarun_shell.cc',
        './zarun_main.cc',
    ],
    'javascript_files': [
      './modules/js/bootstrap.js',
      './modules/js/node_util.js',
    ],
  },
}
