from subprocess import run
from time import sleep
import os

# Path and name to the script you are trying to start
file_path = os.path.dirname(__file__)+"/script_sqlite_revamp.py" 

restart_timer = 2
def start_script():
    try:
        # Make sure 'python' command is available
        run("python "+file_path, check=True) 
    except:
        # Script crashed, lets restart it!
        handle_crash()


def handle_crash():
    sleep(restart_timer)  # Restarts the script after 2 seconds
    start_script()

start_script()