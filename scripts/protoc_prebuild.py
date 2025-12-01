import os
import ssl
import urllib.request

# Raw URL of the rr_serial.proto file in the GitHub repository
PROTO_URL = "https://raw.githubusercontent.com/Ryder-Robots/rr_proto/0.0.4-DEVEL/proto/rr_serial.proto"
PROTO_DIR = "proto"
PROTO_FILE = "rr_serial.proto"

def download_proto_file():
    os.makedirs(PROTO_DIR, exist_ok=True)
    proto_path = os.path.join(PROTO_DIR, PROTO_FILE)
    if os.path.exists(proto_path):
        os.remove(proto_path)

    if not os.path.exists(proto_path):
        print(f"Downloading {PROTO_FILE}...")
        context = ssl._create_unverified_context()
        with urllib.request.urlopen(PROTO_URL, context=context) as response, open(proto_path, 'wb') as out_file:
            out_file.write(response.read())
        print(f"Downloaded {PROTO_FILE} to {proto_path}")
    else:
        print(f"{PROTO_FILE} already exists at {proto_path}")

if __name__ == "__main__":
    download_proto_file()
