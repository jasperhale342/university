from socket import *
import sys
import time
import datetime
UDP_IP = sys.argv[1]
port = int(sys.argv[2])
sock = socket(AF_INET, SOCK_DGRAM)
#t1 = time.time()
sock.settimeout(1)
for i in range(1, 11):
	message = 'ping ' + str(i)
	t1 = datetime.datetime.now().time()
	t0 = time.time()
	print(message + " "+ str(t1))	
	sock.sendto(message, (UDP_IP, port))
	try:
		data, addr = sock.recvfrom(1024)
	except:
		print("Request timed out")
		continue
	t2 = time.time()
	t3 = str(t2-t0)
	print(data + ' RTT: ' + t3 +' seconds')
