var global = this;

global.console = console;
global.print = console.log;
global.define = define;
global.require = require;

requireAsync("process").then(function(process) { global.process = process; });

this.global = global;
