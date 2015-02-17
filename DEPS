# This file is automatically processed to create .DEPS.git which is the file
# that gclient uses under git.
#
# See http://code.google.com/p/chromium/wiki/UsingGit
#
# To test manually, run:
#   python tools/deps2git/deps2git.py -o .DEPS.git -w <gclientdir>
# where <gcliendir> is the absolute path to the directory containing the
# .gclient file (the parent of 'src').
#
# Then commit .DEPS.git locally (gclient doesn't like dirty trees) and run
#   gclient sync
# Verify the thing happened you wanted. Then revert your .DEPS.git change
# DO NOT CHECK IN CHANGES TO .DEPS.git upstream. It will be automatically
# updated by a bot when you modify this one.
#
# When adding a new dependency, please update the top-level .gitignore file
# to list the dependency's destination directory.

vars = {
  'chromium_git': 'https://chromium.googlesource.com',
  'githab_git': 'https://github.com/zalemwoo',
  'gitlab_git': 'http://gitlab.wwatwh.com:10080',
  
  'libcxx_revision': '1beaff680ee20c76e304824fc9240e6247e5a81e',
  'libcxxabi_revision': 'f0098574639866eebc5f277a6b00304fffece0c7',
  'clang_revision': '21c0f1864716dbe8d8d4061c18efbde7c49b6a6a',
#  'sfntly_revision': '1bdaae8fc788a5ac8936d68bf24f37d977a13dac',
#  'skia_revision': 'bc97c9378bf8b89cc17280a2a04a5c3a9405e6ab',

#  'build_revision': 'e8a433670f3a380a5d4316f4504dcf141dec5a05',
  'buildtools_revision': '5c5e924788fe40f7d6e0a3841ac572de2475e689',
  'base_revision': '9d73d5f6241cdc89817a1c49835f03528f284fc1',
  'net_revision': '4332822af288a44af960052ef48a33340d23e762',
  'crypto_revision': 'fcf4398c70205a4b0911914e3adec94ca4bd88a9',
  'sqlite_revision': 'e35e9749c8ccfe5bd5163ac264146b96c53e563e',
  'zlib_revision': 'afd8c4593c010c045902f6c0501718f1823064a3',

  'valgrind_revision': '12ad17c17b5099d5e6cd271a52b6caaad09640d2',
  'valgrind_deps_revision': '59886873b4b8258a8e0f098c23e1958e0d0c0a26',

#  'swarming_revision': 'd863df38343f8057dbddfe5543addcf19a49ce0f',
#  "angle_revision": "bc393df9cd8ae893da686533644da29a8c4911ae",
#  'pdfium_revision': 'eef005055d6aafc2cc6066af37ce47d8b15ec7cd',
  'boringssl_revision': 'a87de9b39b5a9b1d6e9ed6d18167946397581df1',
#  'lss_revision': '952107fa7cea0daaabead28c0e92d579bee517eb',
#  'nss_revision': 'bb4e75a43d007518ae7d618665ea2f25b0c60b63',
  'khronos_revision': '5ff9764c9d1e74c31691fefc150d4d919157288a',

  'mojo_revision': 'd32e1b379f51861fdeaf95c7beaf12803fd22c48',
  'third_party_mojo_revision': 'c9b8bb7093e150cad280ccd9b7b8533839f59872',

  'v8_revision': '2db40955e94d6ae946b1e12aca8bf25a4b3185df', # 4.2.70
  'gin_revision': 'bf03fbc4b57b95cd11974a4b79c30e735eb5bcf3',
  'sql_revision': 'eb1cdfdff64a0e47942f978579b725e9b3067448',
  'linenoise_revision': '0d9bd4eb1cad04588e078d18d57e2b47810d8083',
  'termcolor_revision': '63fc95fc9167be01212173cf32b3de0dfc6ba1b8',
}

# Only these hosts are allowed for dependencies in this DEPS file.
allowed_hosts = [
  'chromium.googlesource.com',
  'boringssl.googlesource.com',
  'pdfium.googlesource.com',
]

