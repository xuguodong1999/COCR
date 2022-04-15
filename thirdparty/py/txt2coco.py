import json
import os

import cv2

dataset = {"info": {
    "description": "soso8 in coco format",
    "url": "",
    "version": "1.0",
    "year": 2021,
    "contributor": "xuguodong1999",
    "date_created": "2021-01-22"},
    "images": [],
    "annotations": [],
    "categories": [
        {"supercategory:": "single-bond", "id": 0, "name": "single-bond"},
        {"supercategory:": "double-bond", "id": 1, "name": "double-bond"},
        {"supercategory:": "triple-bond", "id": 2, "name": "triple-bond"},
        {"supercategory:": "solid-wedge", "id": 3, "name": "solid-wedge"},
        {"supercategory:": "hash-wedge", "id": 4, "name": "hash-wedge"},
        {"supercategory:": "wavy-bond", "id": 5, "name": "wavy-bond"},
        {"supercategory:": "ring", "id": 6, "name": "ring"},
        {"supercategory:": "string", "id": 7, "name": "string"},
    ]
}

datapath = "~/datasets/soso-obj/JPEGImages"
annopath = "~/datasets/soso-obj/labels"
trainsetfile = "~/datasets/soso-obj/train.list"
outputpath = "~/datasets/soso-obj/"
phase = "soso8-coco-train"
classes = {"single-bond": 0, "double-bond": 1, "triple-bond": 2, "solid-wedge": 3,
           "hash-wedge": 4, "wavy-bond": 5, "ring": 6, "string": 7}

with open(trainsetfile) as f:
    count = 1
    cnt = 0
    annoid = 0
    for line in f:
        cnt += 1
        line = line.strip()

        name, ext = os.path.basename(line).split('.')

        imagepath = os.path.join(datapath, name + ".jpg")
        # no obstacle currently drop it
        txtpath = os.path.join(annopath, name + ".txt")
        if not os.path.exists(txtpath):
            print(txtpath)
            continue

        im = cv2.imread(imagepath)

        height, width, channels = im.shape

        if cnt % 1000 == 0:
            print(cnt)

        dataset["images"].append(
            {"license": 5, "file_name": line, "coco_url": "local", "height": height, "width": width,
             "flickr_url": "local", "id": cnt})
        with open(txtpath) as annof:
            annos = annof.readlines()

        for ii, anno in enumerate(annos):
            parts = anno.strip().split(' ')
            if len(parts) is not 5:
                continue
            class_id = int(parts[0])
            x = float(parts[1])
            y = float(parts[2])
            w = float(parts[3])
            h = float(parts[4])
            if parts[0].find("group") == -1:
                iscrowd = 0
            else:
                iscrowd = 1

            annoid = annoid + 1

            class_id += 1  # start from 1 instead of 0
            x1 = int((x - w / 2) * width)
            y1 = int((y - h / 2) * height)
            wid = int(w * width)
            hei = int(h * height)

            dataset["annotations"].append({
                "segmentation": [],
                "iscrowd": iscrowd,
                "area": wid * hei,
                "image_id": cnt,
                "bbox": [x1, y1, wid, hei],
                "category_id": class_id,
                "id": annoid
            })
        count += 1

json_name = os.path.join(outputpath, "{}.json".format(phase))

with open(json_name, 'w') as f:
    json.dump(dataset, f)
