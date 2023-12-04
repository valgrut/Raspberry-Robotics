from flask import Flask, render_template, Response
import cv2

app = Flask(__name__)
#app.config["CACHE_TYPE"] = "null"

@app.route('/')
def index():
    """Video streaming home page."""
    return render_template('index.html')

def gen_frames():
    """Video streaming generator function."""
    # camera = cv2.VideoCapture(cv2.CAP_V4L2)

    camera = cv2.VideoCapture()

    # vs = cv2.VideoCapture(f'v4l2src device=/dev/video0 io-mode=2 ! image/jpeg, width=(int)2592, height=(int)1944 !  nvjpegdec ! video/x-raw, format=I420 ! appsink', cv2.CAP_GSTREAMER)
    # vs.set(cv2.CAP_PROP_FPS, 15)
    # vs.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
    # vs.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)
    # time.sleep(2)
    # vs.set(cv2.CAP_PROP_EXPOSURE, -8.0)

    while True:
        success, frame = camera.read()
        if not success:
            print("Error during camera.read(), break.")
            break;
        success, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()
        yield (b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

    camera.release()
    cv2.destroyAllWindows()


@app.route('/video_feed')
def video_feed():
    """Video streaming route. Put this in the src attribute of an img tag."""
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')


if __name__ == '__main__':
    app.run(host='192.168.88.210', port=5000, debug=True, threaded=True)
