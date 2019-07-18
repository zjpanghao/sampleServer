#ifndef INCLUDE_FACE_ENTITY_H
#define INCLUDE_FACE_ENTITY_H
namespace kface {
struct News {
  int id;
  std::string abstract;
  std::string url;
};

struct ObjectDetectResult {
  std::string category;
  float score;
  int x;
  int y;
  int width;
  int height;

};
}
#endif
