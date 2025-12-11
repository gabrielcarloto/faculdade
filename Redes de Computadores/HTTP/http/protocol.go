package http

import "fmt"

const BodyDelimiter = "\r\n\r\n"

type HTTPError struct {
	HTTPResponse

	Err error
}

func (e *HTTPError) Error() string {
	return fmt.Sprintf("[%d] %s. Cause: %v)", e.StatusCode, e.Message, e.Err)
}

func (e *HTTPError) Unwrap() error {
	return e.Err
}

const (
	StatusOK                  = 200
	StatusForbidden           = 403
	StatusNotFound            = 404
	StatusInternalServerError = 500
)
