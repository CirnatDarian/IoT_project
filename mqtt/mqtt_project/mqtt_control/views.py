import json
import paho.mqtt.client as mqtt
from django.shortcuts import render
from django.http import HttpResponse
from threading import Thread
from .models import TemperatureData, HumidityData, RelayData
import time

MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_USER = "mqtt_user"
MQTT_PASSWORD = "jmecher69"

mqtt_client = mqtt.Client()
mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)

def control_relay(request):
    command = request.GET.get('command', '')
    if command:
        result = mqtt_client.publish("esp32/command", command)
        if result[0] == 0:
            return HttpResponse(f"Comanda {command} trimisa!")
        else:
            return HttpResponse("Eroare la trimiterea comenzii.")
    else:
        return HttpResponse("Nu s-a trimis nicio comanda.")

def on_message(client, userdata, msg):
    if msg.topic == "esp32/data":
        try:
            payload = json.loads(msg.payload.decode("utf-8"))
            temperature = payload.get('temperatura', 'No temperature data')
            humidity = payload.get('umiditate', 'No humidity data')
            
            # Salveaza datele de temperatura si umiditate
            if temperature != 'No temperature data' and humidity != 'No humidity data':
                temp_data = TemperatureData(value=temperature)
                humidity_data = HumidityData(value=humidity)
                temp_data.save()
                humidity_data.save()
        except json.JSONDecodeError:
            print("Mesajul nu este un JSON valid:", msg.payload)

    if msg.topic == "esp32/command":
        try:
            payload = json.loads(msg.payload.decode("utf-8"))
            timestamp = payload.get('timestamp', 'No timestamp')
            relay_state = payload.get('command', 'No command')
            
            # Salveaza datele de control al releelor
            relay_data = RelayData(state=relay_state, timestamp=timestamp)
            relay_data.save()
        except json.JSONDecodeError:
            print("Mesajul nu este un JSON valid:", msg.payload)

mqtt_client.on_message = on_message

def start_mqtt_listener():
    mqtt_client.subscribe("esp32/data")
    mqtt_client.subscribe("esp32/command")
    mqtt_client.loop_forever()

def run_mqtt_thread():
    thread = Thread(target=start_mqtt_listener)
    thread.daemon = True
    thread.start()

def update_sensor_data():
    while True:
        time.sleep(60)
        mqtt_client.loop()

def start_sensor_update():
    update_thread = Thread(target=update_sensor_data)
    update_thread.daemon = True
    update_thread.start()

def index(request):
    latest_temperature = TemperatureData.objects.last()
    latest_humidity = HumidityData.objects.last()
    latest_relay = RelayData.objects.last()

    context = {
        'temperature': latest_temperature.value if latest_temperature else 'No data yet',
        'humidity': latest_humidity.value if latest_humidity else 'No data yet',
        'relay_timestamp': latest_relay.timestamp if latest_relay else 'No data yet'  # Adaugam timestamp-ul
    }

    return render(request, 'mqtt_control/index.html', context)

def home(request):
    return render(request, 'home.html')

run_mqtt_thread()
start_sensor_update()
