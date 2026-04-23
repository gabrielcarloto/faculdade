import cv2
import numpy as np


def main():
    img = cv2.imread("./Wind Waker GC.bmp")
    img_luminance = cv2.cvtColor(img, cv2.COLOR_BGR2HLS)[:, :, 1]
    mask = np.where(img_luminance > 130, 255, 0).astype(np.uint8)
    bright_pass = cv2.bitwise_and(img, img, mask=mask)
    cv2.imwrite("brightpass.bmp", bright_pass)

    blur1 = bright_pass.copy()
    for _ in range(5):
        blur1 = cv2.boxFilter(blur1, -1, (8, 8))
    cv2.imwrite("blur_0.bmp", blur1)

    blur2 = bright_pass.copy()
    for _ in range(5):
        blur2 = cv2.boxFilter(blur2, -1, (15, 15))
    cv2.imwrite("blur_1.bmp", blur2)

    blur3 = bright_pass.copy()
    for _ in range(5):
        blur3 = cv2.boxFilter(blur3, -1, (30, 30))
    cv2.imwrite("blur_2.bmp", blur3)

    blur4 = bright_pass.copy()
    for _ in range(5):
        blur4 = cv2.boxFilter(blur4, -1, (50, 50))
    cv2.imwrite("blur_3.bmp", blur4)

    img = img.astype(np.float32) / 255
    blur1 = blur1.astype(np.float32) / 255
    blur2 = blur2.astype(np.float32) / 255
    blur3 = blur3.astype(np.float32) / 255
    blur4 = blur4.astype(np.float32) / 255

    blurs = blur1 + blur2 + blur3 + blur4

    bloom = 0.8 * img + 0.2 * blurs
    bloom = np.clip(bloom * 255, 0, 255).astype(np.uint8)

    cv2.imwrite("bloom.bmp", bloom)


if __name__ == "__main__":
    main()
