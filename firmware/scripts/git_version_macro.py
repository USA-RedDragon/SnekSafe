import subprocess
import sys

revision = (
    subprocess.check_output(["git", "rev-parse", "--short", "HEAD"])
    .strip()
    .decode("utf-8")
)

version = open(".version", "r").read().strip()

# If the first argument to this script is set and it's 'dev' add -DCORE_DEBUG_LEVEL=5 -DDEV=true to the build flags
if len(sys.argv) > 1 and sys.argv[1] == "dev":
    print("'-DGIT_COMMIT=\"%s\"' '-DVERSION=\"%s\"' '-DCORE_DEBUG_LEVEL=5' '-DDEV=true'" % (revision, version))
else:
    print("'-DGIT_COMMIT=\"%s\"' '-DVERSION=\"%s\"'" % (revision, version))
