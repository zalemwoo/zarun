zarun
===
**WIP**

This repo uses gclient to manage dependencies, so to build things from this
repo you have to first download depot_tools and make sure it is in your path:

http://www.chromium.org/developers/how-tos/install-depot-tools

Then, create a directory somewhere for your checkout, cd into it,
and run the following commands:

```bash
$ mkdir zarun && cd zarun
$ gclient config --name=src https://github.com/zalemwoo/zarun.git
$ gclient sync
```

build
```bash
$ gn gen out/Debug(or Release or anything else)
$ ninja -C out/Debug zarun
```

build(64bit, for more run "gn help")
```bash
$ gn gen out/Debug --args="cpu_arch=\"x64\""
$ ninja -C out/Debug zarun