deps = {
  'src/buildtools':
    Var('chromium_git') + '/chromium/buildtools.git' + '@' +  Var('buildtools_revision'),

  'src/sdch/open-vcdiff':
    Var('chromium_git') + '/external/open-vcdiff.git' + '@' + '438f2a5be6d809bc21611a94cd37bfc8c28ceb33', # from svn revision 41

  'src/testing/gtest':
    Var('chromium_git') + '/external/googletest.git' + '@' + '8245545b6dc9c4703e6496d1efd19e975ad2b038', 

  'src/testing/gmock':
    Var('chromium_git') + '/external/gmock.git' + '@' + '29763965ab52f24565299976b936d1265cb6a271',

#  'src/third_party/angle':
#   Var('chromium_git') + '/angle/angle.git' + '@' +  Var('angle_revision'),

  'src/third_party/icu':
    Var('chromium_git') + '/chromium/deps/icu52.git' + '@' + 'd8b2a9d7b0039a4950ee008c5b1d998902c44c60', # from svn revision 292476

  'src/third_party/libc++/trunk':
    Var('chromium_git') + '/chromium/llvm-project/libcxx.git' + '@' +  Var('libcxx_revision'),

  'src/third_party/libc++abi/trunk':
    Var('chromium_git') + '/chromium/llvm-project/libcxxabi.git' + '@' +  Var('libcxxabi_revision'),
   
  'src/third_party/sqlite':
    Var('chromium_git') + '/chromium/src/third_party/sqlite.git' + '@' + Var('sqlite_revision'),

  'src/third_party/zlib':
    Var('chromium_git') + '/chromium/src/third_party/zlib.git' + '@' + Var('zlib_revision'),

  'src/tools/grit':
    Var('chromium_git') + '/external/grit-i18n.git' + '@' + 'a5890a8118c0c80cc0560e6d8d5cf65e5d725509',

  'src/tools/gyp':
    Var('chromium_git') + '/external/gyp.git' + '@' + '487c0b6ae8b44932e45347211bca0e8387718436', # from svn revision 1998

#  'src/tools/swarming_client':
#    Var('chromium_git') + '/external/swarming.client.git' + '@' +  Var('swarming_revision'),

  'src/tools/clang':
    Var('chromium_git') + '/chromium/src/tools/clang.git' + '@' + Var('clang_revision'),

  'src/tools/valgrind':
    Var('chromium_git') + '/chromium/src/tools/valgrind.git' + '@' + Var('valgrind_revision'),

  'src/third_party/valgrind-git':
    Var('chromium_git') + '/chromium/deps/valgrind.git' + '@' + Var('valgrind_deps_revision'),

  'src/base':
    Var('chromium_git') + '/chromium/src/base.git' + '@' +  Var('base_revision'),

  'src/net':
    Var('chromium_git') + '/chromium/src/net.git' + '@' +  Var('net_revision'),

  'src/crypto':
    Var('chromium_git') + '/chromium/src/crypto.git' + '@' +  Var('crypto_revision'),

  'src/third_party/boringssl/src':
    'https://boringssl.googlesource.com/boringssl.git' + '@' +  Var('boringssl_revision'),

#  'src/third_party/sfntly/cpp/src':
#    Var('chromium_git') + '/external/sfntly/cpp/src.git' + '@' +  Var('sfntly_revision'),

#  'src/third_party/skia':
#    Var('chromium_git') + '/skia.git' + '@' +  Var('skia_revision'),

#  'src/third_party/ots':
#    Var('chromium_git') + '/external/ots.git' + '@' + '98897009f3ea8a5fa3e20a4a74977da7aaa8e61a',

#  'src/third_party/brotli/src':
#    Var('chromium_git') + '/external/font-compression-reference.git' + '@' + '65cb3326e30ef8a67eb1d4411ec563e91be6e9ae',

  'src/third_party/yasm/source/patched-yasm':
    Var('chromium_git') + '/chromium/deps/yasm/patched-yasm.git' + '@' + '4671120cd8558ce62ee8672ebf3eb6f5216f909b',

#  'src/third_party/libjpeg_turbo':
#    Var('chromium_git') + '/chromium/deps/libjpeg_turbo.git' + '@' + '034e9a9747e0983bc19808ea70e469bc8342081f',

#  'src/third_party/smhasher/src':
#    Var('chromium_git') + '/external/smhasher.git' + '@' + 'e87738e57558e0ec472b2fc3a643b838e5b6e88f',

#  'src/third_party/pywebsocket/src':
#    Var('chromium_git') + '/external/pywebsocket/src.git' + '@' + 'cb349e87ddb30ff8d1fa1a89be39cec901f4a29c',

#  'src/third_party/mesa/src':
#    Var('chromium_git') + '/chromium/deps/mesa.git' + '@' + '9731cb962963bca8a05f3d0f6ea40c3a781f0537',

#  'src/third_party/pdfium':
#   'https://pdfium.googlesource.com/pdfium.git' + '@' +  Var('pdfium_revision'),

  'src/mojo':
    Var('chromium_git') + '/chromium/src/mojo.git' + '@' +  Var('mojo_revision'),

  'src/third_party/mojo/src':
    Var('chromium_git') + '/external/mojo.git' + '@' +  Var('third_party_mojo_revision'),
   
  'src/third_party/khronos':
    Var('chromium_git') + '/chromium/src/third_party/khronos.git' + '@' +  Var('khronos_revision'),
   
  'src/v8':
    Var('githab_git') + '/v8-git-mirror.git' + '@' +  Var('v8_revision'),
    
  'src/sql':
    Var('gitlab_git') + '/chromium/chromium.src.sql.git' + '@' +  Var('sql_revision'),
    
  'src/gin':
    Var('githab_git') + '/gin.git' + '@' +  Var('gin_revision'),

  'src/third_party/linenoise':
    Var('githab_git') + '/linenoise.git' + '@' +  Var('linenoise_revision'),

  'src/third_party/termcolor':
    Var('githab_git') + '/termcolor.git' + '@' +  Var('termcolor_revision'),
}


