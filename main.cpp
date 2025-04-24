#include <QString>
#include <iostream>
#include <fstream>
#include <QCoreApplication>
#include <QImage>
#include <vector>

using namespace std;

// Función para cargar los píxeles de una imagen BMP
unsigned char* loadPixels(const QString &filename, int &width, int &height) {
    QImage img(filename);

    if (img.isNull()) {
        return nullptr; // No se pudo cargar la imagen
    }

    width = img.width();
    height = img.height();

    int dataSize = width * height * 3;
    unsigned char* data = new unsigned char[dataSize];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = img.pixel(x, y);
            int index = (y * width + x) * 3;
            data[index] = qRed(pixel);   // Rojo
            data[index + 1] = qGreen(pixel); // Verde
            data[index + 2] = qBlue(pixel);  // Azul
        }
    }

    return data;
}

// Función para exportar los píxeles a una imagen BMP
bool exportImage(const unsigned char* data, int width, int height, const QString &filename) {
    QImage img(width, height, QImage::Format_RGB888);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            unsigned char red = data[index];
            unsigned char green = data[index + 1];
            unsigned char blue = data[index + 2];
            img.setPixel(x, y, qRgb(red, green, blue));
        }
    }

    return img.save(filename); // Guardar la imagen en el archivo
}

// Función de XOR
unsigned char applyXOR(unsigned char value, unsigned char key) {
    return value ^ key;
}

// Función de rotación a la izquierda
unsigned char rotateLeft(unsigned char value, int bits) {
    return (value << bits) | (value >> (8 - bits));
}

// Función para leer un archivo de texto y almacenarlo en un arreglo
std::vector<unsigned char> loadTextFile(const QString &filename) {
    std::ifstream file(filename.toStdString(), std::ios::binary);
    std::vector<unsigned char> content;

    if (!file.is_open()) {
        cout << "No se pudo abrir el archivo de texto: " << filename.toStdString() << endl;
        return content;  // Regresa un arreglo vacío si no se pudo abrir el archivo
    }

    char byte;
    while (file.get(byte)) {
        content.push_back(static_cast<unsigned char>(byte));  // Leer byte a byte
    }

    file.close();
    return content;
}

// Función para comparar dos imágenes byte a byte
void compararImagenesConTexto(const unsigned char* imgData, const std::vector<unsigned char>& textData, int width, int height) {
    int dataSize = width * height * 3;  // Suponiendo que las imágenes son RGB (3 canales)
    int diferenciaTotal = 0;

    for (int i = 0; i < dataSize; ++i) {
        if (i < textData.size() && imgData[i] != textData[i]) {
            cout << "Byte diferente en índice " << i << ": Imagen = " << (int)imgData[i]
                 << ", Texto = " << (int)textData[i] << endl;
            diferenciaTotal++;
        }
    }

    if (diferenciaTotal == 0) {
        cout << "La imagen y el archivo de texto son idénticos." << endl;
    } else {
        cout << "Total de diferencias: " << diferenciaTotal << endl;
    }
}

