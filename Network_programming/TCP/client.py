from socket import *
import sys
if len(sys.argv) < 3:
    print('Need more information')
    exit()
serverIP = sys.argv[1]
Port = sys.argv[2]
file = sys.argv[3]
#print(serverIP + " "+ Port " " + file)
path = "GET /"+ file + " HTTP/1.1\n\n" 
s = socket(AF_INET, SOCK_STREAM)
s.connect((serverIP, int(Port)))
s.send(path.encode())
data = s.recv(1024)
output= data
while data:
    data = s.recv(1024)
    output = output + data
print(output)
s.close()
