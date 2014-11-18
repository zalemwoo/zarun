{
    'variables': {
    },

    'targets': [
    {
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        'base/base.gyp:*',
        'gin/gin.gyp:gin_shell',
        'zarun/zarun.gyp:zarun',
      ],
    }
    ],
}
