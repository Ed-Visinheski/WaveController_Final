import cv2
import mediapipe as mp
import socket
import urllib.request
import os

# Initialize MediaPipe Hand Landmarker
BaseOptions = mp.tasks.BaseOptions
HandLandmarker = mp.tasks.vision.HandLandmarker
HandLandmarkerOptions = mp.tasks.vision.HandLandmarkerOptions
VisionRunningMode = mp.tasks.vision.RunningMode

# Download hand landmarker model if not present
model_path = 'hand_landmarker.task'
if not os.path.exists(model_path):
    print("Downloading hand landmarker model...")
    url = "https://storage.googleapis.com/mediapipe-models/hand_landmarker/hand_landmarker/float16/1/hand_landmarker.task"
    try:
        urllib.request.urlretrieve(url, model_path)
        print("Model downloaded successfully!")
    except Exception as e:
        print(f"Error downloading model: {e}")
        exit(1)

# Verify model file is valid
if not os.path.exists(model_path) or os.path.getsize(model_path) < 1000:
    print("Model file is invalid or corrupted. Deleting and re-running may help.")
    if os.path.exists(model_path):
        os.remove(model_path)
    exit(1)

options = HandLandmarkerOptions(
    base_options=BaseOptions(model_asset_path=model_path),
    running_mode=VisionRunningMode.IMAGE,
    num_hands=2,
    min_hand_detection_confidence=0.7,
    min_hand_presence_confidence=0.7,
    min_tracking_confidence=0.7
)

cap = cv2.VideoCapture(0)

UDP_IP = "127.0.0.1"
UDP_PORT = 5005
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

with HandLandmarker.create_from_options(options) as landmarker:
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        # Flip the frame for a mirror effect
        frame = cv2.flip(frame, 1)
        rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        
        # Convert to MediaPipe Image
        mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb_frame)
        
        # Process the frame (IMAGE mode doesn't need timestamp)
        results = landmarker.detect(mp_image)

        if results.hand_landmarks:
            for hand_landmarks in results.hand_landmarks:
                # Index tip (8), Thumb tip (4)
                x = hand_landmarks[8].x
                y = hand_landmarks[8].y
                thumb_x = hand_landmarks[4].x
                thumb_y = hand_landmarks[4].y

                # Calculate Euclidean distance (normalized)
                pinch_dist = ((x - thumb_x) ** 2 + (y - thumb_y) ** 2) ** 0.5
                is_pinch = 1 if pinch_dist < 0.07 else 0  # Adjust threshold as needed

                win_x = int(x * 700)
                win_y = int(y * 400)
                msg = f"{win_x},{win_y},{is_pinch}"
                sock.sendto(msg.encode(), (UDP_IP, UDP_PORT))

                # Draw landmarks manually
                h, w, _ = frame.shape
                for landmark in hand_landmarks:
                    cx, cy = int(landmark.x * w), int(landmark.y * h)
                    cv2.circle(frame, (cx, cy), 5, (0, 255, 0), -1)

        # Resize window to 700x400
        display_frame = cv2.resize(frame, (700, 400))
        cv2.imshow('Hand Tracking', display_frame)
        if cv2.waitKey(1) & 0xFF == 27:  # ESC to quit
            break

cap.release()
cv2.destroyAllWindows()