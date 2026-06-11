import sys
import cv2
import numpy as np

BLACK = 0.7
WHITE = 0.85


def main():
    img = cv2.imread("./img/7.bmp")
    bg = cv2.imread("./cacholo.webp")

    if img is None or bg is None:
        sys.exit()

    rows, cols = img.shape[:2]
    bg = cv2.resize(bg, (cols, rows))

    img = img.astype(np.float32) / 255.0
    bg = bg.astype(np.float32) / 255.0
    mask = np.zeros_like(img)
    res = np.zeros_like(img)

    for row in range(rows):
        for col in range(cols):
            b, g, r = img[row, col]
            green_diff = max(g - max(b, r), 0.0)
            greendice = 1.0 - green_diff
            mask[row, col] = max(min((greendice - BLACK) / (WHITE - BLACK), 1), 0)
            img[row, col] = [b, g - green_diff, r]

    mask **= 3
    mask = cv2.GaussianBlur(mask, (3, 3), 0)

    res = img * mask + (bg * (1.0 - mask))

    mask = (mask * 255).astype(np.uint8)
    cv2.imwrite("./mask.bmp", mask)

    img = (img * 255).astype(np.uint8)
    cv2.imwrite("./img-menos-verde.bmp", img)

    res = (res * 255).astype(np.uint8)
    cv2.imwrite("./res.bmp", res)


if __name__ == "__main__":
    main()
