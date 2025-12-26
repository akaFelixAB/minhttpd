# 🚀 minhttpd

A tiny, minimal TCP listener written in C that ***only needs the Linux Kernel***. It listens on port 8080 and echoes received bytes to stdout prefixed with "recieved: ".

### Quick facts:
- **Language:** C, Assembly.
- **Build:** `Makefile` produces `build/httpd` (release) or `build/httpd-debug` (debug).
- **License:** MIT — see `LICENCE`.

### Why this project? ✨
- Educational minimal HTTP/TCP server showing raw syscalls and a tiny runtime.

## Getting started 🛠️

### 1. Build (release):

```sh
make release
```

### 2. Run:

```sh
./build/httpd
```

### 3. Test (from another terminal):

```sh
curl http://127.0.0.1:8080/
```

## What to expect ⚠️
- The server accepts TCP connections on port `8080`. It reads bytes from the client and prints them to stdout prefixed with `received: `.
- This is not a full HTTP implementation — it's a minimal educational echo-server.

## Files of interest 📁
- `httpd.c` — main server loop, socket setup, read/echo logic.
- `start.S` — tiny `_start` and syscall wrappers.
- `Makefile` — build targets `release` and `debug`.

## Development notes 🧪
- TODO in source: implement `fork` for handling concurrent clients.
- The build uses `-nostdlib` and static flags; adjust `CFLAGS`/`LDFLAGS` in the `Makefile` for your environment if needed.

## Contributing
- Feel free to open issues or PRs to add features (proper HTTP parsing, concurrency, logging).

## License 📄
- MIT — see `LICENCE` for details.
