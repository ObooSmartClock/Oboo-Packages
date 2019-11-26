
function t(msg) {
    print('\n=== '+msg+ ' ===')
}



//t ("Test: mpg123 & ");
//var result = popen('mpg123 --loop -1 SampleAudio_0.7mb.mp3 &');
//print ("JS " + result);
/*
t ("Test: top & ");
var result = system('top &');
print ("JS " + result);*/



t ("Test: nonblock1");
var result1 = launchProcess('sleep 25');
print("JS " + result1);

t ("Test: nonblock2");
var result2 = launchProcess('sleep 10');
print("JS " + result2);

t ("Test: nonblock3");
var result3 = launchProcess('sleep 15');
print("JS " + result3);

t ("Test: nonblock4");
var result4 = launchProcess('sleep 15');
print("JS " + result4);

t ("Test: nonblock5");
var result5 = launchProcess('sleep 15');
print("JS " + result5);

t ("Test: nonblock6");
var result6 = launchProcess('sleep 15');
print("JS " + result6);

t ("Test: check proc");
var ju = checkProcess(result5);
print("JS " + ju);

//sleep(10);

t ("Test: kill " + result1);
var resultk1 = killProcess(result1);
//var result = nonblock(result);
print("JS " + resultk1);

t ("Test: kill " + result3);
var resultk1 = killProcess(result3);
//var result = nonblock(result);
print("JS " + resultk1);


sleep(3);

t ("Test: nonblock7");
var result7 = launchProcess('sleep 15');
print("JS " + result7);

t ("Test: nonblock8");
var result8 = launchProcess('sleep 15');
print("JS " + result8);

sleep(5);

t ("Test: nonblock9");
var result9 = launchProcess('sleep 15');
print("JS " + result9);

t ("Test: check proc");
var juc = checkProcess('43111');
print("JS " + juc);

t("Test: end 0");
result5 = launchProcess('end 0');
print("JS5 " + result5);


/*	t ("Test: nonblock1");
var result = nonblock('top');
print("JS " + result);
*/

//t ("Test: top & echo $!");
//var result = popen('top & echo $!');
//print ("JS " + result);

/*
t ("Test: echo !$");
var result = popen('echo !$');
print ("JS " + result);*/


/*
t ("Test: cat non existing file");
var result = popen('cat jublje.txt');
print ("JS " + result);
*/
