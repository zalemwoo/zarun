var global = this;

global.os = requireNative("os");
global.subprocess = requireNative("subprocess");
global.util = require("node_util");
global.console = console;
global.print = console.log;
global.define = define;
global.require = require;

this.global = global;
