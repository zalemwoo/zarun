(function(global){
if(global.console === undefined){
    var console = process.binding("console");
}
if(global.Lua === undefined){
    console.error("Usage: " + process.argv[0] + " lua_patch.js lua.vm.js lua_script.js");
    return;
}
var lua = new Lua.State();
lua.execute("js.global.console:log('normal log')");
})(this)
