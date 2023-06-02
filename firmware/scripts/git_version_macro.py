import subprocess
import json

revision = (
    subprocess.check_output(["git", "rev-parse", "--short", "HEAD"])
    .strip()
    .decode("utf-8")
)

version = open(".version", "r").read().strip()

print("'-DGIT_COMMIT=\"%s\"' '-DVERSION=\"%s\"'" % (revision, version))
