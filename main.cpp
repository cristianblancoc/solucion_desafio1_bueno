#include <QImage>
#include <QString>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// Cargar imagen BMP
vector<unsigned char> loadImage(const QString& filename, int& width, int& height) {
    QImage img(filename);
    vector<unsigned char> data;

    if (img.isNull()) return data;

    img = img.convertToFormat(QImage::Format_RGB888);
    width = img.width();
    height = img.height();

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            QRgb pixel = img.pixel(x, y);
            data.push_back(qRed(pixel));
            data.push_back(qGreen(pixel));
            data.push_back(qBlue(pixel));
        }

    return data;
}

// Guardar imagen BMP
bool saveImage(const vector<unsigned char>& data, int width, int height, const QString& filename) {
    QImage img(width, height, QImage::Format_RGB888);
    int index = 0;

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            if (index + 2 >= data.size()) break;
            img.setPixel(x, y, qRgb(data[index], data[index + 1], data[index + 2]));
            index += 3;
        }

    return img.save(filename);
}

// Cargar semilla desde M1.txt
vector<unsigned char> loadSemilla(const QString& filename) {
    ifstream file(filename.toStdString(), ios::binary);
    vector<unsigned char> semilla;
    char c;
    while (file.get(c))
        semilla.push_back(static_cast<unsigned char>(c));
    return semilla;
}

// Imprimir semilla en formato RGB (3 en 3)
void imprimirSemilla(const vector<unsigned char>& semilla) {
    cout << "\nContenido de la semilla (M1.txt):\n";
    for (size_t i = 0; i + 2 < semilla.size(); i += 3) {
        cout << (int)semilla[i] << " "
             << (int)semilla[i + 1] << " "
             << (int)semilla[i + 2] << endl;
    }
}

// Comparación simple entre dos vectores
bool compararSimple(const vector<unsigned char>& img1, const vector<unsigned char>& img2) {
    if (img1.size() != img2.size()) return false;
    for (size_t i = 0; i < img1.size(); ++i) {
        if (img1[i] != img2[i]) return false;
    }
    return true;
}

// XOR entre bytes
unsigned char xorByte(unsigned char a, unsigned char b) {
    return a ^ b;
}

// Rotación izquierda de byte
unsigned char rotateLeft(unsigned char val, int bits) {
    return (val << bits) | (val >> (8 - bits));
}

int main() {
    int w = 0, h = 0;

    // Cargar imágenes de entrada
    auto P3 = loadImage("P3.bmp", w, h);
    auto IM = loadImage("I_M.bmp", w, h);

    if (P3.empty() || IM.empty()) {
        cout << "Error cargando P3.bmp o I_M.bmp\n";
        return -1;
    }

    // Operación 1: XOR inversa
    vector<unsigned char> xorInversa;
    for (size_t i = 0; i < P3.size(); ++i)
        xorInversa.push_back(xorByte(P3[i], IM[i]));
    saveImage(xorInversa, w, h, "xor_inversa.bmp");

    // Operación 2: Rotación izquierda
    vector<unsigned char> rotacionInversa;
    for (unsigned char b : xorInversa)
        rotacionInversa.push_back(rotateLeft(b, 3));
    saveImage(rotacionInversa, w, h, "rotacion_inversa.bmp");

    // Operación 3: XOR final
    vector<unsigned char> imagenFinal;
    for (size_t i = 0; i < rotacionInversa.size(); ++i)
        imagenFinal.push_back(xorByte(rotacionInversa[i], IM[i]));
    saveImage(imagenFinal, w, h, "imagen_original.bmp");

    // Cargar máscara y semilla
    auto M = loadImage("M.bmp", w, h);
    auto M1 = loadSemilla("M1.txt");

    // Mostrar contenido de semilla
    imprimirSemilla(M1);

    // Comparar imagen final con máscara
    cout << "\nComparando imagen original con imagen 1 (M.bmp): ";
    if (compararSimple(imagenFinal, M)) {
        cout << "Se parecen\n";
    } else {
        cout << "No se parecen\n";
    }

    // Comparar imagen final con semilla
    cout << "Comparando imagen original con imagen 2 (M1.txt): ";
    if (compararSimple(imagenFinal, M1)) {
        cout << "Se parecen\n";
    } else {
        cout << "No se parecen\n";
    }

    return 0;
}
