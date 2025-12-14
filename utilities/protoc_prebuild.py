import os
import ssl
import urllib.request

# Raw URL of the rr_serial.proto file in the GitHub repository
PROTO_URL = "https://raw.githubusercontent.com/Ryder-Robots/rr_proto/0.0.5-DEVEL/proto/"
PROTO_DIR = "proto"
PROTO_FILES = ["rr_serial.proto", "rr_serial.options"]

def download_proto_file():
    os.makedirs(PROTO_DIR, exist_ok=True)

    for proto_file in PROTO_FILES:
        proto_url_ex = urllib.parse.urljoin(PROTO_URL, proto_file)
        proto_path = os.path.join(PROTO_DIR, proto_file)
        if os.path.exists(proto_path):
            os.remove(proto_path)

        print(f"Downloading {proto_file}...")
        context = ssl._create_unverified_context()
        with urllib.request.urlopen(proto_url_ex, context=context) as response, open(proto_path, 'wb') as out_file:
            out_file.write(response.read())
        print(f"Downloaded {proto_file} to {proto_path}")

if __name__ == "__main__":
    download_proto_file()
