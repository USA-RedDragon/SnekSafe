import subprocess
import json

revision = (
    subprocess.check_output(["git", "rev-parse", "--short", "HEAD"])
    .strip()
    .decode("utf-8")
)

version = open(".version", "r").read().strip()

frontend_version = json.load(open("../frontend/package.json", "r"))["version"]

print("'-DFRONTEND_VERSION=\"%s\"' '-DGIT_COMMIT=\"%s\"' '-DVERSION=\"%s\"'" % (frontend_version, revision, version))
