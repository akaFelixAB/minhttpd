# 🚀 minhttpd

Minimal HTTP 1.0 server written in C (with a tiny assembly `_start`), requiring only the Linux kernel. Demonstrates raw syscalls, socket programming, and simple HTTP file serving.

## ✨ Features
- 🪶 **Minimal dependencies:** No libc, only Linux syscalls (see `start.S`).
- 📄 **HTTP 1.0 file server:** Serves a static file to any client (e.g., browser, curl, ncat).
- 🍴 **Forks per connection:** Each client handled in a separate process (via `fork`).
- 🛠️ **Manual socket setup:** No helper libraries, all socket logic in C.

## 🏗️ Build

Release build:

```sh
make release
# Output: build/minhttpd
wc -c build/minhttpd # About 6.6 KB
```

Debug build:

```sh
make debug
# Output: build/minhttpd-debug
```

## ▶️ Usage

```sh
./build/minhttpd <file-to-serve> [port]
# Example:
./build/minhttpd test.html 8080
```

Default port is 8080 if not specified.

## 🧪 Test

Open another terminal and run:

```sh
curl http://127.0.0.1:8080/
```

Or open this URL in a web browser:

```
http://127.0.0.1:8080/
```

## ⚙️ How it works
- Listens on the given port (default 8080).
- Accepts TCP connections, forks a process for each client.
- Reads HTTP request until an empty line (\r\n\r\n or \n\n).
- Responds with HTTP/1.1 200 OK and the contents of the specified file.
- If file not found, responds with 404 Not Found.
- Closes the connection after serving the file.

## 📁 File structure
- `httpd.c` — Main server logic, socket setup, HTTP parsing, file serving.
- `start.S` — Custom entry point and syscall wrappers (no libc).
- `Makefile` — Build rules for debug/release.
- `test.html` — Example file to serve.
- `LICENCE` — MIT License.

## 📝 Notes
- Only supports serving a single file (no directory listing).
- Not a full HTTP implementation (no keep-alive, no headers except status).
- Designed for educational purposes and minimalism.
- Uses `-nostdlib`, `-ffreestanding`, and static linking.

## 📄 License

MIT — see `LICENCE` for details.
