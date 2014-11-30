{
  'variables': {
    'zarun_sources': [
	    './utils/file_util.cc',
	    './utils/path_util.cc',
	    
	    './backend/backend_context_delegate.cc',
	    './backend/backend_thread.cc',
	    './backend/backend_application.cc',
	    
	    './modules/native_javascript_module.cc',
	    './modules/object_backed_native_module.cc',
	    './modules/native_source_map.cc',
	    './modules/javascript_module_system.cc',
	    './modules/module_provider.cc',
	    './modules/cpp/console.cc',
	    './modules/cpp/process.cc',
	    
	    './console.cc',
	    './switches.cc',
	    './line_editor.cc',
	    './safe_builtins.cc',
	    './script_context.cc',
	    './environment.cc',
	    './zarun_shell.cc',
	    './zarun_main.cc',
    ],
  },
}
