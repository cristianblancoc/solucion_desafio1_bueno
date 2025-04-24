#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QImage>
#include <QDir>
#include <cstring>

using namespace std;

// XOR
unsigned char applyXOR(unsigned char value, unsigned char mask) {
    return value ^ mask;
}

// Rotación de 3 bits a la izquierda
unsigned char rotateBitsLeft(unsigned char value, int rotation) {
    rotation = rotation % 8;
    return (value << rotation) | (value >> (8 - rotation));
}

// Cargar píxeles de imagen
unsigned char* loadPixels(QString input, int &width, int &height) {
    QImage imagen(input);

    if (imagen.isNull()) {
        std::cout << "Error: No se pudo cargar la imagen BMP." << std::endl;
        return nullptr;
    }

    imagen = imagen.convertToFormat(QImage::Format_RGB888);
    width = imagen.width();
    height = imagen.height();
    int dataSize = width * height * 3;
    unsigned char* pixelData = new unsigned char[dataSize];

    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);
        unsigned char* dstLine = pixelData + y * width * 3;
        memcpy(dstLine, srcLine, width * 3);
    }

    return pixelData;
}

// Cargar máscara
unsigned char* loadMask(QString input, int width, int height) {
    QImage mask(input);

    if (mask.isNull()) {
        std::cout << "Error: No se pudo cargar la imagen de la máscara." << std::endl;
        return nullptr;
    }

    mask = mask.convertToFormat(QImage::Format_RGB888);
    unsigned char* maskData = new unsigned char[width * height * 3];

    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = mask.scanLine(y);
        unsigned char* dstLine = maskData + y * width * 3;
        memcpy(dstLine, srcLine, width * 3);
    }

    return maskData;
}

// Proceso de restauración: XOR y rotación
unsigned char* restoreImageAgain(unsigned char* inputData, unsigned char* maskData, int width, int height, unsigned char key) {
    int size = width * height * 3;
    unsigned char* newData = new unsigned char[size];

    for (int i = 0; i < size; ++i) {
        newData[i] = applyXOR(inputData[i], maskData[i]);
        newData[i] = rotateBitsLeft(newData[i], 3);
    }

    return newData;
}

// Guardar imagen
bool exportImage(unsigned char* pixelData, int width, int height, QString archivoSalida) {
    QImage outputImage(width, height, QImage::Format_RGB888);

    for (int y = 0; y < height; ++y) {
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    if (!outputImage.save(archivoSalida, "BMP")) {
        std::cout << "Error: No se pudo guardar la imagen BMP." << std::endl;
        return false;
    } else {
        std::cout << "Imagen BMP guardada correctamente como " << archivoSalida.toStdString() << std::endl;
        return true;
    }
}

// Función principal
int main() {
    QString archivoEntrada = "C:\\Users\\User\\Documents\\solucion_desafio1_bueno\\imagenRestaurada.bmp";
    QString archivoMascara = "C:\\Users\\User\\Documents\\solucion_desafio1_bueno\\M.bmp";
    QString archivoSalida = "C:\\Users\\User\\Documents\\solucion_desafio1_bueno\\imagenRestaurada2.bmp";

    int width = 0, height = 0;
    unsigned char key = 123;

    unsigned char* inputData = loadPixels(archivoEntrada, width, height);
    unsigned char* maskData = loadMask(archivoMascara, width, height);

    if (!inputData || !maskData) {
        std::cout << "Error al cargar las imágenes." << std::endl;
        return -1;
    }

    unsigned char* newImage = restoreImageAgain(inputData, maskData, width, height, key);
    exportImage(newImage, width, height, archivoSalida);

    delete[] inputData;
    delete[] maskData;
    delete[] newImage;

    return 0;
}
