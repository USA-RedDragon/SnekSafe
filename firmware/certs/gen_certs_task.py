import os

Import("env")
env.Execute("$PYTHONEXE -m pip install cryptography")

print("Running CA hook")
try:
    os.remove("certs/x509_crt_bundle.bin")
except FileNotFoundError:
    pass

from gen_crt_bundle import CertificateBundle
certs = CertificateBundle()
certs.add_from_path("certs")
crt_bundle = certs.create_bundle()

with open("certs/x509_crt_bundle.bin", 'wb') as f:
    f.write(crt_bundle)
