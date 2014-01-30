// ======================================================================== //
// Copyright 2009-2013 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "../common/tutorial/tutorial_device.h"

/* scene data */
RTCScene g_scene = NULL;
Vec3f* colors = NULL;

/* render function to use */
renderPixelFunc renderPixel;

/* adds a cube to the scene */
unsigned int addCube (RTCScene scene_i)
{
  /* create a triangulated cube with 12 triangles and 8 vertices */
  unsigned int mesh = rtcNewTriangleMesh (scene_i, RTC_GEOMETRY_STATIC, 12, 8);

  /* set vertices */
  Vertex* vertices = (Vertex*) rtcMapBuffer(scene_i,mesh,RTC_VERTEX_BUFFER); 
  vertices[0].x = -1; vertices[0].y = -1; vertices[0].z = -1; 
  vertices[1].x = -1; vertices[1].y = -1; vertices[1].z = +1; 
  vertices[2].x = -1; vertices[2].y = +1; vertices[2].z = -1; 
  vertices[3].x = -1; vertices[3].y = +1; vertices[3].z = +1; 
  vertices[4].x = +1; vertices[4].y = -1; vertices[4].z = -1; 
  vertices[5].x = +1; vertices[5].y = -1; vertices[5].z = +1; 
  vertices[6].x = +1; vertices[6].y = +1; vertices[6].z = -1; 
  vertices[7].x = +1; vertices[7].y = +1; vertices[7].z = +1; 
  rtcUnmapBuffer(scene_i,mesh,RTC_VERTEX_BUFFER); 

  /* create triangle color array */
  colors = new Vec3f[12];

  /* set triangles and colors */
  int tri = 0;
  Triangle* triangles = (Triangle*) rtcMapBuffer(scene_i,mesh,RTC_INDEX_BUFFER);
  
  // left side
  colors[tri] = Vec3f(1,0,0); triangles[tri].v0 = 0; triangles[tri].v1 = 2; triangles[tri].v2 = 1; tri++;
  colors[tri] = Vec3f(1,0,0); triangles[tri].v0 = 1; triangles[tri].v1 = 2; triangles[tri].v2 = 3; tri++;

  // right side
  colors[tri] = Vec3f(0,1,0); triangles[tri].v0 = 4; triangles[tri].v1 = 5; triangles[tri].v2 = 6; tri++;
  colors[tri] = Vec3f(0,1,0); triangles[tri].v0 = 5; triangles[tri].v1 = 7; triangles[tri].v2 = 6; tri++;

  // bottom side
  colors[tri] = Vec3f(0.5f);  triangles[tri].v0 = 0; triangles[tri].v1 = 1; triangles[tri].v2 = 4; tri++;
  colors[tri] = Vec3f(0.5f);  triangles[tri].v0 = 1; triangles[tri].v1 = 5; triangles[tri].v2 = 4; tri++;

  // top side
  colors[tri] = Vec3f(1.0f);  triangles[tri].v0 = 2; triangles[tri].v1 = 6; triangles[tri].v2 = 3; tri++;
  colors[tri] = Vec3f(1.0f);  triangles[tri].v0 = 3; triangles[tri].v1 = 6; triangles[tri].v2 = 7; tri++;

  // front side
  colors[tri] = Vec3f(0,0,1); triangles[tri].v0 = 0; triangles[tri].v1 = 4; triangles[tri].v2 = 2; tri++;
  colors[tri] = Vec3f(0,0,1); triangles[tri].v0 = 2; triangles[tri].v1 = 4; triangles[tri].v2 = 6; tri++;

  // back side
  colors[tri] = Vec3f(1,1,0); triangles[tri].v0 = 1; triangles[tri].v1 = 3; triangles[tri].v2 = 5; tri++;
  colors[tri] = Vec3f(1,1,0); triangles[tri].v0 = 3; triangles[tri].v1 = 7; triangles[tri].v2 = 5; tri++;

  rtcUnmapBuffer(scene_i,mesh,RTC_INDEX_BUFFER);

  return mesh;
}

