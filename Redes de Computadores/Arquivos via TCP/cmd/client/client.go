package main

import (
	"bufio"
	"errors"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"path/filepath"

	"arquivos-tcp/internal/protocol"
	"arquivos-tcp/internal/ui"

	tea "github.com/charmbracelet/bubbletea"
)

type Client struct {
	conn    net.Conn
	writer  *bufio.Writer
	program *tea.Program
}

func main() {
	addr := flag.String("addr", "localhost:3000", "Endereço do servidor (host:porta)")
	flag.Parse()

	conn, err := net.Dial("tcp", *addr)
	if err != nil {
		log.Fatalln("Erro ao conectar ao servidor: ", err)
	}
	defer conn.Close()

	client := &Client{
		conn:   conn,
		writer: bufio.NewWriter(conn),
	}

	model := ui.NewClientModel(client)
	client.program = tea.NewProgram(model)
	uiWriter := ui.NewUIWriter(client.program)
	log.SetOutput(uiWriter)

	go client.listenMessages()

	if _, err := client.program.Run(); err != nil {
		log.Fatal(err)
	}
}

func (c *Client) RequestFile(path string) error {
	return protocol.Send(c.writer, protocol.MsgFile, []byte(path))
}

func (c *Client) SendChat(message string) error {
	return protocol.Send(c.writer, protocol.MsgChat, []byte(message))
}

func (c *Client) CloseConnection() {
	protocol.Send(c.writer, protocol.MsgEndConnection, []byte{})
	c.conn.Close()
}

func (c *Client) listenMessages() {
	log.Println("Conectado ao servidor: " + c.conn.RemoteAddr().String())

	for {
		msg, err := protocol.Read(c.conn)
		if err != nil {
			if !errors.Is(err, net.ErrClosed) {
				c.program.Send(ui.ErrorMsg(err))
			}
			return
		}

		switch msg.Type {
		case protocol.MsgChat:
			log.Println("Mensagem recebida do servidor")
			c.program.Send(ui.ChatMsg{
				Content: string(msg.Content),
				IsSent:  false,
			})
		case protocol.MsgOk:
			log.Println("Arquivo recebido do servidor")
			c.handleReceivedFile(msg)
		case protocol.MsgError:
			log.Println("Erro recebido: " + string(msg.Content))
			c.program.Send(ui.ErrorMsg(fmt.Errorf("erro do servidor: %s", string(msg.Content))))
		case protocol.MsgEndConnection:
			log.Println("Servidor encerrou a conexão")
			c.program.Send(ui.ErrorMsg(fmt.Errorf("servidor encerrou a conexão")))
			c.program.Quit()
			return
		}
	}
}

func (c *Client) handleReceivedFile(msg *protocol.Message) {
	if err := os.MkdirAll("./out/", 0o755); err != nil {
		c.program.Send(ui.ErrorMsg(fmt.Errorf("erro ao criar pasta out: %w", err)))
	} else {
		savePath := filepath.Join("./out", msg.Filename)
		err := os.WriteFile(savePath, msg.Content, 0o644)

		if err != nil {
			c.program.Send(ui.ErrorMsg(fmt.Errorf("erro ao salvar arquivo: %w", err)))
		} else {
			log.Printf("Arquivo \"%s\", com tamanho %d bytes, salvo em: %s\n", msg.Filename, msg.Size, savePath)
			c.program.Send(ui.FileSavedMsg{Path: savePath})
		}
	}
}