// Función principal
int main() {
    QString archivoentrada = "P3.bmp";
    QString archivodeentrada2 = "I_M.bmp";
    QString archivodesalida = "xor_inversa.bmp";

    int width = 0, height = 0;
    unsigned char* p3Data = loadPixels(archivoentrada, width, height);
    unsigned char* imData = loadPixels(archivodeentrada2, width, height);

    if (!p3Data || !imData) {
        cout << "Error al cargar P3.bmp o I_M.bmp" << endl;
        return -1;
    }

    // PASO 1: XOR INVERSO
    int dataSize = width * height * 3;
    unsigned char* xorInversaData = new unsigned char[dataSize];

    for (int i = 0; i < dataSize; ++i) {
        xorInversaData[i] = applyXOR(p3Data[i], imData[i]);
    }

    bool exportado1 = exportImage(xorInversaData, width, height, archivodesalida);
    if (!exportado1) {
        cout << "Error al guardar xor_inversa.bmp" << endl;
        return -1;
    }

    delete[] p3Data;
    delete[] imData;

    // PASO 2: ROTACIÓN INVERSA
    QString archivoRotado = "rotacion_inversa.bmp";

    int width2 = 0, height2 = 0;
    unsigned char* xorInversaLoaded = loadPixels(archivodesalida, width2, height2);

    if (!xorInversaLoaded) {
        cout << "Error al cargar xor_inversa.bmp" << endl;
        return -1;
    }

    int dataSize2 = width2 * height2 * 3;
    unsigned char* rotadoData = new unsigned char[dataSize2];

    for (int i = 0; i < dataSize2; ++i) {
        rotadoData[i] = rotateLeft(xorInversaLoaded[i], 3);
    }

    bool exportado2 = exportImage(rotadoData, width2, height2, archivoRotado);
    if (!exportado2) {
        cout << "Error al guardar rotacion_inversa.bmp" << endl;
        return -1;
    }

    delete[] xorInversaData;
    delete[] xorInversaLoaded;

    // PASO 3: XOR FINAL
    QString archivoFinal = "imagen_original.bmp";

    int width3 = 0, height3 = 0;
    unsigned char* rotadoFinal = loadPixels(archivoRotado, width3, height3);
    unsigned char* imFinal = loadPixels(archivodeentrada2, width3, height3); // mismo I_M

    if (!rotadoFinal || !imFinal) {
        cout << "Error al cargar rotacion_inversa.bmp o I_M.bmp" << endl;
        return -1;
    }

    int dataSize3 = width3 * height3 * 3;
    unsigned char* imagenOriginal = new unsigned char[dataSize3];

    for (int i = 0; i < dataSize3; ++i) {
        imagenOriginal[i] = applyXOR(rotadoFinal[i], imFinal[i]);
    }

    bool exportadoFinal = exportImage(imagenOriginal, width3, height3, archivoFinal);
    if (!exportadoFinal) {
        cout << "Error al guardar imagen_original.bmp" << endl;
    }

    delete[] rotadoData;
    delete[] rotadoFinal;
    delete[] imFinal;
    delete[] imagenOriginal;

    // COMPARACIÓN ENTRE LAS 3 IMÁGENES Y LA SEMILLA (M1)
    QString archivoMascara = "M.bmp";  // La máscara es una imagen BMP
    QString archivoSemilla = "M1.txt";  // La semilla es un archivo de texto

    int wm = 0, hm = 0;
    unsigned char* mascara = loadPixels(archivoMascara, wm, hm);  // Cargar la imagen de la máscara

    if (!mascara) {
        cout << "No se pudo cargar M.bmp" << endl;
        return -1;
    }

    // Leer el archivo de texto M1
    std::vector<unsigned char> semilla = loadTextFile(archivoSemilla);

    // Comparar las 3 imágenes generadas con la máscara y la semilla
    cout << "Comparando xor_inversa.bmp con la máscara y semilla:" << endl;
    unsigned char* xorInversaDataLoaded = loadPixels("xor_inversa.bmp", wm, hm);
    compararImagenesConTexto(xorInversaDataLoaded, semilla, wm, hm);
    delete[] xorInversaDataLoaded;

    cout << "\nComparando rotacion_inversa.bmp con la máscara y semilla:" << endl;
    unsigned char* rotacionInversaDataLoaded = loadPixels("rotacion_inversa.bmp", wm, hm);
    compararImagenesConTexto(rotacionInversaDataLoaded, semilla, wm, hm);
    delete[] rotacionInversaDataLoaded;

    cout << "\nComparando imagen_original.bmp con la máscara y semilla:" << endl;
    unsigned char* imagenOriginalDataLoaded = loadPixels("imagen_original.bmp", wm, hm);
    compararImagenesConTexto(imagenOriginalDataLoaded, semilla, wm, hm);
    delete[] imagenOriginalDataLoaded;

    delete[] mascara;

    return 0;
}
