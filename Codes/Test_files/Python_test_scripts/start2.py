import subprocess
import os

subprocess.run("python3 "+os.path.dirname(__file__)+"/python_gui.py & python3 "+os.path.dirname(__file__)+"/mqtt_database_add_remove_change.py", shell=True)