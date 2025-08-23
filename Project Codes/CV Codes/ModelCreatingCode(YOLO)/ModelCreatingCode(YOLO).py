from ultralytics import YOLO

model = YOLO("yolov8s.pt")

model.train(
    data=r"D:\GitHub\WRO2025\Vehicles Dataset\Data.yaml",
    lr0=0.01003,
    lrf=0.01081,
    momentum=0.93591,
    weight_decay=0.00053,
    warmup_epochs=3.04196,
    warmup_momentum=0.8,
    box=7.5,
    cls=0.46727,
    dfl=1.53594,
    hsv_h=0.0,
    hsv_s=0.12,
    hsv_v=0.12,
    translate=0.09,
    scale=0.22,
    fliplr=0.25,
    degrees=0.0,
    shear=0.0,
    perspective=0.0,
    flipud=0.0,
    mosaic=0.0,
    mixup=0.0,
    cutmix=0.0,
    copy_paste=0.0,
    erasing=0.0,
    epochs=100,
    batch=8,
    imgsz=640,
    dropout=0.12,
    patience=25,
    device="cpu",
    workers=0,
)

model.val(
    data=r"D:\GitHub\WRO2025\Vehicles Dataset\Data.yaml", batch=16, conf=0.45, iou=0.50
)
