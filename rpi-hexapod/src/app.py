from flask import Flask, render_template, Response
from picamera2 import Picamera2, Preview
import io
import time

app = Flask(__name__)

@app.route('/')
def index():
    """Video streaming home page."""
    return render_template('index.html')

def gen():
    """Video streaming generator function."""
    picam2 = Picamera2()
    camera_config = picam2.create_preview_configuration()
    picam2.configure(camera_config)
    picam2.start()

    while True:
        photo = "/home/hexapod/Code/frame.jpg"
        time.sleep(0.05)
        # picam2.capture_file(photo)
        data = io.BytesIO()
        picam2.capture_file(data, format="jpeg")
        
        yield (b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + data.getvalue() + b'\r\n')
        # yield (b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + open(photo, 'rb').read() + b'\r\n')

    picam2.close()

@app.route('/video_feed')
def video_feed():
    """Video streaming route. Put this in the src attribute of an img tag."""
    return Response(gen(),mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='192.168.88.210', port=5000, debug=True, threaded=True)
