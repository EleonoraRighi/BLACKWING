import os
os.environ.setdefault("OPENCV_LOG_LEVEL", "ERROR")

import cv2
import numpy as np

# ---------------- YOLO CONFIG ----------------
cfg_path = "/home/airlab/yolo/yolov4-tiny/yolov4-tiny.cfg"
weights_path = "/home/airlab/yolo/yolov4-tiny/yolov4-tiny.weights"
names_path = "/home/airlab/yolo/coco.names"

net = cv2.dnn.readNet(weights_path, cfg_path)
net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA_FP16)

with open(names_path, "r") as f:
    classes = [line.strip() for line in f.readlines()]

# ---------------- CAMERA ----------------
# Modifica qui se il tuo device non è /dev/video0
video_device = 0  # prova con 1 o 2 se non funziona
gst_pipeline = (
    f"v4l2src device=/dev/video{video_device} ! "
    "image/jpeg, width=640, height=480, framerate=30/1 ! "
    "jpegdec ! videoconvert ! appsink"
)

cap = cv2.VideoCapture(gst_pipeline, cv2.CAP_GSTREAMER)
if not cap.isOpened():
    print("⚠️ GStreamer not working, trying with VideoCapture directly")
    cap = cv2.VideoCapture(video_device)

if not cap.isOpened():
    print("❌ Error opening webcam")
    exit()

# ---------------- VIDEO WRITER ----------------
fourcc = cv2.VideoWriter_fourcc(*"MJPG")
video_out = cv2.VideoWriter(
    "/home/airlab/yolo/yolo_video.avi",
    fourcc, 10, (640, 480)
)

#print("Recording... CTRL+C to exit")

# ---------------- YOLO PARAMETERS ----------------
min_conf_yolo = 0.4
min_area_yolo = 8000
nms_threshold = 0.3
redetect_interval = 1
frame_count = 0

# ---------------- FILE PATHS ----------------
# Scrive i file nella stessa cartella di questo script (vision/)
_here = os.path.dirname(os.path.abspath(__file__))
person_file = os.path.join(_here, "camera_detection.txt")
like_dislike_file = os.path.join(_here, "likedislike_detection.txt")

# Inizializza i file vuoti
open(person_file, "w").close()
open(like_dislike_file, "w").close()

# ---------------- COLOR DETECTION ----------------
def detect_color(box_img, threshold=0.05):
    """
    Rileva il colore predominante (arancione o blu) in una ROI.
    box_img: immagine BGR ritagliata dal box.
    threshold: percentuale minima di pixel di un colore per considerarlo predominante.
    """
    if box_img.size == 0:
        return "other"

    hsv = cv2.cvtColor(box_img, cv2.COLOR_BGR2HSV)

    # Range HSV per arancione
    lower_orange = np.array([5, 100, 100])
    upper_orange = np.array([20, 255, 255])

    # Range HSV per blu
    lower_blue = np.array([100, 150, 50])
    upper_blue = np.array([140, 255, 255])

    mask_orange = cv2.inRange(hsv, lower_orange, upper_orange)
    mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)

    orange_count = cv2.countNonZero(mask_orange)
    blue_count = cv2.countNonZero(mask_blue)
    total_pixels = box_img.shape[0] * box_img.shape[1]

    orange_ratio = orange_count / total_pixels
    blue_ratio = blue_count / total_pixels

    if orange_ratio > blue_ratio and orange_ratio > threshold:
        return "orange"
    elif blue_ratio > orange_ratio and blue_ratio > threshold:
        return "blue"
    else:
        return "other"
# =====================================================
try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        frame = cv2.rotate(frame, cv2.ROTATE_180)
        h, w = frame.shape[:2]

        detected_person = False
        detected_colors = []

        # ================= YOLO =================
        if frame_count % redetect_interval == 0:
            blob = cv2.dnn.blobFromImage(
                frame, 1 / 255.0, (416, 416),
                swapRB=True, crop=False
            )
            net.setInput(blob)
            outs = net.forward(net.getUnconnectedOutLayersNames())

            boxes = []
            confidences = []
            class_ids = []

            for out_det in outs:
                for det in out_det:
                    scores = det[5:]
                    class_id = np.argmax(scores)
                    conf = scores[class_id]

                    if conf < min_conf_yolo:
                        continue

                    cx, cy, bw, bh = (det[:4] * np.array([w, h, w, h])).astype(int)
                    x = int(cx - bw / 2)
                    y = int(cy - bh / 2)
                    area = bw * bh

                    if area < min_area_yolo:
                        continue

                    boxes.append([x, y, bw, bh])
                    confidences.append(float(conf))
                    class_ids.append(class_id)

            indices = cv2.dnn.NMSBoxes(
                boxes, confidences,
                score_threshold=min_conf_yolo,
                nms_threshold=nms_threshold
            )

            for i in indices:
                i = i[0] if isinstance(i, (list, tuple, np.ndarray)) else i
                x, y, bw, bh = boxes[i]
                cls = classes[class_ids[i]]

                # Limita la ROI ai bordi del frame
                x1 = max(0, x)
                y1 = max(0, y)
                x2 = min(w, x + bw)
                y2 = min(h, y + bh)

                if x2 > x1 and y2 > y1:
                    roi = frame[y1:y2, x1:x2]
                    color = color = detect_color(roi, threshold=0.05)  # 5% dei pixel
                else:
                    color = "other"

                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                cv2.putText(frame, f"{cls} - {color}", (x1, y1 - 5),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

                # Aggiorna rilevazioni per i file
                if cls == "person":
                    detected_person = True
                if color == "orange":
                    detected_colors.append("like")
                elif color == "blue":
                    detected_colors.append("dislike")

                #print(f"Detected {cls} with predominant color: {color}")

        # ================= AGGIORNA FILE =================
        # camera_detection.txt
        if detected_person:
            with open(person_file, "w") as f:
                f.write("person\n")
        else:
            open(person_file, "w").close()

        # likedislike_detection.txt
        if detected_colors:
            with open(like_dislike_file, "w") as f:
                f.write("\n".join(detected_colors) + "\n")
        else:
            open(like_dislike_file, "w").close()

        frame_count += 1
        video_out.write(frame)

except KeyboardInterrupt:
    print("Stopped")

finally:
    cap.release()
    video_out.release()
    print("Video saved in /home/airlab/yolo/yolo_video.avi")
