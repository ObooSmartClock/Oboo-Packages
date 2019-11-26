var result;

// base functions
function setup() {
  print('> starting setup function');
  result = popen('echo $((1+2))');
	print(' < setup function done');
}

function loop() {
  print ('loop function');
	print ('result = ' + result);
}
