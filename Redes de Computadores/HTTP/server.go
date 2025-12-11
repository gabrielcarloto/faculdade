package main

import (
	"bufio"
	"errors"
	"flag"
	"io/fs"
	"log"
	"net"
	"os"
	"path"
	"sync"

	"http-server/http"
)

type Server struct {
	clients map[net.Conn]*Client
	root    string
	mu      sync.Mutex
}

type Client struct {
	writer *bufio.Writer
}

func main() {
	root := flag.String("root", "../../Padrões Web/primeira página/", "Pasta raiz para servir arquivos")
	flag.Parse()

	server := &Server{
		clients: make(map[net.Conn]*Client),
		mu:      sync.Mutex{},
		root:    *root,
	}

	server.Init()
}

func (s *Server) Init() {
	listener, err := net.Listen("tcp", "localhost:3000")
	if err != nil {
		log.Printf("Erro ao iniciar servidor: %v", err)
		return
	}

	log.Println("Escutando na porta 3000")

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

	buffer := make([]byte, 4096)
	acc := make([]byte, 0)

	for {
		bytesRead, err := conn.Read(buffer)
		if err != nil {
			break
		}

		acc = append(acc, buffer[:bytesRead]...)

		for {
			request, complete := http.ReadRequest(acc)

			if !complete {
				break
			}

			acc = acc[request.Size:]

			res, shouldClose := s.handleRequest(request)
			conn.Write(http.SerializeResponse(res))

			if shouldClose {
				s.handleEndConnection(conn)
				return
			}
		}
	}

	s.handleEndConnection(conn)
}

func (s *Server) handleEndConnection(conn net.Conn) {
	addr := conn.RemoteAddr().String()

	log.Println("Encerrando conexão com " + addr)
	conn.Close()

	s.mu.Lock()
	delete(s.clients, conn)
	s.mu.Unlock()
}

func (s *Server) handleRequest(request http.Request) (http.HTTPResponse, bool) {
	if request.Method != "GET" {
		// erro
	}

	connection := request.Headers["Connection"]
	shouldClose := connection == "Close"

	log.Println("GET RECEBIDO")
	log.Println(request.Path)

	// for header, value := range request.Headers {
	// 	log.Println(header + ": " + value)
	// }

	log.Println("Pegando arquivo")
	content, err := s.getFile(request.Path)
	if err != nil {
		if httpError, ok := err.(*http.HTTPError); ok {
			log.Println("Retornando erro")
			return httpError.HTTPResponse, shouldClose
		}

		return http.HTTPResponse{
			Message:    "Internal Server Error",
			StatusCode: http.StatusInternalServerError,
		}, shouldClose
	}

	return http.HTTPResponse{
		StatusCode: http.StatusOK,
		Message:    "OK",
		Body:       content,
	}, shouldClose
}

func (s *Server) getFile(filepath string) ([]byte, error) {
	contents, err := os.ReadFile(path.Join(s.root, filepath))
	if err != nil {
		if errors.Is(err, fs.ErrNotExist) {
			return nil, &http.HTTPError{HTTPResponse: http.HTTPResponse{Message: "Not Found", StatusCode: http.StatusNotFound}}
		}

		if errors.Is(err, fs.ErrPermission) {
			return nil, &http.HTTPError{HTTPResponse: http.HTTPResponse{Message: "Permission denied", StatusCode: http.StatusForbidden}}
		}

		return nil, &http.HTTPError{HTTPResponse: http.HTTPResponse{Message: "Internal Server Error", StatusCode: http.StatusInternalServerError}, Err: err}
	}

	return contents, nil
}
