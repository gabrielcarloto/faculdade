package ui

import "net"

type LogMsg string

type ChatMsg struct {
	Addr    string
	Content string
	IsSent  bool
}

type ClientChangeMsg struct {
	Addr   string
	Joined bool
	Conn   net.Conn
}

type FileSavedMsg struct {
	Path string
}

type ErrorMsg error
