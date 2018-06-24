# BEGASEP
Betting game server protocol

Program description in short

A betting game server accepting clients on tcp sockets, letting the
clients pick a number within a published range, and every 15 seconds
randomize a winning number, and then letting the clients know if they
won or not.



Description of the program

The program must accept IPv4 connections on TCP port 2222 and handle up
to BEGASEP_NUM_CLIENTS clients. The code must allow for this define to 
range from 1 to 64500.

An unique client id must be generated upon receipt of a BEGASEP_OPEN
message, this client id must never collide with another connected client.

If the maximum number of simultaenous clients is reached the server
must tear down the connection, else it must respond to the BEGASEP_OPEN
message with a BEGASEP_ACCEPT message with the allocated client id and
the allowed betting number range.
When the client has sent a BEGASEP_BET message, which must follow no
other message than BEGASEP_ACCEPT and have a betting number within
the published range, the server is to include that client in the next
betting run.  The client must only send the BEGASEP_BET message once,any 
protocol breaches must immediately result in a connection teardown by the server.

A betting run must be performed every 15 seconds but only if there are
clients connected.

The betting run must generate a random number within the published
betting number range and send out the winning number to all clients
with the BEGASEP_RESULT message, indicating for each client if they
won or not.  Each winner should also be printed on a seperate line to
stdout, syslog, or similar.  The connection to each client that betted
in this run must then be closed.

The program must be able to run "forever" without interaction.
The program must be size optimized and shall allow the change of 
BEGASEP_NUM_CLIENTS on the fly, the changed value will be used in
next draw.

