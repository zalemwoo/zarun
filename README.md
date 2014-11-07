zarun
===

This repo uses gclient to manage dependencies, so to build things from this
repo you have to first download depot_tools and make sure it is in your path:

http://www.chromium.org/developers/how-tos/install-depot-tools

Then, create a directory somewhere for your checkout, cd into it,
and run the following commands:

```bash
$ mkdir zarun && cd zarun
$ echo "solutions = [{u'managed': False, u'name': u'src', u'url': u'https://github.com/zalemwoo/zarun.git', u'custom_deps': {}, u'deps_file': u'DEPS', u'safesync_url': u''}]"  > .gclient
$ gclient sync
```

build
```bash
$ ./gyp_zarun
$ ninja -C out/Debug
```
