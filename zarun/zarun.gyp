{
  'variables': {
    'chromium_code': 1,
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
      ],
      'sources': [
        './backend/backend_context_delegate.cc',
        './backend/backend_thread.cc',
        './backend/backend_application.cc',
        
        './modules/module_registry.cc',
        './modules/module_provider.cc',
        './modules/builtin_module_provider.cc',
        './modules/cpp/console.cc',
        './modules/cpp/process.cc',
        './modules/cpp/gc.cc',
        
        './switches.cc',
        './readline_editor.cc',
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
#          '-lreadline',
          '-ldl',
        ],
      },
      'msvs_settings': {
        'VCLinkerTool': {
          'SubSystem': '1', # /SUBSYSTEM:CONSOLE
        },
      },
    },
  ],
}
