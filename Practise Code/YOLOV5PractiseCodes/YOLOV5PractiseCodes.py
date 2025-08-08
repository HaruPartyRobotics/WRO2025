import cv2 as campversion
from ultralytics import YOLO as holud
import numpy as np

moomoo = holud("yolov5x.pt")
moomoo.train(
    data=r"C:\Users\LENOVO\Downloads\RealLifeMouseDataset\data.yaml",
    epochs=3,
    imgsz=640,
)
moomoo_rasta = r"C:\Users\LENOVO\runs\detect\train3\weights\best.pt"
mountain = holud(moomoo_rasta)
tomato_path = r"C:\Users\LENOVO\Downloads\Screenshot 2025-08-05 134514.png"
tomato_pic = campversion.imread(
    r"C:\Users\LENOVO\Downloads\Screenshot 2025-08-05 134514.png"
)
russia = mountain.predict(source=tomato_path)
frustation = russia[0].boxes.conf.cpu().numpy()
for cnf in frustation:
    if cnf > 0.7:
        if isinstance(russia, list):
            for r in russia:
                r.save()
                r.show()
        else:
            russia.save()
            russia.show()
