FROM golang:1.25.1 AS builder

WORKDIR /app

COPY go.mod go.sum ./
RUN go mod download

COPY . .

RUN CGO_ENABLED=0 GOOS=linux go build -ldflags="-s -w" -o server .

FROM gcr.io/distroless/base-debian12

WORKDIR /app
COPY --from=builder /app/server .
COPY ./models ./models

EXPOSE 8000

ENTRYPOINT ["/app/server"]
