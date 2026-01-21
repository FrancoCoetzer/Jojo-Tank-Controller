import socket
import threading

server = input("Enter hostname or IP [127.0.0.1]: ").strip() or "127.0.0.1"
port_input = input("Enter port [5000]: ").strip()
port = int(port_input) if port_input else 5000

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    sock.connect((server, port))
except ConnectionRefusedError:
    print(f"Connection refused to {server}:{port}")
    exit(1)
except socket.gaierror:
    print(f"Invalid hostname: {server}")
    exit(1)

running = True

def receive():
    while running:
        try:
            data = sock.recv(4096)
            if data:
                print(data.decode(), end="", flush=True)
            else:
                print("\nServer closed connection.")
                break
        except:
            break

# Start receiver thread
thread = threading.Thread(target=receive, daemon=True)
thread.start()

print(f"Connected to {server}:{port} (type 'exit' to quit)\n")

while running:
    try:
        cmd = input()
        if cmd.lower() == "exit":
            break
        sock.send((cmd + "\n").encode())
    except:
        break

running = False
sock.close()
print("Disconnected.")