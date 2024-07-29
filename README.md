# FT_IRC
Este proyecto implementa un servidor IRC (Internet Relay Chat) basado en el protocolo IRC, que permite a los usuarios conectarse, crear canales, enviar mensajes privados y realizar varias otras operaciones típicas de un sistema IRC.

## Ejecución
```
./ircserv <puerto> <password>
```
Donde <puerto> es el número de puerto en el que deseas que el servidor escuche las conexiones entrantes y <password> la contrasña para poder conectarse al Servidor.

## Conexión de clientes
Los clientes pueden conectarse al servidor utilizando un cliente IRC estándar (como irssi, HexChat, mIRC, etc.) especificando la IP del servidor y el puerto.

## Comandos soportados
1. **PASS:** Autenticación con contraseña.
2. **NICK:** Establecer o cambiar el apodo del usuario.
3. **USER:** Establecer el nombre de usuario del cliente.
4. **JOIN:** Unirse a un canal.
5. **PRIVMSG:** Enviar un mensaje privado a otro usuario o canal.
6. **MODE:** Gestionar modos de canal y usuario (+i/-i, +l/-l, +k/-k, +t/-t, +o/-o).
7. **KICK:** Expulsar a un usuario de un canal.
8. **INVITE:** Invitar a un usuario a un canal.

## BOT
Existe un BOT en el canal #42bot que enviará un mensaje aleatorio cada vez que interactúes con él. Para usarlo, simplemente únete al canal #42bot y envía un mensaje.
