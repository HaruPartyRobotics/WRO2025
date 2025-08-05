from ultralytics import YOLO
import cv2

# Load model (make sure 'yolov5s.pt' is compatible; you can also try 'yolov8s.pt')
model = YOLO('yolov5n.pt')

# Load and run detection on image
results = model("C:\Users\LENOVO\OneDrive\gg.png", save=True)

# Print results (detected classes and confidence)
results[0].print()

# Show annotated image using OpenCV
annotated_frame = results[0].plot()
cv2.imshow("Detection", annotated_frame)
cv2.waitKey(0)
cv2.destroyAllWindows()
