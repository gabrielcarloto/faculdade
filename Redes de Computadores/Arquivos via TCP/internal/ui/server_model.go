package ui

import (
	"fmt"
	"net"
	"slices"
	"strings"

	"github.com/charmbracelet/bubbles/textinput"
	"github.com/charmbracelet/bubbles/viewport"
	tea "github.com/charmbracelet/bubbletea"
)

type serverSessionState int

const (
	ServerStateMenu serverSessionState = iota
	ServerStateBroadcastInput
	ServerStateSelectClient
	ServerStateClientInput
)

type ServerModel struct {
	actions        ServerActions
	state          serverSessionState
	logs           []string
	clients        []string
	clientConns    map[string]net.Conn
	chatHistories  map[string][]string
	viewport       viewport.Model
	logViewport    viewport.Model
	textInput      textinput.Model
	selectedClient string
}

func NewServerModel(actions ServerActions) ServerModel {
	return ServerModel{
		actions:       actions,
		state:         ServerStateMenu,
		logs:          []string{},
		clients:       []string{},
		clientConns:   make(map[string]net.Conn),
		chatHistories: make(map[string][]string),
		viewport:      NewDefaultViewport(""),
		logViewport:   NewDefaultViewport(""),
		textInput:     NewDefaultTextInput("Digite a mensagem..."),
	}
}

func (m ServerModel) Init() tea.Cmd {
	return textinput.Blink
}

func (m ServerModel) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	var cmd tea.Cmd

	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch m.state {
		case ServerStateMenu:
			switch msg.String() {
			case "1":
				m.state = ServerStateBroadcastInput
				m.textInput.Placeholder = "Mensagem para todos..."
				m.textInput.Focus()
				m.textInput.SetValue("")
				return m, nil
			case "2":
				if len(m.clients) > 0 {
					m.state = ServerStateSelectClient
				}
			case "q", "ctrl+c":
				return m, tea.Quit
			}

		case ServerStateBroadcastInput:
			switch msg.String() {
			case "esc":
				m.state = ServerStateMenu
				m.textInput.Blur()
			case "enter":
				value := m.textInput.Value()
				m.actions.Broadcast(value)
				m.textInput.SetValue("")
				m.state = ServerStateMenu
				m.textInput.Blur()
			}

		case ServerStateSelectClient:
			switch msg.String() {
			case "esc":
				m.state = ServerStateMenu
			default:
				for i := 0; i < len(m.clients) && i < 9; i++ {
					if msg.String() == fmt.Sprintf("%d", i+1) {
						m.selectedClient = m.clients[i]
						m.state = ServerStateClientInput
						m.textInput.Placeholder = fmt.Sprintf("Mensagem para %s...", m.selectedClient)
						m.textInput.Focus()
						m.textInput.SetValue("")

						history := m.chatHistories[m.selectedClient]
						m.viewport.SetContent(strings.Join(history, "\n"))
						m.viewport.GotoBottom()
						return m, nil
					}
				}
			}

		case ServerStateClientInput:
			switch msg.String() {
			case "esc":
				m.state = ServerStateMenu
				m.textInput.Blur()
			case "enter":
				value := m.textInput.Value()
				if conn, ok := m.clientConns[m.selectedClient]; ok {
					m.actions.SendTo(conn, value)
				}
				m.textInput.SetValue("")
			}
		}

	case LogMsg:
		AppendAndScroll(&m.logViewport, &m.logs, string(msg))

	case ChatMsg:
		prefix := "Cliente: "
		if msg.IsSent {
			prefix = "Você: "
		}
		line := prefix + msg.Content
		if m.chatHistories[msg.Addr] == nil {
			m.chatHistories[msg.Addr] = []string{}
		}
		m.chatHistories[msg.Addr] = append(m.chatHistories[msg.Addr], line)

		if m.state == ServerStateClientInput && m.selectedClient == msg.Addr {
			history := m.chatHistories[msg.Addr]
			m.viewport.SetContent(strings.Join(history, "\n"))
			m.viewport.GotoBottom()
		}

	case ClientChangeMsg:
		if msg.Joined {
			m.clients = append(m.clients, msg.Addr)
			m.clientConns[msg.Addr] = msg.Conn
		} else {
			m.clients = slices.DeleteFunc(m.clients, func(addr string) bool {
				return addr == msg.Addr
			})

			delete(m.clientConns, msg.Addr)
		}
	}

	if m.state == ServerStateBroadcastInput || m.state == ServerStateClientInput {
		m.textInput, cmd = m.textInput.Update(msg)
		return m, cmd
	}

	return m, nil
}

func (m ServerModel) View() string {
	s := ""

	switch m.state {
	case ServerStateMenu:
		s += "=== SERVIDOR TCP ===\n\n"
		s += "1. Broadcast (Enviar para todos)\n"
		s += "2. Enviar mensagem privada\n"
		s += "q. Sair\n"
	case ServerStateBroadcastInput:
		s += "=== SERVIDOR TCP ===\n\n"
		s += "Digite a mensagem de Broadcast:\n"
		s += m.textInput.View() + "\n"
		s += "(Enter para enviar, Esc para cancelar)\n"
	case ServerStateSelectClient:
		s += "=== SERVIDOR TCP ===\n\n"
		s += "Selecione um cliente:\n"
		for i, c := range m.clients {
			s += fmt.Sprintf("%d. %s\n", i+1, c)
		}
		s += "(Esc para cancelar)\n"
	case ServerStateClientInput:
		s += RenderChat("CHAT - "+m.selectedClient, m.viewport, m.textInput)
	}

	if m.state != ServerStateClientInput {
		s += "\n--- Clientes Conectados ---\n"
		if len(m.clients) == 0 {
			s += "(Nenhum)\n"
		} else {
			for _, c := range m.clients {
				s += "- " + c + "\n"
			}
		}
	}

	s += RenderLogs(m.logViewport)

	return s
}
