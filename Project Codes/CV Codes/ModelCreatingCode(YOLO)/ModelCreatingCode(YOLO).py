from ultralytics import YOLO

model = YOLO("yolov8s.pt")
model.tune(
    data="data.yaml",
    epochs=50,
    iterations=15,
    optimizer="AdamW",
    batch=4,
    imgsz=640,
    augment=False,
    device="cpu",
    plots=False,
    val=False,
    verbose=False,
)
model.train(
    data=r"C:\Users\LENOVO\OneDrive\Documents Of LEARN\GitHub\WRO2025\Vehicles Dataset\Data.yaml",
    **model.tune_results.best_params,
    data="data.yaml",
    epochs=150,
    batch=8,
    imgsz=640,
    augment=True,
    hsv_h=0.0,
    hsv_s=0.12,
    hsv_v=0.12,
    translate=0.18,
    scale=0.22,
    fliplr=0.25,
    dropout=0.12,
    patience=35,
    mosaic=0.0,
    mixup=0.0,
    erasing=0.0,
    device="cpu",
    workers=0
)
model.val(data="data.yaml", batch=16, conf=0.45, iou=0.50)
