var result;

// base functions
function setup() {
    serialReadByteArray();
  
  var ret = serialReadByteArray();
  print('returned array with length: ' + ret.length + ' values: ' + ret);
  for (var i = 0; i < ret.length; i++) {
    print(ret[i]);
  }
}

function loop() {
    return -1;
}