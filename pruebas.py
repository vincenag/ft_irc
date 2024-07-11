import socket

# Configuración del servidor
SERVER = "localhost"
PORT = 4444

# Conectar al servidor
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((SERVER, PORT))

# Enviar comandos NICK y USER
client.send("PASS kali\n".encode())
client.send("NICK ruben\n".encode())
client.send("USER miUsername miHostname miServername miRealname\n".encode())

# Recibir y mostrar la respuesta del servidor
response = client.recv(4096)
print(response.decode())

client.close()