run:
	python3 src/app.py

config:
	bash rpi-configuration.sh 32bit

ssh:
	sshpass -p hexapod ssh hexapod@192.168.88.210

sync:
	bash move-code-to-rpi.sh
