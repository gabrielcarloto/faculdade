package http

import (
	"fmt"
	"strconv"
)

type HTTPResponse struct {
	StatusCode int
	Message    string
	Body       []byte
	Headers    map[string]string
}

func SerializeResponse(res HTTPResponse) []byte {
	text := fmt.Sprintf("HTTP/1.1 %d %s", res.StatusCode, res.Message)

	if res.Headers == nil {
		res.Headers = map[string]string{}
	}

	res.Headers["Content-Length"] = strconv.Itoa(len(res.Body))

	for header, value := range res.Headers {
		text = fmt.Sprintf("%s\n%s: %s", text, header, value)
	}

	text = fmt.Sprintf("%s%s%s", text, BodyDelimiter, string(res.Body))

	return []byte(text)
}
