
import numpy as np

import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore

from flask import Flask, request, jsonify, render_template
from flask_mqtt import Mqtt
import json

import matplotlib
import matplotlib.pyplot as plt


app = Flask(__name__)

#MQTT setup.
app.config['MQTT_BROKER_URL'] = 'maqiatto.com'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = 'gate.tang@gmail.com'
app.config['MQTT_PASSWORD'] = 'letmein'
app.config['MQTT_REFRESH_TIME'] = 1.0  # refresh time in seconds
mqtt = Mqtt(app)

cred = credentials.Certificate("iotMaker_Key.json")
firebase_admin.initialize_app(cred)

db = firestore.client()

doc_ref = db.collection(u'users').document(u'inputData')
date_ref = db.collection(u'users').document (u'date')

displaySignal = ""

@app.route('/readdata', methods=['GET']) #path of link. 
def respond():
    doc = doc_ref.get()
    a = doc.to_dict()
    return json.dumps(a)

@app.route("/", methods = ['POST', 'GET'])
def digitalSignal():
    global displaySignal
    if request.method == 'POST':
        digitalSignal = request.form.get('signalState')
        if digitalSignal == "1":
            displaySignal = "ON"
        if digitalSignal == None:
            digitalSignal = "0"
            displaySignal = "OFF"
        mqtt.publish('gate.tang@gmail.com/digitalSignal', payload = digitalSignal, qos=0, retain=False)
        print("Digital State: " + digitalSignal) # for debugging

        analogSignal = request.form.get('analogSignal')
        mqtt.publish('gate.tang@gmail.com/analogSignal', payload = analogSignal, qos=0, retain=False)
        print (analogSignal)

        return render_template("home.html", displaySignal=displaySignal)
    return render_template("home.html", displaySignal=displaySignal)
    
@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt.subscribe('gate.tang@gmail.com/input')
       
if __name__ == '__main__':
# Threaded option to enable multiple instances for multiple user access support
    app.run(threaded=True, port=5000)
