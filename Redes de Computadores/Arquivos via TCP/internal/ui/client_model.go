package ui

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/bubbles/textinput"
	"github.com/charmbracelet/bubbles/viewport"
	tea "github.com/charmbracelet/bubbletea"
)

type clientSessionState int

const (
	ClientStateMenu clientSessionState = iota
	ClientStateRequestFile
	ClientStateChat
)

type ClientModel struct {
	actions       ClientActions
	state         clientSessionState
	textInput     textinput.Model
	viewport      viewport.Model
	logViewport   viewport.Model
	chatHistory   []string
	logs          []string
	statusMessage string
	err           error
}

func NewClientModel(actions ClientActions) ClientModel {
	ti := NewDefaultTextInput("Digite aqui...")
	ti.Focus()

	return ClientModel{
		actions:     actions,
		state:       ClientStateMenu,
		textInput:   ti,
		viewport:    NewDefaultViewport("Chat"),
		logViewport: NewDefaultViewport(""),
		chatHistory: []string{},
		logs:        []string{"Cliente iniciado..."},
	}
}

func (m ClientModel) Init() tea.Cmd {
	return textinput.Blink
}

func (m ClientModel) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	var cmd tea.Cmd

	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch m.state {
		case ClientStateMenu:
			switch msg.String() {
			case "1":
				m.state = ClientStateRequestFile
				m.textInput.Placeholder = "Caminho do arquivo (ex: assets/img.png)"
				m.textInput.SetValue("")
				m.statusMessage = ""
				return m, nil
			case "2":
				m.state = ClientStateChat
				m.textInput.Placeholder = "Digite sua mensagem..."
				m.textInput.SetValue("")
				m.statusMessage = ""
				m.viewport.SetContent(strings.Join(m.chatHistory, "\n"))
				m.viewport.GotoBottom()
				return m, nil
			case "3", "q", "ctrl+c":
				m.actions.CloseConnection()
				return m, tea.Quit
			}

		case ClientStateRequestFile, ClientStateChat:
			switch msg.String() {
			case "esc":
				m.state = ClientStateMenu
				m.statusMessage = ""
				return m, nil
			case "enter":
				value := m.textInput.Value()
				if m.state == ClientStateRequestFile {
					err := m.actions.RequestFile(value)
					if err != nil {
						m.statusMessage = "Erro ao enviar requisição: " + err.Error()
					} else {
						m.statusMessage = "Requisição enviada: " + value
					}
					m.textInput.SetValue("")
				} else if m.state == ClientStateChat {
					err := m.actions.SendChat(value)
					if err != nil {
						m.statusMessage = "Erro ao enviar mensagem: " + err.Error()
					} else {
						AppendAndScroll(&m.viewport, &m.chatHistory, "Você: "+value)
					}
					m.textInput.SetValue("")
				}
			}
		}

	case ChatMsg:
		AppendAndScroll(&m.viewport, &m.chatHistory, "Server: "+msg.Content)

	case FileSavedMsg:
		m.statusMessage = "Arquivo salvo em " + msg.Path

	case ErrorMsg:
		m.statusMessage = "Erro: " + msg.Error()

	case LogMsg:
		AppendAndScroll(&m.logViewport, &m.logs, string(msg))
	}

	if m.state == ClientStateRequestFile || m.state == ClientStateChat {
		m.textInput, cmd = m.textInput.Update(msg)
		return m, cmd
	}

	return m, nil
}

func (m ClientModel) View() string {
	if m.err != nil {
		return fmt.Sprintf("Erro: %v\n", m.err)
	}

	s := "\n"

	switch m.state {
	case ClientStateMenu:
		s += "=== MENU CLIENTE ===\n"
		s += "1. Requisitar arquivo\n"
		s += "2. Chat\n"
		s += "3. Encerrar conexão\n"
		s += "\n(Pressione o número da opção)\n"

	case ClientStateRequestFile:
		s += "=== REQUISITAR ARQUIVO ===\n"
		s += "Digite o caminho do arquivo no servidor:\n"
		s += m.textInput.View() + "\n"
		s += "\n(Enter para enviar, Esc para voltar)\n"

	case ClientStateChat:
		s += RenderChat("CHAT", m.viewport, m.textInput)
	}

	if m.statusMessage != "" {
		s += "\nSTATUS: " + m.statusMessage + "\n"
	}

	s += RenderLogs(m.logViewport)

	return s
}
