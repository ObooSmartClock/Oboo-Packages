var pid;
var numSeconds = 0;
var prevTime = 0;

function launchProcess (cmd) {
	print('launching new process');
	var newPid = nonblock('sleep 60');
	print('process pid is ' + newPid);

	return newPid;
}

// base functions
function setup() {
  print('> starting setup function');
  pid = launchProcess('sleep 60');
	print(' < setup function done');
}

function loop() {
	var currTime = new Date().getTime()
	if (currTime - prevTime > 1000) {
		print('second #' + numSeconds);

		// launch a new process
		if (numSeconds % 5 === 0 ) {
			pid = launchProcess('sleep 60');
		}

		// kill a process
		// if (numSeconds === 10) {
		// 	print('killing pid ' + pid);
		// 	var result = nonblock('kill -9 ' + pid);
		// }

		// increment
		prevTime = currTime;
		numSeconds++;
	}

}