deps_os = {
  'unix': {
    # Linux, really.
#    'src/third_party/lss':
#      Var('chromium_git') + '/external/linux-syscall-support/lss.git' + '@' + Var('lss_revision'),

    # Note that this is different from Android's freetype repo.
#    'src/third_party/freetype2/src':
#     Var('chromium_git') + '/chromium/src/third_party/freetype2.git' + '@' + '495a23fce9cd125f715dc20643d14fed226d76ac',

    # Used for embedded builds. CrOS & Linux use the system version.
#    'src/third_party/fontconfig/src':
#     Var('chromium_git') + '/external/fontconfig.git' + '@' + 'f16c3118e25546c1b749f9823c51827a60aeb5c1',
  },
  'android': {
#    'src/third_party/colorama/src':
#     Var('chromium_git') + '/external/colorama.git' + '@' + '799604a1041e9b3bc5d2789ecbd7e8db2e18e6b8',

#    'src/third_party/jsr-305/src':
#        Var('chromium_git') + '/external/jsr-305.git' + '@' + '642c508235471f7220af6d5df2d3210e3bfc0919',

#    'src/third_party/android_tools':
#     Var('chromium_git') + '/android_tools.git' + '@' + 'ea50cccc11657404ce22cf928062ed1a3927eb39',

#    'src/third_party/freetype':
#       Var('chromium_git') + '/chromium/src/third_party/freetype.git' + '@' + 'a2b9955b49034a51dfbc8bf9f4e9d312149cecac',
  },
  'win': {
#    'src/third_party/nss':
#     Var('chromium_git') + '/chromium/deps/nss.git' + '@' + Var('nss_revision'),
#    'src/third_party/bison':
#     Var('chromium_git') + '/chromium/deps/bison.git' + '@' + '083c9a45e4affdd5464ee2b224c2df649c6e26c3',
#    'src/third_party/gperf':
#     Var('chromium_git') + '/chromium/deps/gperf.git' + '@' + 'd892d79f64f9449770443fb06da49b5a1e5d33c1',
  }
}


