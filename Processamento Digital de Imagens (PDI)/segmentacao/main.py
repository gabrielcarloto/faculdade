# ===============================================================================
# Exemplo: segmentação de uma imagem em escala de cinza.
# -------------------------------------------------------------------------------
# Autor: Bogdan T. Nassu
# Universidade Tecnológica Federal do Paraná
# ===============================================================================

import sys
import timeit
import numpy as np
import cv2

# ===============================================================================

INPUT_IMAGE = "arroz.bmp"

NEGATIVO = False
THRESHOLD = 0.6
ALTURA_MIN = 15
LARGURA_MIN = 15
N_PIXELS_MIN = 100

UNLABELED = -1

# ===============================================================================


def binariza(img, threshold):
    """Binarização simples por limiarização.

    Parâmetros: img: imagem de entrada. Se tiver mais que 1 canal, binariza cada
                  canal independentemente.
                threshold: limiar.

    Valor de retorno: versão binarizada da img_in."""
    return np.where(img > threshold, 1, 0).astype(np.float32)


# -------------------------------------------------------------------------------


def rotula(img, largura_min, altura_min, n_pixels_min):
    """Rotulagem usando flood fill. Marca os objetos da imagem com os valores
    [0.1,0.2,etc].

    Parâmetros: img: imagem de entrada E saída.
                largura_min: descarta componentes com largura menor que esta.
                altura_min: descarta componentes com altura menor que esta.
                n_pixels_min: descarta componentes com menos pixels que isso.

    Valor de retorno: uma lista, onde cada item é um vetor associativo (dictionary)
    com os seguintes campos:

    'label': rótulo do componente.
    'n_pixels': número de pixels do componente.
    'T', 'L', 'B', 'R': coordenadas do retângulo envolvente de um componente conexo,
    respectivamente: topo, esquerda, baixo e direita."""

    img = np.where(img == 1, UNLABELED, 0).astype(np.float32)
    rows, cols = img.shape[:2]

    currentLabel = 0
    data = []
    for row in range(rows):
        for col in range(cols):
            if img[row, col] == UNLABELED:
                currentLabel += 1
                initialInfo = {
                    "label": currentLabel,
                    "n_pixels": 0,
                    "T": row,
                    "B": row,
                    "L": col,
                    "R": col,
                }

                component = flood(img, row, col, initialInfo)
                data.append(component)

    filtered = [
        item
        for item in data
        if item["n_pixels"] >= n_pixels_min
        and (item["B"] - item["T"] + 1 >= altura_min)
        and (item["R"] - item["L"] + 1 >= largura_min)
    ]

    return filtered


def flood(img, y, x, info):
    info["n_pixels"] += 1
    img[y, x] = info["label"]

    info["T"] = min(info["T"], y)
    info["B"] = max(info["B"], y)
    info["L"] = min(info["L"], x)
    info["R"] = max(info["R"], x)

    neighbors = [(y - 1, x), (y + 1, x), (y, x - 1), (y, x + 1)]
    height, width = img.shape[:2]

    for ny, nx in neighbors:
        isWithinRows = 0 <= ny < height
        isWithinCols = 0 <= nx < width

        if isWithinRows and isWithinCols and img[ny, nx] == UNLABELED:
            flood(img, ny, nx, info)

    return info


# ===============================================================================


def main():
    # Abre a imagem em escala de cinza.
    img = cv2.imread(INPUT_IMAGE, cv2.IMREAD_GRAYSCALE)
    if img is None:
        print("Erro abrindo a imagem.\n")
        sys.exit()

    # É uma boa prática manter o shape com 3 valores, independente da imagem ser
    # colorida ou não. Também já convertemos para float32.
    img = img.reshape((img.shape[0], img.shape[1], 1))
    img = img.astype(np.float32) / 255

    # Mantém uma cópia colorida para desenhar a saída.
    img_out = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)

    # Segmenta a imagem.
    if NEGATIVO:
        img = 1 - img
    img = binariza(img, THRESHOLD)
    cv2.imshow("01 - binarizada", img)
    cv2.waitKey()
    cv2.imwrite("01 - binarizada.png", img * 255)

    start_time = timeit.default_timer()
    componentes = rotula(img, LARGURA_MIN, ALTURA_MIN, N_PIXELS_MIN)
    n_componentes = len(componentes)
    print("Tempo: %f" % (timeit.default_timer() - start_time))
    print("%d componentes detectados." % n_componentes)

    # Mostra os objetos encontrados.
    for c in componentes:
        cv2.rectangle(img_out, (c["L"], c["T"]), (c["R"], c["B"]), (0, 0, 1))

    cv2.imshow("02 - out", img_out)
    cv2.waitKey()
    cv2.imwrite("02 - out.png", img_out * 255)


if __name__ == "__main__":
    main()

# ===============================================================================
