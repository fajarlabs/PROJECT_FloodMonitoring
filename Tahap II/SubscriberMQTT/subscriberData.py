import struct
from time import sleep
import psycopg2
import time
import threading
import paho.mqtt.client as mqtt

def connectMQTT():
	global CLIENT, MQTT_HOST, MQTT_PORT, MQTT_TIMEOUT

	CLIENT = mqtt.Client()
	CLIENT.on_connect = on_connect
	CLIENT.on_message = on_message
	CLIENT.connect(MQTT_HOST, MQTT_PORT, MQTT_TIMEOUT)
	CLIENT.loop_forever()

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
	global BASE_TOPIC

	print("Connected with result code "+str(rc))
	# get raw mirror data 
	client.subscribe(BASE_TOPIC+"/raw_data")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    cleanPayload = clearStartEndSymbol(msg.payload)
    itemParse(cleanPayload, str(msg.payload))

# parse item series
def itemParse(receive_data, signal_message):
	global DEBUG

	serialNumber = ''
	reqCommand = ''
	data = ''

	fuelSensor = ''
	voltageSensor = ''
	tickRain = ''
	floodLevel = ''

	try :
		# =============== pecah data utama dengan delimiter tilde ====================
		dataList = receive_data.split("~")

		try :
			serialNumber = dataList[0]
		except Exception as e :
			if DEBUG :
				print(e)
		try :
			reqCommand = dataList[1]
		except Exception as e :
			if DEBUG :
				print(e)
		try :
			data = dataList[2]
		except Exception as e :
			if DEBUG :
				print(e)

		# ================ pecah data item dengan delimiter dollar ===================
		splitItemByDollar = data.split('$')

		try :
			fuelSensor = splitItemByDollar[0]
		except Exception as e :
			if DEBUG :
				print(e)
		try :
			voltageSensor = splitItemByDollar[1]
		except Exception as e :
			if DEBUG :
				print(e)
		try :
			tickRain = splitItemByDollar[2]
		except Exception as e :
			if DEBUG :
				print(e)
		try :
			floodLevel = splitItemByDollar[3]
		except Exception as e :
			if DEBUG :
				print(e)

		# Lakukan sesuatu untuk variable-variable diatas, di simpan di database atau lainnya
		# lihat datanya
		print(f" {fuelSensor} - {voltageSensor} - {tickRain} - {floodLevel} ")
		
	except Exception as e :
		pass

# receive message from raw mqtt
def clearStartEndSymbol(raw_data):
	global ALLOW_PREVIX

	is_allow = False
	result = ""
	try :
		raw_data = raw_data.strip().decode('utf_8')
		for prev in ALLOW_PREVIX :
			if(prev in raw_data):
				is_allow = True
				raw_data = raw_data.replace(prev,"")
		raw_data = raw_data.replace(">>","")
		if is_allow :
			result = raw_data
		else :
			raw_data = ""
			result = ""
	except Exception as e :
		print(e)

	return result

if __name__ == "__main__":

	# Debug
	DEBUG = False

	# previx serial
	ALLOW_PREVIX = ["<<DATA"]

	# ================ DB ACCOUNT ==========================
	# CONNECTION = None
	# USERDB = "postgres"
	# PASSDB = "postgres"
	# HOSTDB = "127.0.0.1"
	# PORTDB = "5432"
	# DBNAME = "postgres"
	# ======================================================

	# ================ MQTT ACCOUNT ========================
	CLIENT = None
	MQTT_HOST = "test.mosquitto.org"
	MQTT_PORT = 1883
	MQTT_TIMEOUT = 60
	# ======================================================
	# ================ TOPIC ===============================
	BASE_TOPIC = "/mytopic"
	# ======================================================

	try :
		# start MQTT broadcast
		tMQTT = threading.Thread(target=connectMQTT, args=())
		tMQTT.start()
	except Exception as e :
		print(e)
