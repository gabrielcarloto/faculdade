package ui

import "net"

type ClientActions interface {
	RequestFile(path string) error
	SendChat(message string) error
	CloseConnection()
}

type ServerActions interface {
	Broadcast(message string)
	SendTo(conn net.Conn, message string)
}
