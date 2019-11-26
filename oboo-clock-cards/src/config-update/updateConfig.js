
function setValueIfUndefined( parent, variableName, value){
    parent[variableName] = (parent[variableName] === undefined) ? value : parent[variableName];
}

function updateConfig() {
    readFile('/etc/config.json', '', function (err, data) {
        var config;
        if (!err) {
            try { 
                config = JSON.parse(data);
            } catch(e) {
                print(e); // error in the above string!
                config = {}; 
                // return null;
            }
        } else {
            config = {}; 
        }
        setValueIfUndefined(config,"config", {});
        setValueIfUndefined(config["config"],"hourMode", 12);
        setValueIfUndefined(config["config"],"dayNightLed", true);
        setValueIfUndefined(config["config"],"location", "Toronto,CA");
        setValueIfUndefined(config["config"],"brightnessTime", 1);
        setValueIfUndefined(config["config"],"brightnessInfo", 7);
        setValueIfUndefined(config["config"],"tz", "GMT");
        setValueIfUndefined(config["config"],"timezone", "GMT0");
        setValueIfUndefined(config["config"],"autoTimeZone", false);

        setValueIfUndefined(config,"cards",{}); // Why are these a dictionary with numbers for names?
        setValueIfUndefined(config["cards"],"0", {});
        setValueIfUndefined(config["cards"]["0"],"name", "weather");
        setValueIfUndefined(config["cards"]["0"],"enabled", true);
        setValueIfUndefined(config["cards"]["0"],"id", 0);
        setValueIfUndefined(config["cards"]["0"],"tempUnit", "fahrenheit");
        setValueIfUndefined(config["cards"]["0"],"card", "Weather");
        setValueIfUndefined(config["cards"]["0"],"location", "Toronto,CA");
        setValueIfUndefined(config["cards"]["0"],"distanceUnit", "imperial");

        setValueIfUndefined(config["cards"],"1", {});
        setValueIfUndefined(config["cards"]["1"],"name", "timer");
        setValueIfUndefined(config["cards"]["1"],"enabled", true);

        setValueIfUndefined(config["cards"],"2", {});
        setValueIfUndefined(config["cards"]["2"],"name", "music");
        setValueIfUndefined(config["cards"]["2"],"enabled", true);

        setValueIfUndefined(config["cards"],"3", {});
        setValueIfUndefined(config["cards"]["3"],"name", "calendar");
        setValueIfUndefined(config["cards"]["3"],"enabled", true);
    
        var newConfigStr = JSON.stringify(config, null, 2);
        print(newConfigStr);
        writeFile('/etc/config.json', newConfigStr, function (err, data) {
            if (err) throw err;
            print('Saved!');
        });
    });           
}                 

function setup() {
  updateConfig();
}           

function loop() { 
   return -1;
}                 
