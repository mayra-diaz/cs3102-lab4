#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <queue>
#include "../utils/pnm.hpp"

#include "SpatialImageBase.hpp"

namespace utec {
namespace spatial {

/**
 * PRQuadTreeImage implementation
 */

bool compare(pnm::rgb_pixel n0, pnm::rgb_pixel n1, pnm::rgb_pixel n2,
             pnm::rgb_pixel n3);
//bool compare(pnm::gray_pixel n0, pnm::gray_pixel n1, pnm::gray_pixel n2,
//             pnm::gray_pixel n3);

using str = std::string;
using sizet = std::size_t;
using coord = std::array<sizet, 2>;

using pixel_t = pnm::rgb_pixel;
using image_t = pnm::image<pixel_t>;

struct Bound{
  std::size_t sx, sy, ex, ey;
};

template <typename pixel_>
struct Node {
  pixel_ pixel;
  bool is_leaf;
  Bound bounds;
  std::array<Node<pixel_>*, 4> children{};

  Node(pixel_ p) : pixel(p), is_leaf(true){};
  Node(Node<pixel_>* n0, Node<pixel_>* n1, Node<pixel_>* n2, Node<pixel_>* n3): is_leaf(false) {
    children[0] = n0;
    children[1] = n1;
    children[2] = n2;
    children[3] = n3;
  }
};

class PRQuadTreeImage : public SpatialImageBase {
 private:
  image_t img;

  Node<pixel_t>* insert(coord s, coord e){
    //std::cout << s[0] << ' ' << s[1] << "  -  " << e[0] << ' ' << e[1] << '\n';
    if (s[0] + 1 == e[0] && s[1] + 1 == e[1])
      return new Node<pixel_t>(img[s[0]][s[1]]);
    sizet c = (s[1] + e[1]) / 2;
    sizet r = (s[0] + e[0]) / 2;

    auto n0 = insert(s, coord{r,c});
    auto n1 = insert(coord{s[0], c}, coord{r, e[1]});
    auto n2 = insert(coord{r, s[1]}, coord{e[0], c});
    auto n3 = insert(coord{r, c}, e);

    if (compare(n0->pixel, n1->pixel, n2->pixel, n3->pixel)) {
      delete n1, n2, n3;
      //std::cout << "Leaf\n" << s[0] << ' ' << s[1] << "  -  " << e[0] << ' ' << e[1] << "\n\n";
      return n0;
    }
    //std::cout << "Node\n" << s[0] << ' ' << s[1] << "  -  " << e[0] << ' ' << e[1] << "\n\n";
    return new Node<pixel_t>(n0, n1, n2, n3);
  }

  void compress_helper(Node<pixel_t>* root, const std::string& filename){
    std::cout << "Starting to compress\n";
    std::queue<Node<pixel_t>*> q;

    q.push(root);
    while (!q.empty()){
      auto n = q.front();
      //std::cout << "Node with bounds: " << n->bounds.sy << ' ' << n->bounds.sx
        //        << ' ' << n->bounds.ey << ' ' << n->bounds.ex << '\n';
      if (n->is_leaf)
        write_node(n, filename);
      else {
        for (auto c : n->children) q.push(c);
      }
    }
  }

  void write_node(Node<pnm::rgb_pixel>* n, const std::string& filename){
    std::ofstream file(filename, std::ios::out);
    if (file.is_open()){
      file << n->bounds.sx << ' ' << n->bounds.sy << ' '
            << n->bounds.ex << ' ' << n->bounds.ey << ' '
            << n->pixel.red << ' ' << n->pixel.green << ' '  << n->pixel.blue << '\n';
    }
    file.close();
  }

  void write_node(Node<pnm::gray_pixel>* n, const std::string& filename){
    std::ofstream file(filename, std::ios::out);
    if (file.is_open())
      file  << n->bounds.sx << ' ' << n->bounds.sy << ' '
            << n->bounds.ex << ' ' << n->bounds.ey << ' '
            << n->pixel.value << '\n';
    file.close();
  }

 public:
  void load(const std::string& filename) override {
    img = pnm::read(filename);
  }

  void compress(const std::string& filename) override {
    std::ofstream file(filename, std::ios::out);
    if (file.is_open()) file << img.height() << ' ' << img.width() << '\n';
    file.close();
    auto root = insert(coord{0, 0}, coord{img.height(), img.width()});
    std::cout << "Insertion done\n";
    
    std::cout << "Root with bounds: " << root->bounds.sy << ' ' << root->bounds.sx
              << ' ' << root->bounds.ey << ' ' << root->bounds.ex << '\n';
    compress_helper(root, filename);
  }

  void decompress(const std::string& filename) override {

  }

  void convertToPGM(const std::string& filename) override {

  }
};


bool compare(pnm::rgb_pixel n0, pnm::rgb_pixel n1, pnm::rgb_pixel n2,
             pnm::rgb_pixel n3){
  bool blue = n0.blue  == n1.blue    && n0.blue  == n2.blue    && n0.blue  == n3.blue;
  bool green = n0.green == n1.green   && n0.green == n2.green   && n0.green == n3.green;
  bool red = n0.red   == n1.red     && n0.red   == n2.red     && n0.red   == n3.red;
  return blue && green && red;
}
/*
bool compare(pnm::gray_pixel n0, pnm::gray_pixel n1, pnm::gray_pixel n2,
             pnm::gray_pixel n3){
  return n0.value == n1.value && n0.value == n2.value && n0.value == n3.value;
}*/

}  // namespace spatial
}  // namespace utec
