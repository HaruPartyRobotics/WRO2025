from ultralytics import YOLO

model = YOLO("yolov8s.pt")
model.tune(
    data=r"C:\Users\LENOVO\OneDrive\Documents Of LEARN\GitHub\WRO2025\Vehicles Dataset\Data.yaml",
    epochs=10,
)
model.train(
    data=r"C:\Users\LENOVO\OneDrive\Documents Of LEARN\GitHub\WRO2025\Vehicles Dataset\Data.yaml",
    epochs=100,
    imgsz=1024,
    patience=5,
)
