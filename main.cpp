#include <QImage>
#include <QString>
#include <iostream>
#include <fstream>

using namespace std;

// Cargar imagen BMP
unsigned char* loadImage(const QString& filename, int& width, int& height, int& dataSize) {
    QImage img(filename);

    if (img.isNull()) return nullptr;

    img = img.convertToFormat(QImage::Format_RGB888);
    width = img.width();
    height = img.height();
    dataSize = width * height * 3;

    unsigned char* data = new unsigned char[dataSize];
    int index = 0;

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            QRgb pixel = img.pixel(x, y);
            data[index++] = qRed(pixel);
            data[index++] = qGreen(pixel);
            data[index++] = qBlue(pixel);
        }

    return data;
}

//  Guardar imagen BMP
bool saveImage(const unsigned char* data, int width, int height, const QString& filename) {
    QImage img(width, height, QImage::Format_RGB888);
    int index = 0;

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            img.setPixel(x, y, qRgb(data[index], data[index + 1], data[index + 2]));
            index += 3;
        }

    return img.save(filename);
}

// Cargar semilla desde archivo txt
unsigned char* loadSeed(const QString& filename, int& size) {
    ifstream file(filename.toStdString(), ios::binary);
    if (!file) return nullptr;

    file.seekg(0, ios::end);
    size = file.tellg();
    file.seekg(0, ios::beg);

    unsigned char* seed = new unsigned char[size];
    file.read(reinterpret_cast<char*>(seed), size);
    return seed;
}
// Cargar semilla desde archivo txt
unsigned char* loadSeed(const QString& filename, int& size2) {
    ifstream file(filename.toStdString(), ios::binary);
    if (!file) return nullptr;

    file.seekg(0, ios::end);
    size = file.tellg();
    file.seekg(0, ios::beg);

    unsigned char* seed = new unsigned char[size2];
    file.read(reinterpret_cast<char*>(seed), size2);
    return seed;
}

// Imprimir semilla en RGB
void printSeed(const unsigned char* seed, int size) {
    cout << "\nContenido de la semilla (M1.txt):\n";
    for (int i = 0; i + 2 < size; i += 3)
        cout << (int)seed[i] << " " << (int)seed[i + 1] << " " << (int)seed[i + 2] << endl;
}

// Imprimir semilla en RGB
void printSeed(const unsigned char* seed, int size2) {
    cout << "\nContenido de la semilla (M2.txt):\n";
    for (int i = 0; i + 2 < size; i += 3)
        cout << (int)seed[i] << " " << (int)seed[i + 1] << " " << (int)seed[i + 2] << endl;
}


// Comparar dos arrays
bool compareArrays(const unsigned char* a, const unsigned char* b, int size) {
    for (int i = 0; i < size; ++i)
        if (a[i] != b[i]) return false;
    return true;
}

//  XOR
unsigned char xorByte(unsigned char a, unsigned char b) {
    return a ^ b;
}

//  Rotar bitsa la izquierda
unsigned char rotateLeft(unsigned char val, int bits) {
    return (val << bits) | (val >> (8 - bits));
}


int main() {
    int width = 0, height = 0, dataSize = 0;

    // Cargar imágenes
    unsigned char* P3 = loadImage("P3.bmp", width, height, dataSize);
    unsigned char* IM = loadImage("I_M.bmp", width, height, dataSize);

    if (!P3 || !IM) {
        cout << "Error cargando P3.bmp o I_M.bmp\n";
        return -1;
    }

    //  Paso 1: XOR inversa
    unsigned char* paso1 = new unsigned char[dataSize];
    for (int i = 0; i < dataSize; ++i)
        paso1[i] = xorByte(P3[i], IM[i]);
    saveImage(paso1, width, height, "xor_inversa.bmp");

    // Paso 2: Rotación a la izquierda (3 bits)
    unsigned char* paso2 = new unsigned char[dataSize];
    for (int i = 0; i < dataSize; ++i)
        paso2[i] = rotateLeft(paso1[i], 3);
    saveImage(paso2, width, height, "rotacion_inversa.bmp");

    //  Paso 3: XOR final con I_M
    unsigned char* imagenFinal = new unsigned char[dataSize];
    for (int i = 0; i < dataSize; ++i)
        imagenFinal[i] = xorByte(paso2[i], IM[i]);
    saveImage(imagenFinal, width, height, "imagen_original.bmp");

    //  Cargar máscara (M.bmp) y semilla (M1.txt)
    int dummySize = 0;
    unsigned char* mascara = loadImage("M.bmp", width, height, dummySize);
    int semillaSize = 0;
    unsigned char* semilla = loadSeed("M1.txt", semillaSize);
    int semillaSize2 = 0;
    unsigned char* semilla1 = loadSeed("M2.txt", semillaSize2);

    // Mostrar semilla
    printSeed(semilla, semillaSize);
    printSeed(semilla1, semillaSize2);

    // Comparar imagen con máscara (M.bmp)
    cout << "\nComparando imagen con M: ";
    if (compareArrays(imagenFinal, mascara, dataSize)) {
        cout << "se parecen\n";
    } else {
        cout << "no se parecen\n";
    }

    // Comparar imagen original con M1.txt (rotación a la izquierda)
    cout << "Comparando imagen original con M1.txt (rotación a la izquierda): ";
    if (semillaSize == dataSize && compareArrays(paso2, semilla, dataSize)) {
        cout << "se parece\n";
    } else {
        cout << "no se parece\n";
    }

    // Comparar imagen original con M2.txt (primer XOR)
    cout << "Comparando imagen original con M2.txt (primer XOR): ";
    if (semillaSize2 == dataSize && compareArrays(paso1, semilla1, dataSize)) {
        cout << "se parece\n";
    } else {
        cout << "no se parece\n";
    }

    // Liberar memoria
    delete[] P3;
    delete[] IM;
    delete[] paso1;
    delete[] paso2;
    delete[] imagenFinal;
    delete[] mascara;
    delete[] semilla;
    delete[] semilla1;

    return 0;
}
