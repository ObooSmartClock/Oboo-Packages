var clientInfo = {
    state: {},
    stateLocation: '/root/state.json',
    deviceId: '',           // device ID
    dsRoot: '',             // root path to device shadow
    topics: {
        'subscribe': {
            'update': '',   // topic to recieve accepted shadow updates
            'updateRejected': '', // topic to receive rejected shadow updates
            'get': '',      // topic to recieve full shadow
            'delta': ''     // topic to receive delta between desired and reported
        },
        'publish': {
            'get': '',      // topic to trigger a full shadow read
            'update': ''    // topic to update the device shadow
        }
    }
};

function populateClientInfo(deviceId) {
    clientInfo.deviceId = deviceId;
    clientInfo.dsRoot = '$aws/things/' + deviceId + '/shadow';
    
    clientInfo.topics.subscribe.update  = clientInfo.dsRoot + '/update/accepted';   // $aws/things/${deviceId}/shadow/update/accepted
    clientInfo.topics.subscribe.updateRejected  = clientInfo.dsRoot + '/update/rejected';   // $aws/things/${deviceId}/shadow/update/rejected
    clientInfo.topics.subscribe.get     = clientInfo.dsRoot + '/get/accepted';      // $aws/things/${deviceId}/shadow/get/accepted
    clientInfo.topics.subscribe.delta   = clientInfo.dsRoot + '/update/delta';      // $aws/things/${deviceId}/shadow/update/delta
    
    clientInfo.topics.publish.get       = clientInfo.dsRoot + '/get';       // $aws/things/${deviceId}/shadow/get
    clientInfo.topics.publish.update    = clientInfo.dsRoot + '/update';    // $aws/things/${deviceId}/shadow/update
}

function remoteConnect() {
    connect(null, 8883, clientInfo.deviceId, function () {
        print('connection established');
        subscribe(clientInfo.topics.subscribe.delta, null, true);
        subscribe(clientInfo.topics.subscribe.update, null, true);
        subscribe(clientInfo.topics.subscribe.updateRejected, null, true);
        
        subscribe(clientInfo.topics.subscribe.get, function () {
            print('successfully subscribed to topic ' + clientInfo.topics.subscribe.get);
            publish(clientInfo.topics.publish.get, '', true);
        }, true);
    }, true);
}

function localConnect() {
    connect('localhost', 1883, null, function () {
        subscribe('/setAck');
    });
}

function objLoop (obj, callback) {
    Object.keys(obj).forEach(function(key) {
        if (typeof obj[key] === 'object') {
            objLoop(obj[key], callback);
        } else {
            callback(key, obj[key]);
        }
    });
}


// base functions
function setup() {
    readFile('/etc/certs/deviceId', '', function (err, data) {
        if (!err) {
            var deviceId = data.replace(/\n/gm, '');
            populateClientInfo(deviceId);
            
            remoteConnect();
            localConnect();
        }
    });
    
}

function loop() {
    
}

function onMessage(e) {
    if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('message! topic: ' + e.topic);
        // print('message! topic: ' + e.topic + ', value: ' + JSON.stringify(e.payload));
        
        if (e.topic === clientInfo.topics.subscribe.get) {
            clientInfo.state = e.payload.state.reported;
            print(JSON.stringify(clientInfo.state));
            // write full device shadow to fs
            writeFile(clientInfo.stateLocation, JSON.stringify(clientInfo.state), function (err) {
                if (!err) {
                    print('write was successful!')
                }
            });
        } else if (e.topic === clientInfo.topics.subscribe.delta) {
            print('delta!');
            var desired = e.payload.state;
            if (desired.config) {
                // publish local messages to change config on device
                objLoop(desired.config, function (cmd, val) {
                    publish('/set', JSON.stringify({
                        cmd: cmd,
                        value: val
                    }));
                });
                
                // update local state
                clientInfo.state.config = Object.assign(clientInfo.state.config, desired.config);
                // TODO: note this introduces a bug where the brightness object gets overwritten completely - potentially getting rid of some of the key-value pairs
                print(JSON.stringify(clientInfo.state));
                writeFile(clientInfo.stateLocation, JSON.stringify(clientInfo.state), function (err) {
                    // acknowledge reported state has changed  
                    if (!err) {
                        var pubObj = {
                            state: {
                                reported: {
                                    config: desired.config
                                }
                            }
                        };
                        publish(clientInfo.topics.publish.update, JSON.stringify(pubObj), true);
                    }
                });
            }
            // TODO: add handlers for alarms and cards arrays
        } else {
            print('value: ' + JSON.stringify(e.payload));
        }
        print(' ');
    }
}