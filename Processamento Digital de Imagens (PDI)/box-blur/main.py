import time
import sys
import cv2
import numpy as np


def naiveBlur(img, windowX, windowY):
    rows, cols = img.shape[:2]
    blurred = np.zeros_like(img)

    radiusX = windowX // 2
    radiusY = windowY // 2

    # vetorizado (aka magia negra)
    # feito com amor com ajuda de IA
    # mas eu sei explicar ok
    # bjs do verde chocho
    for rowOffset in range(-radiusY, radiusY + 1):
        for colOffset in range(-radiusX, radiusX + 1):
            startRow = rowOffset + radiusY
            startCol = colOffset + radiusX
            endRow = rows - radiusY + rowOffset
            endCol = cols - radiusX + colOffset

            blurred[radiusY : rows - radiusY, radiusX : cols - radiusX] += img[
                startRow:endRow, startCol:endCol
            ]

    blurred /= windowX * windowY

    # versão humana
    # mesma ideia, mas pessoas normais entendem
    # obs.: espero que vc entenda
    # mas se não entender, não é pessoal ok
    #
    # for row in range(radius, rows - radius):
    #     for col in range(radius, cols - radius):
    #         for channel in range(channels):
    #             sum = 0
    #
    #             for winRow in range(row - radius, row + radius):
    #                 for winCol in range(col - radius, col + radius):
    #                     sum = sum + img[winRow, winCol, channel]
    #
    #             average = sum / (window * window)
    #             blurred[row, col, channel] = average

    return blurred


def compareImages(img, cvResult, windowX, windowY):
    if not img.shape == cvResult.shape:
        print("As imagens comparadas não possuem as mesmas dimensões.")
        sys.exit()

    rows, cols = img.shape[:2]
    radiusY = windowY // 2
    radiusX = windowX // 2

    # ó que bonitinho isso eu fiz sozinho :)
    imgWithoutBorders = img[radiusY : rows - radiusY, radiusX : cols - radiusX]
    cvWithoutBorders = cvResult[radiusY : rows - radiusY, radiusX : cols - radiusX]

    # tentei fazer comparação convertendo para int, mas não dava certo
    # talvez a conversão arredondava os valores de maneira inconsistente
    # ou talvez eu implementei tudo errado :(
    # mas aparentemente comparar os valores como floats e ter como limite
    # 1 nível de cor resolve isso
    return np.allclose(imgWithoutBorders, cvWithoutBorders, atol=1 / 255, rtol=0)


def integralBlur(img, windowX, windowY):
    integral = img.copy()
    rows, cols, channels = integral.shape

    # ó que troço feio pelamor
    for row in range(rows):
        for col in range(cols):
            for chan in range(channels):
                if row > 0:
                    integral[row, col, chan] += integral[row - 1, col, chan]
                if col > 0:
                    integral[row, col, chan] += integral[row, col - 1, chan]
                if row > 0 and col > 0:
                    integral[row, col, chan] -= integral[row - 1, col - 1, chan]

    radiusX = windowX // 2
    radiusY = windowY // 2

    blurred = np.zeros_like(img)

    # retiro o que eu disse. ISSO é feio
    for row in range(radiusY, rows - radiusY):
        for col in range(radiusX, cols - radiusX):
            for chan in range(channels):
                top = row - radiusY - 1
                left = col - radiusX - 1

                topLeft = integral[top, left, chan] if top >= 0 and left >= 0 else 0
                bottomLeft = integral[row + radiusY, left, chan] if left >= 0 else 0
                topRight = integral[top, col + radiusX, chan] if top >= 0 else 0
                bottomRight = integral[row + radiusY, col + radiusX, chan]

                blurred[row, col, chan] = topLeft + bottomRight - topRight - bottomLeft

    blurred /= windowX * windowY

    return blurred


