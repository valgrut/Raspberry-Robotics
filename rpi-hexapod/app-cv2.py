# import required modules
from flask import Flask, render_template, Response 
import cv2
import socket 
import io 

##### Gives:
# [ WARN:0@0.248] global ./modules/videoio/src/cap_gstreamer.cpp (2401) handleMessage OpenCV | GStreamer warning: Embedded video playback halted; module v4l2src0 reported: Device '/dev/video0' is busy
# [ WARN:0@0.249] global ./modules/videoio/src/cap_gstreamer.cpp (1356) open OpenCV | GStreamer warning: unable to start pipeline
# [ WARN:0@0.249] global ./modules/videoio/src/cap_gstreamer.cpp (862) isPipelinePlaying OpenCV | GStreamer warning: GStreamer: pipeline have not been created
# [ WARN:0@0.249] global ./modules/videoio/src/cap_v4l.cpp (902) open VIDEOIO(V4L2:/dev/video0): can't open camera by index

app = Flask(__name__) 
vc = cv2.VideoCapture(0) 

@app.route('/') 
def index(): 
    """Video streaming .""" 
    return render_template('index.html') 

def gen(): 
    """Video streaming generator function.""" 
    while True: 
        rval, frame = vc.read() 
        cv2.imwrite(r'/home/hexapod/Code/pic.jpeg', frame) 
        yield (b'--frame\r\n' 
            b'Content-Type: image/jpeg\r\n\r\n' + open('/home/hexapod/Code/pic.jpeg', 'rb').read() + b'\r\n') 

@app.route('/video_feed') 
def video_feed(): 
    """Video streaming route. Put this in the src attribute of an img tag.""" 
    return Response(gen(), mimetype='multipart/x-mixed-replace; boundary=frame') 

if __name__ == '__main__': 
    app.run(host='192.168.88.210', debug=True, threaded=True)
