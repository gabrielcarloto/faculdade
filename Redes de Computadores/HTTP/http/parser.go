package http

import (
	"bytes"
	"strconv"
	"strings"
)

type Request struct {
	Method  string
	Path    string
	Version string
	Headers map[string]string
	Body    []byte
	Size    int
}

func ReadRequest(data []byte) (Request, bool) {
	delimiterIndex := bytes.Index(data, []byte(BodyDelimiter))

	if delimiterIndex == -1 {
		return Request{}, false
	}

	requestEnd := delimiterIndex + len(BodyDelimiter)

	headers := data[:delimiterIndex]
	lines := strings.Split(string(headers), "\n")
	meta := strings.Split(lines[0], " ")

	request := Request{
		Method:  meta[0],
		Path:    meta[1],
		Version: meta[2],
		Headers: map[string]string{},
	}

	for _, header := range lines[1:] {
		if header == "" {
			continue
		}

		parts := strings.SplitN(header, ":", 2)

		if len(parts) == 2 {
			request.Headers[parts[0]] = parts[1]
		}
	}

	contentLength, hasBody := request.Headers["Content-Length"]

	if !hasBody {
		request.Size = requestEnd
		return request, true
	}

	bodyLength, _ := strconv.Atoi(contentLength)
	bodyStart := delimiterIndex + len(BodyDelimiter)

	if len(data) < bodyStart+bodyLength {
		return Request{}, false
	}

	requestEnd += bodyLength

	request.Body = data[bodyStart:bodyLength]
	request.Size = requestEnd

	return request, true
}
