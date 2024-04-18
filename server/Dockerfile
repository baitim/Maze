FROM alpine
WORKDIR /app
COPY . .
RUN apk add build-base
RUN apk add libuv-dev
RUN make clean 2>/dev/null ; make
RUN chmod +x /app/server
EXPOSE 5002
EXPOSE 5002/udp
ENTRYPOINT ["/app/server"]
