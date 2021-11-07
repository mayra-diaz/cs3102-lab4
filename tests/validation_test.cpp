#include <gtest/gtest.h>

#include <iostream>

#include "PNMImage.hpp"
#include "PRQuadTreeImage.hpp"

using namespace utec::spatial;

TEST(QuadTest, BasicTest) {
  PRQuadTreeImage qt;
  qt.load("../tests/images/utec.pgm");
  qt.compress("../tests/images/prueba.qt");
  qt.decompress("../tests/images/prueba.qt");
  qt.convertToPGM("../tests/images/prueba.pgm");

  PNMImage comparator;
  EXPECT_TRUE(comparator.equals("../tests/images/utec.pgm", "../tests/images/prueba.pgm"));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
