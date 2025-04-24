#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QImage>
#include <QDir>
#include <cstring>

using namespace std;

// Función para aplicar XOR
unsigned char applyXOR(unsigned char value, unsigned char key) {
    return value ^ key;
}

// Función para realizar rotación de bits a la izquierda
unsigned char rotateBitsLeft(unsigned char value, int rotation) {
    return (value << rotation) | (value >> (8 - rotation));
}

// Función para cargar los píxeles de la imagen BMP
unsigned char* loadPixels(QString input, int &width, int &height) {
    QImage imagen(input);

    if (imagen.isNull()) {
        std::cout << "Error: No se pudo cargar la imagen BMP." << std::endl;
        return nullptr;
    }

    // Verifica las dimensiones de la imagen cargada
    std::cout << "Dimensiones de la imagen cargada: " << imagen.width() << " x " << imagen.height() << std::endl;

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

// Función para cargar los píxeles de la máscara
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

// Función para restaurar la imagen con enmascaramiento y rotación
unsigned char* restoreImage(unsigned char* transformedData, unsigned char* maskData, int width, int height, unsigned char key) {
    int size = width * height * 3;
    unsigned char* restoredData = new unsigned char[size];

    // Aplicar XOR y rotación de 3 bits
    for (int i = 0; i < size; ++i) {
        // Primero XOR
        restoredData[i] = applyXOR(transformedData[i], key);
        // Luego rotación de 3 bits a la izquierda
        restoredData[i] = rotateBitsLeft(restoredData[i], 3);
        // Finalmente enmascaramiento
        restoredData[i] -= maskData[i];
    }

    return restoredData;
}

// Función para exportar la imagen BMP
bool exportImage(unsigned char* pixelData, int width, int height, QString archivoSalida) {
    QImage outputImage(width, height, QImage::Format_RGB888);

    // Copiar los píxeles modificados
    for (int y = 0; y < height; ++y) {
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    // Guardar la imagen en disco en formato BMP
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
    // Actualiza las rutas a la carpeta correcta
    QString archivoEntrada = "C:\\Users\\User\\Documents\\solucion_desafio1_bueno\\I_D.bmp";
    QString archivoMascara = "C:\\Users\\User\\Documents\\solucion_desafio1_bueno\\M.bmp";
    QString archivoSalida = "C:\\Users\\User\\Documents\\solucion_desafio1_bueno\\imagenRestaurada.bmp";

    int width = 0, height = 0;

    // Cargar imagen transformada y máscara
    unsigned char* transformedData = loadPixels(archivoEntrada, width, height);
    unsigned char* maskData = loadMask(archivoMascara, width, height);

    // Verificar si las imágenes se cargaron correctamente
    if (!transformedData || !maskData) {
        std::cout << "Error al cargar las imágenes." << std::endl;
        return -1;
    }

    // Establecer una clave para XOR (esto puede ser cualquier valor adecuado)
    unsigned char key = 123;  // Este es solo un ejemplo

    // Restaurar la imagen con enmascaramiento y rotación
    unsigned char* restoredImage = restoreImage(transformedData, maskData, width, height, key);

    // Exportar la imagen restaurada
    bool success = exportImage(restoredImage, width, height, archivoSalida);
    if (success) {
        std::cout << "Imagen restaurada y guardada exitosamente." << std::endl;
    } else {
        std::cout << "Hubo un error al guardar la imagen." << std::endl;
    }

    // Liberar la memoria de los datos de la imagen y la máscara
    delete[] transformedData;
    delete[] maskData;
    delete[] restoredImage;

    return 0;
}