include_rules = [
  # Everybody can use some things.
  '+base',
  '+build',

  '+url',
  '+testing',
  '+third_party/icu/source/common/unicode',
  '+third_party/icu/source/i18n/unicode',
]


# checkdeps.py shouldn't check include paths for files in these dirs:
skip_child_includes = [
  'sdch',
  'skia',
  'testing',
  'third_party',
]


hooks = [
  {
    # This clobbers when necessary (based on get_landmines.py). It must be the
    # first hook so that other things that get/generate into the output
    # directory will not subsequently be clobbered.
    'name': 'landmines',
    'pattern': '.',
    'action': [
        'python',
        'src/build/landmines.py',
    ],
  },
#  {
    # Downloads an ARM sysroot image to src/arm-sysroot. This image updates
    # at about the same rate that the chrome build deps change.
    # This script is a no-op except for linux users who have
    # target_arch=arm in their GYP_DEFINES.
#    'name': 'sysroot',
#    'pattern': '.',
#    'action': ['python', 'src/build/linux/install-arm-sysroot.py',
#               '--linux-only'],
#  },
  {
    # Pull clang if needed or requested via GYP_DEFINES.
    'name': 'clang',
    'pattern': '.',
    'action': ['python', 'src/tools/clang/scripts/update.py', '--if-needed'],
  },
  {
    # Update LASTCHANGE. This is also run by export_tarball.py in
    # src/tools/export_tarball - please keep them in sync.
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python', 'src/build/util/lastchange.py',
               '-o', 'src/build/util/LASTCHANGE'],
  },
  # Pull GN binaries. This needs to be before running GYP below.
  {
    'name': 'gn_linux32',
    'pattern': '.',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=linux*',
                '--no_auth',
                '--bucket', 'chromium-gn',
                '-s', 'src/buildtools/linux32/gn.sha1',
    ],
  },
  {
    'name': 'gn_linux64',
    'pattern': '.',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=linux*',
                '--no_auth',
                '--bucket', 'chromium-gn',
                '-s', 'src/buildtools/linux64/gn.sha1',
    ],
  },
  {
    'name': 'gn_mac',
    'pattern': '.',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=darwin',
                '--no_auth',
                '--bucket', 'chromium-gn',
                '-s', 'src/buildtools/mac/gn.sha1',
    ],
  },
  {
    'name': 'gn_win',
    'pattern': '.',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=win*',
                '--no_auth',
                '--bucket', 'chromium-gn',
                '-s', 'src/buildtools/win/gn.exe.sha1',
    ],
  },
  # Pull clang-format binaries using checked-in hashes.
  {
    'name': 'clang_format_linux',
    'pattern': '.',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=linux*',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/linux64/clang-format.sha1',
    ],
  },
  # Pull binutils for linux, enabled debug fission for faster linking /
  # debugging when used with clang on Ubuntu Precise.
  # https://code.google.com/p/chromium/issues/detail?id=352046
#  {
#    'name': 'binutils',
#    'pattern': 'src/third_party/binutils',
#    'action': [
#        'python',
#        'src/third_party/binutils/download.py',
#    ],
#  },
  # Pull eu-strip binaries using checked-in hashes.
  {
    'name': 'eu-strip',
    'pattern': '.',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=linux*',
                '--no_auth',
                '--bucket', 'chromium-eu-strip',
                '-s', 'src/build/linux/bin/eu-strip.sha1',
    ],
  },
  {
    # Ensure that we don't accidentally reference any .pyc files whose
    # corresponding .py files have already been deleted.
    'name': 'remove_stale_pyc_files',
    'pattern': 'src/tools/.*\\.py',
    'action': [
        'python',
        'src/tools/remove_stale_pyc_files.py',
        'src/tools',
    ],
  },
  {
    'name': 'symbol_link_valgrind',
    'action': [
        'sh',
        '-c',
        'cd src/third_party && test \( ! -h valgrind \) && ln -s ./valgrind-git/binaries ./valgrind || true'
     ],
  },
]
