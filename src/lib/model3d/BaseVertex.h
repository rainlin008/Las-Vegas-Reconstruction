//***************************************************************
//
//   \brief Base class for all vector classes
//
//   Base Vector for all purposes, including mathematical
//   operations ans rendering.
//
//  \autor Thomas Wiemann
//
//***************************************************************


#ifndef __BASE_VERTEX_H__
#define __BASE_VERTEX_H__

#include "gl.h"
#include "glut.h"

#include <iostream>
#include <math.h>

using namespace std;

class Matrix4;

class BaseVertex{

public:

  BaseVertex();
  BaseVertex(float x, float y, float z);
  BaseVertex(const BaseVertex &o);

  virtual ~BaseVertex(){};

  float length();

  BaseVertex cross(const BaseVertex other) const;

  void rotate(Matrix4 m);

  virtual void crossTo(const BaseVertex other);
  virtual inline void render();

  virtual float operator*(const BaseVertex other) const;

  virtual BaseVertex operator*(const float scale) const;
  virtual BaseVertex operator+(const BaseVertex other) const;
  virtual BaseVertex operator-(const BaseVertex other) const;

  virtual void operator-=(const BaseVertex other);
  virtual void operator+=(const BaseVertex other);
  virtual void operator*=(const float scale);
  virtual void operator/=(const float scale);

  virtual bool operator==(const BaseVertex &other) const;
  virtual bool operator!=(const BaseVertex &other) const;

  virtual float operator[](const int index);

  void transform(const Matrix4 &m);
  void transformRM(const Matrix4 &m);
  void transformCM(const Matrix4 &m);
  void rotate(const Matrix4 &m);
  void rotateCM(const Matrix4 &m);
  void rotateRM(const Matrix4 &m);

  float x;
  float y;
  float z;

private:
	static const float epsilon;
};


inline ostream& operator<<(ostream& os, const BaseVertex v){
  os << "BaseVertex: " << v.x << " " << v.y << " " << v.z << endl;
  return os;
}

inline void BaseVertex::render(){
  glVertex3f(x, y, z);
}


typedef BaseVertex Vertex;


#endif