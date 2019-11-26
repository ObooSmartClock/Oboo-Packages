import time, os
import unittest
import paho.mqtt.client as mqtt
import subprocess
import json

dir_path = os.path.dirname(os.path.realpath(__file__))

count = 0
cardReady = False

client = mqtt.Client()
client.connect('localhost')
client.subscribe('/card')
client.subscribe('/audio')

messageBuff = []
lastCardUpdate = {}
lastAudioUpdate = {}

def resetBuff () :
    global messageBuff
    global lastCardUpdate
    global lastAudioUpdate
    messageBuff = []
    lastCardUpdate = {}
    lastAudioUpdate = {}

def on_message(client, userdata, message):
    payload = json.loads(message.payload)
    if message.topic == '/card':
        if payload['cmd'] == 'new_card':
            resp = {
                "action": "create",
                "attention": payload['responseTopic'],
                "cardId": 0
            }
            client.publish('/cardResponse', json.dumps(resp))
            
            client.publish('/cardResponse', json.dumps({
                "action":"select", 
                "cardId": 0 }))
            global cardReady
            cardReady = True
            
        if payload['cmd'] == 'update_card':
            global messageBuff
            global lastCardUpdate
            messageBuff.append(payload)
            lastCardUpdate = payload
    elif message.topic == '/audio':
        global lastAudioUpdate
        lastAudioUpdate = payload

    
client.on_message=on_message     

client.loop_start()    #start the loop

class TestTimer(unittest.TestCase):
    
    def setUp(self):
        self.myProcess = subprocess.Popen(['oboo-runtime/ort', '%s/timer-card/timer.js'%(dir_path)])
        while True:
            if cardReady:
                print 'Card Ready ...'
                break
            else:
                time.sleep(0.5)
          
    def tearDown(self):
        self.myProcess.kill()
        global cardReady
        cardReady = False

    def test_increase(self):
        for i in range(10):
            client.publish('/button', json.dumps({"action": "press", "id": 3}))
            time.sleep(0.1)
        time.sleep(0.5)
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '20:00')
        resetBuff()
     
    def test_decrease(self):
        for i in range(10):
            client.publish('/button', json.dumps({"action": "press", "id": 2}))
            time.sleep(0.1)
        time.sleep(0.5)
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '00:00')
        resetBuff()
    
    def test_increase_max(self):
        for i in range(70):
            client.publish('/button', json.dumps({"action": "press", "id": 3}))
            time.sleep(0.1)
        time.sleep(0.5)    
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '60:00')
        resetBuff() 
        
    def test_decrease_min(self):
        for i in range(30):
            client.publish('/button', json.dumps({"action": "press", "id": 2}))
            time.sleep(0.1)
        time.sleep(0.5)    
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '00:00')
        resetBuff() 
    
    def test_start(self):
        client.publish('/button', json.dumps({"action": "press", "id": 1}))
        time.sleep(3)
            
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '09:57')
        resetBuff()
    
    def test_stop(self):
        client.publish('/button', json.dumps({"action": "press", "id": 1}))
        time.sleep(3)
        client.publish('/button', json.dumps({"action": "press", "id": 0}))
        time.sleep(2)
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '10:00')
        time.sleep(2)
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '10:00')
        resetBuff()
    
    def test_expire(self):
        client.publish('/test/set3', '')
        time.sleep(0.1)
        # start the timer
        client.publish('/button', json.dumps({"action": "press", "id": 1}))
        time.sleep(4)
        self.assertEqual(lastAudioUpdate['action'], 'play')
        self.assertTrue(len(lastAudioUpdate['file']) > 0)
        time.sleep(2)
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '00:00')
    
    def test_modify_while_running(self):
        client.publish('/test/set3', '')
        time.sleep(0.1)
        # start the timer
        client.publish('/button', json.dumps({"action": "press", "id": 1}))
        time.sleep(0.5)
        # increment the timer
        for i in range(3):
            client.publish('/button', json.dumps({"action": "press", "id": 3}))
            time.sleep(0.1)
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '03:02')
        
        # decrement the timer
        for i in range(6):
            client.publish('/button', json.dumps({"action": "press", "id": 2}))
            time.sleep(0.1)
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '00:00')
        self.assertEqual(lastAudioUpdate['action'], 'play')
        self.assertTrue(len(lastAudioUpdate['file']) > 0)
    
    def test_restart_after_expiry(self):
        client.publish('/test/set3', '')
        time.sleep(0.1)
        # start the timer
        client.publish('/button', json.dumps({"action": "press", "id": 1}))
        time.sleep(4)
        self.assertEqual(lastAudioUpdate['action'], 'play')
        self.assertTrue(len(lastAudioUpdate['file']) > 0)
        time.sleep(2)
        # start the timer again
        client.publish('/button', json.dumps({"action": "press", "id": 1}))
        time.sleep(2)
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '00:01')
    
    def test_stop_after_expiry(self):
        client.publish('/test/set3', '')
        time.sleep(0.1)
        # start the timer
        client.publish('/button', json.dumps({"action": "press", "id": 1}))
        time.sleep(4)
        self.assertEqual(lastAudioUpdate['action'], 'play')
        self.assertTrue(len(lastAudioUpdate['file']) > 0)
        time.sleep(2)
        # hit stop
        client.publish('/button', json.dumps({"action": "press", "id": 0}))
        time.sleep(2)
        self.assertEqual(lastCardUpdate['elements'][0]['value'], '00:03')
    
    def test_increment_while_running(self):
        # start the timer
        client.publish('/button', json.dumps({"action": "press", "id": 1}))
        time.sleep(4)
        # increment many times
        for i in range(70):
            client.publish('/button', json.dumps({"action": "press", "id": 3}))
            time.sleep(0.1)
        time.sleep(0.5)    
        self.assertEqual(lastCardUpdate['elements'][0]['value'].split(':')[0], '59')

if __name__ == '__main__':
    # time.sleep(40)
    unittest.main()
    