/*! random number generator for floating point numbers in range [0,1] */
inline float frand(int& seed) {
  /*seed = 7 * seed + 5;
  seed = 13 * seed + 17;
  seed = 3 * seed + 2;
  seed = 127 * seed + 13;
  return (seed & 0xFFFF)/(float)0xFFFF;*/
  return drand48();
}

/*! Uniform hemisphere sampling. Up direction is the z direction. */
Vec3f sampleSphere(const float& u, const float& v) 
{
  const float phi = 2.0f*(float)pi * u;
  const float cosTheta = 1.0f - 2.0f * v, sinTheta = 2.0f * sqrt(v * (1.0f - v));
  return Vec3f(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

Vec3f noise(Vec3f p, float t) {
  //return div(p,length(p));
  return p + Vec3f(sin(4.0f*t),4.0f*t,cos(4.0f*t));
  //return p + Vec3f(4.0f*t,4.0f*t,0.0f);
}

Vertex* vertices = NULL;
int*    indices = NULL;

/* adds hair to the scene */
unsigned int addHair (RTCScene scene_i)
{
#if 0

  unsigned int geomID = rtcNewQuadraticBezierCurves (scene_i, RTC_GEOMETRY_STATIC, 1, 4);
  Vertex* vertices = (Vertex*) rtcMapBuffer(scene_i,geomID,RTC_VERTEX_BUFFER); 
  int*    indices  = (int*   ) rtcMapBuffer(scene_i,geomID,RTC_INDEX_BUFFER); 
  vertices[0].x = 0; vertices[0].y = -2; vertices[0].z = 0; vertices[0].r = 0.1; 
  vertices[1].x = 1; vertices[1].y = -1; vertices[1].z = 1; vertices[1].r = 0.1; 
  vertices[2].x = -1; vertices[2].y =  0; vertices[2].z = -1; vertices[2].r = 0.1; 
  vertices[3].x = 0; vertices[3].y =  1; vertices[3].z = 0; vertices[3].r = 0.0; 
  indices[0] = 0;
  rtcUnmapBuffer(scene_i,geomID,RTC_VERTEX_BUFFER); 
  rtcUnmapBuffer(scene_i,geomID,RTC_INDEX_BUFFER); 
  return geomID;

#else
  int seed = 879;
  const int numCurves = 1000;
  const int numCurveSegments = 4;
  const int numCurvePoints = 3*numCurveSegments+1;
  const float R = 0.01f;

  /* create set of bezier curves */
  unsigned int geomID = rtcNewQuadraticBezierCurves (scene_i, RTC_GEOMETRY_STATIC, numCurves*numCurveSegments, numCurves*numCurvePoints);
  vertices = new Vertex[numCurves*numCurvePoints]; //(Vertex*) rtcMapBuffer(scene_i,geomID,RTC_VERTEX_BUFFER);  // FIXME: use shared buffers
  indices  = new int   [numCurves*numCurveSegments]; //(int*   ) rtcMapBuffer(scene_i,geomID,RTC_INDEX_BUFFER); 
  rtcSetBuffer(scene_i,geomID,RTC_VERTEX_BUFFER,vertices,0,sizeof(Vertex));
  rtcSetBuffer(scene_i,geomID,RTC_INDEX_BUFFER,indices,0,sizeof(int));

  for (size_t i=0; i<numCurves; i++)
  {
    float ru = frand(seed);
    float rv = frand(seed);
    //ru = rv = 0.5f;
    //Vec3f d = sampleSphere(ru,rv);
    //Vec3f p = div(d,max(abs(d.x),max(abs(d.y),abs(d.z))));
    Vec3f p = Vec3f(-2.0f+ru*4.0f,-2.0f,-2.0f+rv*4.0f);
    for (size_t j=0; j<=numCurveSegments; j++) 
    {
      bool last = j == numCurveSegments;
      float f0 = float(2*j+0)/float(2*numCurveSegments);
      float f1 = float(2*j+1)/float(2*numCurveSegments);
      Vec3f p0 = noise(p,f0);
      Vec3f p1 = noise(p,f1);
      
      if (j>0) {
        vertices[i*numCurvePoints+3*j-1].x = 2.0f*p0.x-p1.x;
        vertices[i*numCurvePoints+3*j-1].y = 2.0f*p0.y-p1.y;
        vertices[i*numCurvePoints+3*j-1].z = 2.0f*p0.z-p1.z;
        vertices[i*numCurvePoints+3*j-1].r = last ? 0.0f : R;
      }
      
      vertices[i*numCurvePoints+3*j+0].x = p0.x;
      vertices[i*numCurvePoints+3*j+0].y = p0.y;
      vertices[i*numCurvePoints+3*j+0].z = p0.z;
      vertices[i*numCurvePoints+3*j+0].r = last ? 0.0f : R;

      if (j<numCurveSegments) {
        vertices[i*numCurvePoints+3*j+1].x = p1.x;
        vertices[i*numCurvePoints+3*j+1].y = p1.y;
        vertices[i*numCurvePoints+3*j+1].z = p1.z;
        vertices[i*numCurvePoints+3*j+1].r = R;
      }
    }

    for (size_t j=0; j<numCurveSegments; j++) {
      indices[i*numCurveSegments+j] = i*numCurvePoints+3*j;
    }
  }

  //rtcUnmapBuffer(scene_i,geomID,RTC_VERTEX_BUFFER); 
  //rtcUnmapBuffer(scene_i,geomID,RTC_INDEX_BUFFER); 

  return geomID;
#endif
}

/* adds a ground plane to the scene */
unsigned int addGroundPlane (RTCScene scene_i)
{
  /* create a triangulated plane with 2 triangles and 4 vertices */
  unsigned int mesh = rtcNewTriangleMesh (scene_i, RTC_GEOMETRY_STATIC, 2, 4);

  /* set vertices */
  Vertex* vertices = (Vertex*) rtcMapBuffer(scene_i,mesh,RTC_VERTEX_BUFFER); 
  vertices[0].x = -10; vertices[0].y = -2; vertices[0].z = -10; 
  vertices[1].x = -10; vertices[1].y = -2; vertices[1].z = +10; 
  vertices[2].x = +10; vertices[2].y = -2; vertices[2].z = -10; 
  vertices[3].x = +10; vertices[3].y = -2; vertices[3].z = +10;
  rtcUnmapBuffer(scene_i,mesh,RTC_VERTEX_BUFFER); 

  /* set triangles */
  Triangle* triangles = (Triangle*) rtcMapBuffer(scene_i,mesh,RTC_INDEX_BUFFER);
  triangles[0].v0 = 0; triangles[0].v1 = 2; triangles[0].v2 = 1;
  triangles[1].v0 = 1; triangles[1].v1 = 2; triangles[1].v2 = 3;
  rtcUnmapBuffer(scene_i,mesh,RTC_INDEX_BUFFER);

  return mesh;
}

/* called by the C++ code for initialization */
extern "C" void device_init (int8* cfg)
{
  /* initialize ray tracing core */
  rtcInit(cfg);

  /* create scene */
  g_scene = rtcNewScene(RTC_SCENE_STATIC,RTC_INTERSECT1);

  /* add cube */
  //addCube(g_scene);

  /* add hairs */
  addHair(g_scene);

  /* add ground plane */
  addGroundPlane(g_scene);

  /* commit changes to scene */
  rtcCommit (g_scene);

  /* set start render mode */
  renderPixel = renderPixelStandard;
}

__forceinline Vec3fa evalBezier(const int primID, const float t)
{
  const float t0 = 1.0f - t, t1 = t;
  
  const int i = indices[primID];
  const Vec3fa p00 = *(Vec3fa*)&vertices[i+0];
  const Vec3fa p01 = *(Vec3fa*)&vertices[i+1];
  const Vec3fa p02 = *(Vec3fa*)&vertices[i+2];
  const Vec3fa p03 = *(Vec3fa*)&vertices[i+3];

  const Vec3fa p10 = p00 * t0 + p01 * t1;
  const Vec3fa p11 = p01 * t0 + p02 * t1;
  const Vec3fa p12 = p02 * t0 + p03 * t1;
  const Vec3fa p20 = p10 * t0 + p11 * t1;
  const Vec3fa p21 = p11 * t0 + p12 * t1;
  const Vec3fa p30 = p20 * t0 + p21 * t1;
  
  return p30;
  //tangent = p21-p20;
}

float T_hair = 0.3f;

float occluded(RTCScene scene, RTCRay& shadow)
{
  float T = 1.0f;
  while (true) 
  {
    rtcIntersect(scene,shadow);
    if (shadow.geomID == RTC_INVALID_GEOMETRY_ID) break;
    if (shadow.geomID != 0) return 0.0f;
    
    /* calculate how much the curve occludes the ray */
    float sizeRay = max(shadow.org.w + shadow.tfar*shadow.dir.w, 0.00001f);
    float sizeCurve = evalBezier(shadow.primID,shadow.u).w;
    T *= 1.0f-clamp((1.0f-T_hair)*sizeCurve/sizeRay,0.0f,1.0f);

    /* continue shadow ray */
    shadow.geomID = RTC_INVALID_GEOMETRY_ID;
    shadow.tnear = 1.001f*shadow.tfar;
    shadow.tfar = inf;
  }
  return T;
}

/*! Anisotropic power cosine microfacet distribution. */
class AnisotropicPowerCosineDistribution {
public:

  __forceinline AnisotropicPowerCosineDistribution() {}

  /*! Anisotropic power cosine distribution constructor. */
  __forceinline AnisotropicPowerCosineDistribution(const Vec3fa& R, const Vec3fa& dx, float nx, const Vec3fa& dy, float ny, const Vec3fa& dz) 
    : R(R), dx(dx), nx(nx), dy(dy), ny(ny), dz(dz),
      //norm1(sqrtf((nx+1)*(ny+1)) * float(one_over_two_pi)),
      //norm2(sqrtf((nx+2)*(ny+2)) * float(one_over_two_pi)) {}
      norm1(1.0f), norm2(1.0f) {}

  /*! Evaluates the power cosine distribution. \param wh is the half
   *  vector */
  __forceinline float eval(const Vec3fa& wh) const 
  {
    const float cosPhiH   = dot(wh, dx);
    const float sinPhiH   = dot(wh, dy);
    const float cosThetaH = dot(wh, dz);
    const float R = sqr(cosPhiH)+sqr(sinPhiH);
    if (R == 0.0f) return norm2;
    const float n = (nx*sqr(cosPhiH)+ny*sqr(sinPhiH))*rcp(R);
    return norm2 * pow(abs(cosThetaH), n);
  }

  __forceinline Vec3fa reflect(const Vec3fa& I, const Vec3fa& N) const {
    return I-2.0f*dot(I,N)*N;
  }

  __forceinline Vec3fa eval(int geomID, const Vec3fa& wo, const Vec3fa& wi_) const
  {
    Vec3fa wi = wi_;
    //if (dot(wi,dz) <= 0) return zero;
    const float cosThetaO = dot(wo,dz);
    const float cosThetaI = dot(wi,dz);
    //if (cosThetaI <= 0.0f || cosThetaO <= 0.0f) return zero;
    if (cosThetaI < 0.0f) {
      wi = reflect(wi,dz);
    }
    //PRINT(cosThetaO);
    //PRINT(cosThetaI);
    const Vec3fa wh = normalize(wi + wo);
    const float cosThetaH = dot(wh, dz);
    //if (geomID == 0) PRINT(cosThetaH);
    const float cosTheta = dot(wi, wh); // = dot(wo, wh);
    const float D = eval(wh);
    //const float G = min(1.0f, 2.0f * cosThetaH * cosThetaO * rcp(cosTheta), 2.0f * cosThetaH * cosThetaI * rcp(cosTheta));
    //return R * D * G * rcp(4.0f*cosThetaO);
    const float G = dot(wi,dz);
    return R*D*G;
  }

public:
  Vec3fa R;
  Vec3fa dx;       //!< x-direction of the distribution.
  float nx;        //!< Glossiness in x direction with range [0,infinity[ where 0 is a diffuse surface.
  Vec3fa dy;       //!< y-direction of the distribution.
  float ny;        //!< Exponent that determines the glossiness in y direction.
  Vec3fa dz;       //!< z-direction of the distribution.
  float norm1;     //!< Normalization constant for calculating the pdf for sampling.
  float norm2;     //!< Normalization constant for calculating the distribution.
};

/* task that renders a single screen tile */
Vec3fa renderPixelStandard(int x, int y, const Vec3fa& vx, const Vec3fa& vy, const Vec3fa& vz, const Vec3fa& p)
{
  /* initialize ray */
  RTCRay ray;
  ray.org = p;
  ray.org.w = 0.0f;
  ray.dir = normalize(add(mul(x,vx), mul(y,vy), vz));
  Vec3fa dir1 = normalize(add(mul(x+1,vx), mul(y+1,vy), vz));
  ray.dir.w = 0.5f*0.707f*length(dir1-ray.dir);
  ray.tnear = 0.0f;
  ray.tfar = inf;
  ray.geomID = RTC_INVALID_GEOMETRY_ID;
  ray.primID = RTC_INVALID_GEOMETRY_ID;
  ray.mask = -1;
  ray.time = 0;

  Vec3fa color = Vec3f(0.0f);
  float weight = 1.0f;

  while (true)
  {
    /* intersect ray with scene */
    rtcIntersect(g_scene,ray);
  
    /* exit if we hit environment */
    if (ray.geomID == RTC_INVALID_GEOMETRY_ID) 
      return color;

    /* calculate transmissivity of hair */
    AnisotropicPowerCosineDistribution brdf;

    float Th = 0.0f;
    if (ray.geomID == 0) 
    {
      /* calculate how much the curve occludes the ray */
      float sizeRay = max(ray.org.w + ray.tfar*ray.dir.w, 0.00001f);
      float sizeCurve = evalBezier(ray.primID,ray.u).w;
      Th = 1.0f-clamp((1.0f-T_hair)*sizeCurve/sizeRay,0.0f,1.0f);

      /* calculate tangent space */
      const Vec3fa dx = normalize(ray.Ng);
      const Vec3fa dy = normalize(cross(ray.dir,dx));
      const Vec3fa dz = normalize(cross(dy,dx));

      /* generate anisotropic BRDF */
      //const Vec3fa color(0.5f,0.4f,0.4f);
      const Vec3fa color(1.0f);
      new (&brdf) AnisotropicPowerCosineDistribution(color,dx,10.0f,dy,1.0f,dz);
    }
    else 
    {
      /* calculate tangent space */
      const Vec3fa dz = normalize(ray.Ng);
      const Vec3fa dx = normalize(cross(dz,ray.dir));
      const Vec3fa dy = normalize(cross(dz,dx));
      
      /* generate isotropic BRDF */
      const Vec3fa color(1.0f,1.0f,1.0f);
      new (&brdf) AnisotropicPowerCosineDistribution(color,dx,1.0f,dy,1.0f,dz);
    }
    
    /* calculate shadows */
    //Vec3f diffuse = Vec3f(0.5f,0.4f,0.4f); //colors[ray.primID];
    //if (ray.geomID == 1) diffuse = Vec3f(1.0f,1.0f,1.0f);
    //color = add(color,mul(diffuse,0.5f));
    Vec3fa lightDir = normalize(Vec3fa(-1,-1,-1));
    Vec3fa lightIntensity = Vec3fa(1.0f);
    
    /* initialize shadow ray */
    RTCRay shadow;
    shadow.org = add(ray.org,mul(ray.tfar,ray.dir));
    shadow.org.w = ray.org.w+ray.tfar*ray.dir.w;
    shadow.dir = neg(lightDir);
    shadow.dir.w = 0.0f;
    shadow.tnear = 0.1f;
    shadow.tfar = inf;
    shadow.geomID = RTC_INVALID_GEOMETRY_ID;
    shadow.primID = RTC_INVALID_GEOMETRY_ID;
    shadow.mask = -1;
    shadow.time = 0;
    
    /* trace shadow ray */
    float T = occluded(g_scene,shadow);
    
    /* add light contribution */
    Vec3fa c = brdf.eval(ray.geomID,neg(ray.dir),neg(lightDir));
    //if (ray.geomID == 0) PRINT(c);
    //Vec3fa c = clamp(dot(neg(ray.dir),brdf.dz),0.0f,1.0f);
    color = add(color,mul(weight*(1.0f-Th),mul(c,mul(T,lightIntensity)))); //clamp(-dot(lightDir,normalize(ray.Ng)),0.0f,1.0f)));
    weight *= Th;

    /* continue ray */
    ray.geomID = RTC_INVALID_GEOMETRY_ID;
    ray.tnear = 1.001f*ray.tfar;
    ray.tfar = inf;
  }
  return color;
}

/* task that renders a single screen tile */
void renderTile(int taskIndex, int* pixels,
                     const int width,
                     const int height, 
                     const float time,
                     const Vec3f& vx, 
                     const Vec3f& vy, 
                     const Vec3f& vz, 
                     const Vec3f& p,
                     const int numTilesX, 
                     const int numTilesY)
{
  const int tileY = taskIndex / numTilesX;
  const int tileX = taskIndex - tileY * numTilesX;
  const int x0 = tileX * TILE_SIZE_X;
  const int x1 = min(x0+TILE_SIZE_X,width);
  const int y0 = tileY * TILE_SIZE_Y;
  const int y1 = min(y0+TILE_SIZE_Y,height);

  for (int y = y0; y<y1; y++) for (int x = x0; x<x1; x++)
  {
    /* calculate pixel color */
    Vec3f color = renderPixel(x,y,vx,vy,vz,p);

    /* write color to framebuffer */
    unsigned int r = (unsigned int) (255.0f * clamp(color.x,0.0f,1.0f));
    unsigned int g = (unsigned int) (255.0f * clamp(color.y,0.0f,1.0f));
    unsigned int b = (unsigned int) (255.0f * clamp(color.z,0.0f,1.0f));
    pixels[y*width+x] = (b << 16) + (g << 8) + r;
  }
}

/* called by the C++ code to render */
extern "C" void device_render (int* pixels,
                    const int width,
                    const int height,
                    const float time,
                    const Vec3f& vx, 
                    const Vec3f& vy, 
                    const Vec3f& vz, 
                    const Vec3f& p)
{
  const int numTilesX = (width +TILE_SIZE_X-1)/TILE_SIZE_X;
  const int numTilesY = (height+TILE_SIZE_Y-1)/TILE_SIZE_Y;
  launch_renderTile(numTilesX*numTilesY,pixels,width,height,time,vx,vy,vz,p,numTilesX,numTilesY); 
  rtcDebug();
}

/* called by the C++ code for cleanup */
extern "C" void device_cleanup ()
{
  rtcDeleteScene (g_scene);
  delete[] colors;
  rtcExit();
}

