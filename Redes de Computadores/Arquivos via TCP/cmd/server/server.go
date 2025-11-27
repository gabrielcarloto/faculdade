package main

import (
	"bufio"
	"errors"
	"fmt"
	"log"
	"net"
	"os"
	"sync"

	"arquivos-tcp/internal/protocol"
	"arquivos-tcp/internal/ui"

	tea "github.com/charmbracelet/bubbletea"
)

type Server struct {
	clients   map[net.Conn]*Client
	broadcast chan protocol.Message
	mu        sync.Mutex
	program   *tea.Program
}

type Client struct {
	writer *bufio.Writer
}

func main() {
	if err := os.MkdirAll("out", 0o755); err != nil {
		panic(err)
	}

	server := &Server{
		clients:   make(map[net.Conn]*Client),
		broadcast: make(chan protocol.Message),
		mu:        sync.Mutex{},
	}

	model := ui.NewServerModel(server)
	server.program = tea.NewProgram(model)
	uiWriter := ui.NewUIWriter(server.program)
	log.SetOutput(uiWriter)

	go server.Init()

	if _, err := server.program.Run(); err != nil {
		fmt.Printf("Erro na UI: %v", err)
		os.Exit(1)
	}
}

func (s *Server) Broadcast(text string) {
	s.broadcast <- *protocol.CreateMessage(protocol.MsgChat, []byte(text))
	log.Println("Broadcast enviado: " + text)
}

func (s *Server) SendTo(conn net.Conn, text string) {
	s.mu.Lock()
	client, ok := s.clients[conn]
	s.mu.Unlock()

	if ok {
		protocol.Send(client.writer, protocol.MsgChat, []byte(text))
		log.Printf("Mensagem enviada para %s: %s", conn.RemoteAddr().String(), text)
		go s.program.Send(ui.ChatMsg{
			Addr:    conn.RemoteAddr().String(),
			Content: text,
			IsSent:  true,
		})
	}
}

func (s *Server) Init() {
	listener, err := net.Listen("tcp", "localhost:3000")
	if err != nil {
		log.Printf("Erro ao iniciar servidor: %v", err)
		return
	}

	log.Println("Escutando na porta 3000")

	go s.broadcaster()

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("Erro no accept: %v", err)
			continue
		}

		go s.connectionHandler(conn)
	}
}

func (s *Server) connectionHandler(conn net.Conn) {
	addr := conn.RemoteAddr().String()
	writer := bufio.NewWriter(conn)

	var client Client
	client.writer = writer

	s.mu.Lock()
	s.clients[conn] = &client
	s.mu.Unlock()

	log.Println("Conexão aberta com: " + addr)
	s.program.Send(ui.ClientChangeMsg{Addr: addr, Joined: true, Conn: conn})

	for {
		msg, err := protocol.Read(conn)
		if err != nil {
			if errors.Is(err, net.ErrClosed) {
				log.Println("Conexão fechada: " + addr)
				break
			}

			log.Printf("Erro ao ler de %s: %v", addr, err)
			break
		}

		log.Printf("[%s] Mensagem recebida: %s", addr, protocol.Stringify(msg))

		switch msg.Type {
		case protocol.MsgChat:
			s.handleChat(conn, msg)
		case protocol.MsgFile:
			s.handleFile(conn, msg)
		case protocol.MsgEndConnection:
			s.handleEndConnection(conn, msg)
			return
		}
	}

	conn.Close()
	s.mu.Lock()
	delete(s.clients, conn)
	s.mu.Unlock()
	s.program.Send(ui.ClientChangeMsg{Addr: addr, Joined: false})
}

func (s *Server) handleChat(conn net.Conn, msg *protocol.Message) {
	addr := conn.RemoteAddr().String()

	s.program.Send(ui.ChatMsg{
		Addr:    addr,
		Content: string(msg.Content),
		IsSent:  false,
	})
}

func (s *Server) handleFile(conn net.Conn, msg *protocol.Message) {
	writer := s.clients[conn].writer
	addr := conn.RemoteAddr().String()

	path := string(msg.Content)
	fileInfo, err := os.Stat(path)

	if err != nil || fileInfo.IsDir() {
		protocol.Send(writer, protocol.MsgError, []byte("not found"))
		log.Printf("[%s] Arquivo não encontrado: %s", addr, path)
		return
	}

	contents, err := os.ReadFile(path)
	if err != nil {
		log.Printf("Erro ao ler arquivo: %v", err)
		protocol.Send(writer, protocol.MsgError, []byte("internal server error"))
		return
	}

	fileMessage := protocol.CreateFileMessage(fileInfo.Name(), contents)
	if err := protocol.SendMessage(writer, fileMessage); err != nil {
		log.Println("Erro ao enviar arquivo")
		return
	}

	log.Printf("[%s] Arquivo enviado: %s", addr, path)
}

func (s *Server) handleEndConnection(conn net.Conn, _ *protocol.Message) {
	addr := conn.RemoteAddr().String()

	log.Println("Cliente solicitou desconexão: " + addr)
	conn.Close()

	s.mu.Lock()
	delete(s.clients, conn)
	s.mu.Unlock()

	s.program.Send(ui.ClientChangeMsg{Addr: addr, Joined: false})
}

func (s *Server) broadcaster() {
	for msg := range s.broadcast {
		s.mu.Lock()
		for _, client := range s.clients {
			protocol.SendMessage(client.writer, &msg)
		}
		s.mu.Unlock()
	}
}
