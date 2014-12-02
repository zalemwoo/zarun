var global = this;

global.util = require("node_util");
global.os = requireNative("os");
global.console = console;
global.print = console.log;
global.define = define;
global.require = require;

this.global = global;
