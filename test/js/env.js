var envs = Object.getOwnPropertyNames(process.env);
for(var env in envs){
    console.log(envs[env] + " = " + process.env[envs[env]]);
}
