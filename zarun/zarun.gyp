{
  'variables': {
    'chromium_code': 1,
    'javascript_files': [
      './modules/js/bootstrap.js',
    ],
  },
  'targets': [
    {
      'target_name': 'zarun',
      'type': 'executable',
      'dependencies': [
        '../base/base.gyp:base',
        '../base/base.gyp:base_i18n',
        '../v8/tools/gyp/v8.gyp:v8',
        '../gin/gin.gyp/:gin',
        '../third_party/linenoise/linenoise.gyp/:linenoise',
        'zarun_js2c#host',
      ],
      'include_dirs': [
        '<(SHARED_INTERMEDIATE_DIR)' # for zarun_natives.h
      ],
      'sources': [
        './utils/file_util.cc',
        './utils/path_util.cc',
        
        './backend/backend_context_delegate.cc',
        './backend/backend_thread.cc',
        './backend/backend_application.cc',
        
        './modules/native_javascript_module.cc',
        './modules/object_backed_native_module.cc',
        './modules/native_source_map.cc',
        './modules/javascript_module_system.cc',
        './modules/module_registry.cc',
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
      'cflags': [ '-std=c++11' ],
      'cflags_cc': [ '-fno-rtti', '-fno-exceptions'],
      'link_settings': {
        'libraries': [
          '-L<(PRODUCT_DIR)',
          '-ldl',
        ],
      },
      'msvs_settings': {
        'VCLinkerTool': {
          'SubSystem': '1', # /SUBSYSTEM:CONSOLE
        },
      },
    }, # end zarun
    {
      'target_name': 'zarun_js2c',
      'type': 'none',
      'toolsets': ['host'],
      'actions': [
        {
          'action_name': 'zarun_js2c',
          'inputs': [
            '<@(javascript_files)',
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/zarun_natives.h',
          ],
          'action': [
            'python',
            './tools/zarun_js2c.py',
            '<@(_outputs)',
            '<@(_inputs)',
          ],
        },
      ],
    }, # end zarun_js2c
  ],
}
