package ui

import (
	"strings"

	"github.com/charmbracelet/bubbles/textinput"
	"github.com/charmbracelet/bubbles/viewport"
	tea "github.com/charmbracelet/bubbletea"
)

type UIWriter struct {
	program *tea.Program
}

func NewUIWriter(program *tea.Program) *UIWriter {
	return &UIWriter{
		program: program,
	}
}

func (w *UIWriter) Write(p []byte) (n int, err error) {
	msg := strings.TrimSpace(string(p))
	if msg != "" && w.program != nil {
		go w.program.Send(LogMsg(msg))
	}

	return len(p), nil
}

func NewDefaultTextInput(placeholder string) textinput.Model {
	ti := textinput.New()
	ti.Placeholder = placeholder
	ti.CharLimit = 156
	ti.Width = 40
	return ti
}

func NewDefaultViewport(initialContent string) viewport.Model {
	vp := viewport.New(80, 10)
	vp.SetContent(initialContent)
	return vp
}

func AppendAndScroll(vp *viewport.Model, items *[]string, newItem string) {
	*items = append(*items, newItem)
	vp.SetContent(strings.Join(*items, "\n"))
	vp.GotoBottom()
}

func RenderLogs(vp viewport.Model) string {
	return "\n--- Logs ---\n" + vp.View()
}

func RenderChat(title string, vp viewport.Model, ti textinput.Model) string {
	s := "=== " + title + " ===\n"
	s += vp.View()
	s += "\n" + ti.View() + "\n"
	s += "\n(Enter para enviar, Esc para voltar)\n"
	return s
}
