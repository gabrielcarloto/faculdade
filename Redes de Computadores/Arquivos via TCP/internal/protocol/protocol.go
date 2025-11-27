package protocol

import (
	"bufio"
	"crypto/sha256"
	"encoding/binary"
	"encoding/json"
	"errors"
	"io"
	"log"
	"net"
	"slices"
)

type Message struct {
	Type     uint   `json:"type"`
	Checksum []byte `json:"checksum"`
	Content  []byte `json:"content"`
	Filename string `json:"filename"`
	Size     int    `json:"size"`
}

const (
	MsgChat = iota
	MsgFile
	MsgEndConnection
	MsgError
	MsgOk
)

func Read(connection net.Conn) (*Message, error) {
	var msg Message

	header := make([]byte, 4)
	_, err := io.ReadFull(connection, header)
	if err != nil {
		return nil, err
	}

	messageSize := binary.BigEndian.Uint32(header)
	messageBytes := make([]byte, messageSize)

	_, err = io.ReadFull(connection, messageBytes)
	if err != nil {
		return nil, err
	}

	err = json.Unmarshal(messageBytes, &msg)
	if err != nil {
		log.Println("Erro no Unmarshal")
		return nil, err
	}

	if !checkIntegrity(&msg) {
		log.Println("Descartando mensagem inválida")
		return nil, errors.New("invalid message")
	}

	log.Println("Mensagem válida")

	return &msg, nil
}

func CreateMessage(msgType uint, content []byte) *Message {
	return &Message{
		Type:     msgType,
		Checksum: createHash(content),
		Content:  content,
		Size:     len(content),
	}
}

func CreateFileMessage(filename string, content []byte) *Message {
	return &Message{
		Type:     MsgOk,
		Checksum: createHash(content),
		Content:  content,
		Filename: filename,
		Size:     len(content),
	}
}

func Serialize(msg *Message) ([]byte, error) {
	return json.Marshal(msg)
}

func Stringify(msg *Message) string {
	base := "[" + StrinfigyType(msg.Type) + "] "

	switch msg.Type {
	case MsgChat, MsgFile, MsgError:
		base += string(msg.Content)
	}

	return base
}

func StrinfigyType(msgType uint) string {
	switch msgType {
	case MsgChat:
		return "CHAT"
	case MsgFile:
		return "FILE"
	case MsgEndConnection:
		return "END"
	case MsgError:
		return "ERROR"
	case MsgOk:
		return "OK"
	}

	return "UNKNOWN"
}

func SendMessage(writer *bufio.Writer, message *Message) error {
	data, err := Serialize(message)
	if err != nil {
		return err
	}

	msgLen := len(data)
	var header []byte
	header = binary.BigEndian.AppendUint32(header, uint32(msgLen))

	if _, err := writer.Write(header); err != nil {
		return err
	}

	if _, err := writer.Write(data); err != nil {
		return err
	}

	log.Println("Enviando mensagem...")
	writer.Flush()
	log.Println("Mensagem enviada")

	return nil
}

func Send(writer *bufio.Writer, msgType uint, content []byte) error {
	message := CreateMessage(msgType, content)
	return SendMessage(writer, message)
}

func checkIntegrity(msg *Message) bool {
	return slices.Equal(msg.Checksum, createHash(msg.Content))
}

func createHash(content []byte) []byte {
	hash := sha256.Sum256(content)
	return hash[:]
}
