#ifndef INCLUDE_FACE_ENTITY_H
#define INCLUDE_FACE_ENTITY_H
namespace kface {
struct NewsImage {
  int imageId;
  std::string head;
  std::string imageBase64;
};
struct News {
  int id;
  std::string abstract;
  std::string url;
  NewsImage image;
};

}
#endif