def separableFilter(img, windowX, windowY):
    horizontalSums = np.zeros_like(img)
    rows, cols = horizontalSums.shape[:2]

    radiusY = windowY // 2
    radiusX = windowX // 2

    for offsetX in range(-radiusX, radiusX + 1):
        startX = offsetX + radiusX
        endX = cols + offsetX - radiusX

        horizontalSums[:, radiusX : cols - radiusX] += img[:, startX:endX]

    blurred = np.zeros_like(horizontalSums)

    for offsetY in range(-radiusY, radiusY + 1):
        startY = offsetY + radiusY
        endY = rows + offsetY - radiusY

        blurred[radiusY : rows - radiusY, radiusX : cols - radiusX] += horizontalSums[
            startY:endY, radiusX : cols - radiusX
        ]

    blurred /= windowX * windowY

    return blurred


def main():
    if len(sys.argv) < 3:
        print("Uso: python3 main.py [caminho para imagem] [tamanho da janela]")
        print("\nEx.: python3 main.py ./imagem.bmp 3\n")
        print("- opcionalmente, pode especificar os tamanhos X e Y da janela")
        print("Ex.: python3 main.py ./imagem.bmp 9 15")
        return

    inputImage = sys.argv[1]
    sizeX = int(sys.argv[2])
    sizeY = int(sys.argv[3] if len(sys.argv) > 3 else sys.argv[2])

    if sizeY % 2 == 0 or sizeX % 2 == 0:
        print("Erro: Janela deve ser ímpar")
        sys.exit()

    img = cv2.imread(inputImage)
    if img is None:
        print("Erro abrindo a imagem.\n")
        sys.exit(1)

    img = img.astype(np.float32) / 255

    print("Executando `cv2.boxFilter` para comparação...")
    startTime = time.perf_counter()
    cvBlur = cv2.boxFilter(img, -1, (sizeX, sizeY))
    endTime = time.perf_counter()
    print(f"Tempo de execução: {(endTime - startTime):.4f} segundos")

    ###########
    # INGÊNUO #
    ###########

    print("\nAlgoritmo Ingênuo...")
    startTime = time.perf_counter()
    naivelyBlurred = naiveBlur(img, sizeX, sizeY)
    endTime = time.perf_counter()
    print(f"Tempo de execução: {(endTime - startTime):.4f} segundos")
    print(
        f"Resultado do blur: {'igual a' if compareImages(naivelyBlurred, cvBlur, sizeX, sizeY) else 'diferente d'}o OpenCV"
    )

    print("Salvando em `ingenuo.bmp`")
    cv2.imwrite("ingenuo.bmp", (naivelyBlurred * 255).astype(np.uint8))

    #############
    # SEPARÁVEL #
    #############

    print("\nFiltro Separável...")
    startTime = time.perf_counter()
    separableBlurred = separableFilter(img, sizeX, sizeY)
    endTime = time.perf_counter()
    print(f"Tempo de execução: {(endTime - startTime):.4f} segundos")
    print(
        f"Resultado do blur: {'igual a' if compareImages(separableBlurred, cvBlur, sizeX, sizeY) else 'diferente d'}o OpenCV"
    )

    print("Salvando em `separavel.bmp`")
    cv2.imwrite("separavel.bmp", (separableBlurred * 255).astype(np.uint8))

    ############
    # INTEGRAL #
    ############

    print("\nImagem Integral...")
    startTime = time.perf_counter()
    integralBlurred = integralBlur(img, sizeX, sizeY)
    endTime = time.perf_counter()
    print(f"Tempo de execução: {(endTime - startTime):.4f} segundos")
    print(
        f"Resultado do blur: {'igual a' if compareImages(integralBlurred, cvBlur, sizeX, sizeY) else 'diferente d'}o OpenCV"
    )

    print("Salvando em `integral.bmp`")
    cv2.imwrite("integral.bmp", (integralBlurred * 255).astype(np.uint8))


if __name__ == "__main__":
    main()