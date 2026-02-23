import cv2
import numpy as np
import time

# Paths
cfg_path = "/home/airlab/yolo/yolov4-tiny/yolov4-tiny.cfg"
weights_path = "/home/airlab/yolo/yolov4-tiny/yolov4-tiny.weights"
names_path = "/home/airlab/yolo/coco.names"

# Load YOLO
net = cv2.dnn.readNet(weights_path, cfg_path)
net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA_FP16)

with open(names_path, "r") as f:
    classes = [line.strip() for line in f.readlines()]

# ---- CAMERA VIA GSTREAMER APPSINK (FUNZIONA) ----
gst_pipeline = (
    "v4l2src device=/dev/video0 ! "
    "video/x-raw, width=640, height=480, framerate=30/1 ! "
    "videoconvert ! appsink"
)

cap = cv2.VideoCapture(gst_pipeline, cv2.CAP_GSTREAMER)

if not cap.isOpened():
    print("❌ Errore: impossibile aprire la webcam con GStreamer")
    exit()

# Video writer
fourcc = cv2.VideoWriter_fourcc(*"MJPG")
out = cv2.VideoWriter("/home/airlab/yolo/yolo_video.avi", fourcc, 10, (640, 480))

print("Recording... press CTRL+C to stop")

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Failed to grab frame")
            break

        # Rotate 180°
        frame = cv2.rotate(frame, cv2.ROTATE_180)

        # YOLO input
        blob = cv2.dnn.blobFromImage(frame, 1/255.0, (416,416), swapRB=True, crop=False)
        net.setInput(blob)

        start = time.time()
        outs = net.forward(net.getUnconnectedOutLayersNames())
        print("Inference:", time.time() - start)

        # Draw detections
        h, w = frame.shape[:2]
        for out_det in outs:
            for det in out_det:
                scores = det[5:]
                class_id = np.argmax(scores)
                conf = scores[class_id]

                if conf > 0.5:
                    cx, cy, bw, bh = (det[:4] * np.array([w, h, w, h])).astype(int)
                    x = int(cx - bw/2)
                    y = int(cy - bh/2)
                    cv2.rectangle(frame, (x,y), (x+bw, y+bh), (0,255,0), 2)
                    cv2.putText(frame, f"{classes[class_id]} {conf:.2f}",
                                (x,y-5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,255,0), 2)

        out.write(frame)

except KeyboardInterrupt:
    print("Stopped recording.")

finally:
    cap.release()
    out.release()
    print("Video saved to /home/airlab/yolo/yolo_video.avi")
