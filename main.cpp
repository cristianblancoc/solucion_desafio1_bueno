#include <QImage>
#include <QString>
#include <iostream>
#include <fstream>

using namespace std;

// ================== Cargar imagen BMP ==================
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

// ================== Guardar imagen BMP ==================
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

// ================== Cargar semilla desde archivo txt ==================
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

// ================== Imprimir semilla en RGB ==================
void printSeed(const unsigned char* seed, int size) {
    cout << "\nContenido de la semilla (M1.txt):\n";
    for (int i = 0; i + 2 < size; i += 3)
        cout << (int)seed[i] << " " << (int)seed[i + 1] << " " << (int)seed[i + 2] << endl;
}

// ================== Comparar dos arrays ==================
bool compareArrays(const unsigned char* a, const unsigned char* b, int size) {
    for (int i = 0; i < size; ++i)
        if (a[i] != b[i]) return false;
    return true;
}

// ================== XOR ==================
unsigned char xorByte(unsigned char a, unsigned char b) {
    return a ^ b;
}

// ================== Rotar bits a la izquierda ==================
unsigned char rotateLeft(unsigned char val, int bits) {
    return (val << bits) | (val >> (8 - bits));
}

// ================== MAIN ==================
int main() {
    int width = 0, height = 0, dataSize = 0;

    // Cargar imágenes
    unsigned char* P3 = loadImage("P3.bmp", width, height, dataSize);
    unsigned char* IM = loadImage("I_M.bmp", width, height, dataSize);

    if (!P3 || !IM) {
        cout << "Error cargando P3.bmp o I_M.bmp\n";
        return -1;
    }

    // ===== Paso 1: XOR inversa =====
    unsigned char* paso1 = new unsigned char[dataSize];
    for (int i = 0; i < dataSize; ++i)
        paso1[i] = xorByte(P3[i], IM[i]);
    saveImage(paso1, width, height, "xor_inversa.bmp");

    // ===== Paso 2: Rotación a la izquierda (3 bits) =====
    unsigned char* paso2 = new unsigned char[dataSize];
    for (int i = 0; i < dataSize; ++i)
        paso2[i] = rotateLeft(paso1[i], 3);
    saveImage(paso2, width, height, "rotacion_inversa.bmp");

    // ===== Paso 3: XOR final con I_M =====
    unsigned char* imagenFinal = new unsigned char[dataSize];
    for (int i = 0; i < dataSize; ++i)
        imagenFinal[i] = xorByte(paso2[i], IM[i]);
    saveImage(imagenFinal, width, height, "imagen_original.bmp");

    // ===== Cargar máscara (M.bmp) y semilla (M1.txt) =====
    int dummySize = 0;
    unsigned char* mascara = loadImage("M.bmp", width, height, dummySize);
    int semillaSize = 0;
    unsigned char* semilla = loadSeed("M1.txt", semillaSize);

    // Mostrar semilla
    printSeed(semilla, semillaSize);

    // Comparar con máscara
    cout << "\nComparando imagen original con imagen 1 (M.bmp): ";
    if (compareArrays(imagenFinal, mascara, dataSize)) {
        cout << "Se parecen\n";
    } else {
        cout << "No se parecen\n";
    }

    // Comparar con semilla
    cout << "Comparando imagen original con imagen 2 (M1.txt): ";
    if (semillaSize == dataSize && compareArrays(imagenFinal, semilla, dataSize)) {
        cout << "Se parecen\n";
    } else {
        cout << "No se parecen\n";
    }

    // Liberar memoria
    delete[] P3;
    delete[] IM;
    delete[] paso1;
    delete[] paso2;
    delete[] imagenFinal;
    delete[] mascara;
    delete[] semilla;

    return 0;
}
