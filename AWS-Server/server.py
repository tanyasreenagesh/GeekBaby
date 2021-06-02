from flask import Flask
from flask import request, render_template, Response, session
from flask.sessions import SecureCookieSessionInterface
import time
import json

# Track sensor readings
sound = 0
light = 0
humidity = 0.00
temp = 0.00
airQ = 0.00
isCry = 0

# Parental control flags
speakerFlag = 0
fanFlag = 0
lightFlag = 0


app = Flask(__name__)

# Root directory
@app.route("/")
def hello():
        global sound, light, humidity, temp, airQ, isCry, speakerFlag, fanFlag, lightFlag
        print("Sound        =", request.args.get("sound"))
        print("Light        =", request.args.get("light"))
        print("Humidity     =", request.args.get("humidity"))
        print("Temperature  =", request.args.get("temp"))
        print("Air Quality  =", request.args.get("airQ"))
        print("Is Crying    =", request.args.get("isCry"))
        print("Time: ", time.time())
        print("sFlag =", speakerFlag)
        print("fFlag =", fanFlag)
        print("lFlag =", lightFlag)
        sound = request.args.get("sound")
        light = request.args.get("light")
        humidity = request.args.get("humidity")
        temp = request.args.get("temp")
        airQ = request.args.get("airQ")
        isCry = request.args.get("isCry")


        return str(speakerFlag) + str(fanFlag) + str(lightFlag)



# /data directory: Contains button and data visualization functionality
@app.route("/data", methods=['GET', 'POST'])
def webUI():
        global speakerFlag, fanFlag, lightFlag

        # Parental controls implementation
        if request.method == 'POST':

                print("POST registered")
                if request.form['button3'] == 'Start Rhyme':
                        print("\nSpeaker On\n")
                        speakerFlag = 1
                if request.form['button3'] == 'Stop Rhyme':
                        print("\nSpeaker Off\n")
                        speakerFlag = 0
                if request.form['button3'] == 'Switch Fan On':
                        fanFlag = 1
                if request.form['button3'] == 'Switch Fan Off':
                        fanFlag = 0
                if request.form['button3'] == 'Low Light':
                        lightFlag = 1
                if request.form['button3'] == 'Bright Light':
                        lightFlag = 2
                if request.form['button3'] == 'Turn Light Off':
                        lightFlag = 0

        elif request.method == 'GET':
                return render_template('webUI.html')
        return render_template('webUI.html')


@app.route('/data/isCry')
def cry_data():
        def generate_cry_data():
                while True:
                        json_data = json.dumps({'value':isCry})
                        yield f"data:{json_data}\n\n"
                        time.sleep(3)
        return Response(generate_cry_data(), mimetype='text/event-stream')


@app.route('/data/sound')
def sound_data():
        def generate_sound_data():
                while True:
                        json_data = json.dumps({'time':time.strftime("%H:%M:%S",time.localtime()), 'value':sound})
                        yield f"data:{json_data}\n\n"
                        time.sleep(3)
        return Response(generate_sound_data(), mimetype='text/event-stream')


@app.route('/data/light')
def light_data():
        def generate_light_data():
                while True:
                        json_data = json.dumps({'time':time.strftime("%H:%M:%S",time.localtime()), 'value':light})
                        yield f"data:{json_data}\n\n"
                        time.sleep(3)
        return Response(generate_light_data(), mimetype='text/event-stream')


@app.route('/data/humidity')
def humidity_data():
        def generate_humidity_data():
                while True:
                        json_data = json.dumps({'time':time.strftime("%H:%M:%S",time.localtime()), 'value':humidity})
                        yield f"data:{json_data}\n\n"
                        time.sleep(3)
        return Response(generate_humidity_data(), mimetype='text/event-stream')


@app.route('/data/temp')
def temp_data():
        def generate_temp_data():
                while True:
                        json_data = json.dumps({'time':time.strftime("%H:%M:%S",time.localtime()), 'value':temp})
                        yield f"data:{json_data}\n\n"
                        time.sleep(3)
        return Response(generate_temp_data(), mimetype='text/event-stream')


@app.route('/data/airQ')
def airQ_data():
        def generate_airQ_data():
                while True:
                        json_data = json.dumps({'time':time.strftime("%H:%M:%S",time.localtime()), 'value':airQ})
                        yield f"data:{json_data}\n\n"
                        time.sleep(3)
        return Response(generate_airQ_data(), mimetype='text/event-stream')

if __name__ == '__main__':
        app.run()
