from flask import Flask, render_template, request, jsonify
import time
import json
import serial
app = Flask(__name__)

ser=serial.Serial(port='/dev/ttyS2',
baudrate=9600,
timeout=1)

@app.route('/')
def hello_world():
   return render_template('intro.html')
   
@app.route('/sensor')
def test_sensor():
   return render_template('sensor.html')

@app.route('/_referesh')
def myreferesh():
   x = ser.readline()
   try:
      y = json.loads(x)
   except:
      y = "{}"
   return jsonify(myresult=y)

@app.route('/_transfer')
def mytransfer():
   temp = request.args.get('inputT', 0, type=float) 
   light = request.args.get('inputL', 0, type=float)
   soil = request.args.get('inputS', 0, type=float)
   humidity = request.args.get('inputH', 0, type=float)
   data = {"Soil":[int(soil*100)],"Humidity":[int(humidity*100)],"Temp":[int(temp*100)],"Light":[int(light*100)]}
   for i in str(data):
      ser.write(i.encode())
         
if __name__ == '__main__':
   app.run(host='0.0.0.0',port=80)
