var global = this;

global.node_util = require("node_util");
node_util.process = requireNative("process");
global.console = console;
global.print = console.log;
global.define = define;
global.require = require;

this.global = global;
