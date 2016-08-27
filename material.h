#ifndef MATERIAL_H
#define MATERIAL_H
struct Colour
{
   float r;
   float g;
   float b;
};

struct Material
{
    Colour ambient;
    Colour diffuse;
    Colour specular;
    float shininess;
};
#endif // MATERIAL_H
