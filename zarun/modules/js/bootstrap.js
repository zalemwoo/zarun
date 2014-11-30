var global = this;

global.process = requireNative("process");
global.console = console;
global.print = console.log;
global.define = define;
global.require = require;

this.global = global;
