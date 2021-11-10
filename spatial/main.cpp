#include "PRQuadTreeImage.hpp"

int main() {
  const std::string filename = "exam.ppm";
  std::cout << filename << '\n';
  utec::spatial::PRQuadTreeImage tree;
  tree.load(filename);
  const std::string prueba = "prueba.qt";
  std::cout << "Compressing\n";
  tree.compress(prueba);
}