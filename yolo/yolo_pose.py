import cv2
import numpy as np
import time

# -------------------------------
# PATHS
# -------------------------------
yolo_cfg = "/home/airlab/yolo/yolov4-tiny/yolov4-tiny.cfg"
yolo_weights = "/home/airlab/yolo/yolov4-tiny/yolov4-tiny.weights"
coco_names = "/home/airlab/yolo/coco.names"
pose_onnx = "/home/airlab/yolo/pose_models_onnx/pose_estimation_mediapipe.onnx"
output_video = "/home/airlab/yolo/yolo_pose_video.avi"

# -------------------------------
# LOAD YOLOv4-tiny
# -------------------------------
net = cv2.dnn.readNet(yolo_weights, yolo_cfg)
net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA_FP16)

with open(coco_names, "r") as f:
    classes = [line.strip() for line in f.readlines()]

# -------------------------------
# LOAD ONNX POSE NET
# -------------------------------
pose_net = cv2.dnn.readNetFromONNX(pose_onnx)
pose_net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
pose_net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA_FP16)

# -------------------------------
# OPEN CAMERA
# -------------------------------
cap = cv2.VideoCapture(0)
time.sleep(2)  # warm-up

# -------------------------------
# VIDEO WRITER
# -------------------------------
fourcc = cv2.VideoWriter_fourcc(*"MJPG")
fps = 10  # ridotto per Jetson
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
out = cv2.VideoWriter(output_video, fourcc, fps, (frame_width, frame_height))

print("Recording... press Ctrl+C to stop")

# -------------------------------
# FUNCTIONS
# -------------------------------
def draw_pose(frame, keypoints):
    """
    Disegna i keypoints e linee scheletro sul frame
    """
    # lista di connessioni simile a BlazePose
    connections = [
        (0,1),(1,2),(2,3),(3,7),      # testa-spalle
        (0,4),(4,5),(5,6),(6,8),      # testa-spalle lato opposto
        (9,10),(11,12),(12,14),(14,16),(11,13),(13,15),(12,24),(11,23),(24,26),(23,25),(24,28),(23,27)  # corpo e gambe semplificate
    ]

    # disegna punti
    for x, y in keypoints:
        cv2.circle(frame, (int(x), int(y)), 3, (0,0,255), -1)

    # disegna linee scheletro
    for i,j in connections:
        if i < len(keypoints) and j < len(keypoints):
            cv2.line(frame, (int(keypoints[i][0]), int(keypoints[i][1])),
                     (int(keypoints[j][0]), int(keypoints[j][1])), (0,255,0), 2)

# -------------------------------
# MAIN LOOP
# -------------------------------
try:
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Failed to grab frame")
            break

        # Rotate 180°
        frame = cv2.rotate(frame, cv2.ROTATE_180)

        # -------------------------------
        # YOLO PERSON DETECTION
        # -------------------------------
        blob = cv2.dnn.blobFromImage(frame, 1/255.0, (416,416), swapRB=True, crop=False)
        net.setInput(blob)
        outs = net.forward(net.getUnconnectedOutLayersNames())

        h, w = frame.shape[:2]
        person_boxes = []

        for out_det in outs:
            for det in out_det:
                scores = det[5:]
                class_id = np.argmax(scores)
                conf = scores[class_id]
                if conf > 0.5 and classes[class_id] == "person":
                    cx, cy, bw, bh = (det[0:4] * np.array([w,h,w,h])).astype(int)
                    x = int(cx - bw/2)
                    y = int(cy - bh/2)
                    person_boxes.append((x,y,bw,bh))
                    cv2.rectangle(frame, (x,y), (x+bw, y+bh), (255,0,0), 2)
                    cv2.putText(frame, f"person {conf:.2f}", (x,y-5),
                                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,0,0), 2)

        # -------------------------------
        # POSE ESTIMATION
        # -------------------------------
        for box in person_boxes:
            x,y,bw,bh = box
            x1 = max(0, x)
            y1 = max(0, y)
            x2 = min(w, x+bw)
            y2 = min(h, y+bh)
            person_crop = frame[y1:y2, x1:x2]

            if person_crop.size == 0:
                continue

            # blob e forward ONNX
            pose_blob = cv2.dnn.blobFromImage(person_crop, 1.0/255, (256,256), (0,0,0), swapRB=True, crop=False)
            pose_net.setInput(pose_blob)
            output = pose_net.forward()  # shape: (1, 33*2)

            # estrai keypoints
            keypoints = output.reshape(-1,2)
            keypoints[:,0] = keypoints[:,0] * bw + x1
            keypoints[:,1] = keypoints[:,1] * bh + y1

            # disegna scheletro
            draw_pose(frame, keypoints)

        # -------------------------------
        # WRITE FRAME
        # -------------------------------
        out.write(frame)

except KeyboardInterrupt:
    print("Stopped recording.")

finally:
    cap.release()
    out.release()
    print(f"Video saved to {output_video}")
