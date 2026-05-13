import cv2
import sys
import time
import numpy as np


def main():
    known = 114
    image = cv2.imread(f"{known}.bmp", cv2.IMREAD_GRAYSCALE)

    if image is None:
        sys.exit()

    binarized = adaptiveThresh(image)

    contours, _ = cv2.findContours(
        binarized, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE
    )

    solidities = getSolidities(contours)

    if np.std(solidities) < 0.03:
        print(f"Variância baixa.\nEstimativa final: {len(contours)}")
        print(f"Erro: {(abs(len(contours) - known) / known * 100):.2f}%")
        sys.exit()

    # VALOR MÁGICO?!?!?!?!?!?!!!!!
    # esse foi o valor que pareceu mais consistente para separar os grãos
    # em isolados e agrupados entre as imagens. pra não ficar o valor fixo,
    # faço esses cálculos matemágicos
    best_solidity_estimate = 0.92
    pure_grains = [s for s in solidities if s >= best_solidity_estimate]

    if pure_grains:
        pure_mean = np.mean(pure_grains)
        pure_std = np.std(pure_grains)
        solidity_thresh = pure_mean - (3 * pure_std)
    else:
        solidity_thresh = best_solidity_estimate

    isolated_grains = [
        contours[i] for i, s in enumerate(solidities) if s >= solidity_thresh
    ]

    grouped_grains = [
        contours[i] for i, s in enumerate(solidities) if s < solidity_thresh
    ]

    median_grain_area = np.median([cv2.contourArea(c) for c in isolated_grains])
    grouped_area_estimates = [
        cv2.contourArea(c) / median_grain_area for c in grouped_grains
    ]

    print("ESTIMATIVAS")
    print(f"\nGrãos isolados: {len(isolated_grains)}")
    print(f"Clusters: {len(grouped_grains)}")

    initial_estimate = round(len(isolated_grains) + sum(grouped_area_estimates))
    print(f"Grãos totais (área cv2): {initial_estimate}")

    rectangles = drawRectangles(image, contours, solidities, solidity_thresh)
    cv2.imwrite("./resultado_areas.bmp", rectangles)

    isolated_grains_areas = getAreas(isolated_grains, binarized)
    grouped_grains_areas = getAreas(grouped_grains, binarized)

    median_grain_area = np.median(isolated_grains_areas)
    grouped_area_estimates = [
        area * 1.01 / median_grain_area for area in grouped_grains_areas
    ]

    white_pixels_estimate = round(len(isolated_grains) + sum(grouped_area_estimates))
    print(f"Grãos totais (área absoluta): {white_pixels_estimate}")

    final_estimate = round((initial_estimate + white_pixels_estimate) / 2)
    print(f"\nEstimativa final: {final_estimate}")

    error = abs(final_estimate - known) / known * 100
    print(f"Erro: {error:.2f}%")

    if error > 1:
        time.sleep(2)
        print("Vergonhoso!!")


def getAreas(contours, img):
    areas = []

    for c in contours:
        x, y, w, h = cv2.boundingRect(c)
        rectangle = img[y : y + h, x : x + w]

        inner_contours, _ = cv2.findContours(
            rectangle, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE
        )

        if not inner_contours:
            areas.append(0)
            continue

        largest_contour = max(inner_contours, key=cv2.contourArea)

        mask = np.zeros_like(rectangle)
        cv2.drawContours(mask, [largest_contour], -1, 255, -1)

        white_pixels = np.count_nonzero(mask)
        areas.append(white_pixels)

    return areas


def drawRectangles(img, contours, solidities, solitity_thresh):
    img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)

    for contour, solidity in zip(contours, solidities):
        x, y, w, h = cv2.boundingRect(contour)
        is_isolated = solidity > solitity_thresh
        color = (0, 255, 0) if is_isolated else (0, 0, 255)
        cv2.rectangle(img, (x, y), (x + w, y + h), color, 1)

    return img


def getSolidities(contours):
    solidities = []

    for contour in contours:
        area = cv2.contourArea(contour)

        # sugestão do gemini :)
        # minha ideia inicial era usar a área absoluta
        # calculando o número de pixels brancos e dividindo pelo total
        # maaaaaaaaaas isso não dava bem certo
        # o fecho convexo melhora isso porque limita mais a área em volta do arroz
        # tornando o cálculo menos sensível à rotação
        hull = cv2.convexHull(contour)
        hull_area = cv2.contourArea(hull)
        solidity = area / hull_area if hull_area > 0 else 0
        solidities.append(solidity)

    return solidities


def adaptiveThresh(img):
    block_size = 201
    constant_c = -30

    adaptive = cv2.adaptiveThreshold(
        img,
        255,
        cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
        cv2.THRESH_BINARY,
        block_size,
        constant_c,
    )
    cv2.imwrite("./adaptive.bmp", adaptive)

    close_kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))
    adaptive = cv2.morphologyEx(adaptive, cv2.MORPH_OPEN, close_kernel)
    cv2.imwrite("./adaptive_open.bmp", adaptive)

    return adaptive


if __name__ == "__main__":
    main()
