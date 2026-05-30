#ifndef RATS3D_H
#define RATS3D_H
#include <type_traits>
#include <cstdlib>
#include <r@@2e.hpp>
#include <cmath>
#include <vector>
#define SCAST(t,v) static_cast<t>(v)
#define MESHTRI_OUTLN_01 0b00000001
#define MESHTRI_OUTLN_12 0b00000010
#define MESHTRI_OUTLN_20 0b00000100
template<arith T> inline auto constexpr triarea(T x0,T y0,T x1,T y1,T x2,T y2){
  if constexpr(std::is_integral_v<T>){
    using sT=std::make_signed_t<T>;
    return ((x0 * ((sT)y1-y2)) + (x1 * ((sT)y2-y0)) + (x2 * ((sT)y0-y1)));//these probably shouldn't be negative
  }else{return ((x0 * (y1-y2)) + (x1 * (y2-y0)) + (x2 * (y0-y1)));}//but it still works so
}
namespace mesh {
  float nearplanex=0.25f;
  float farplanex=12.0f;
  float fov=90.0f;
  const char* charsbyopacity="$@MN%&E0KUO*+;:,.";
  int opacitylength=17;
  inline float fovScale(){return 1.0f/tanf((fov/2.0f)*(float)M_PI/180.0f);}
  template<arith T,arith U> constexpr void rotate(T& axis_0,T& axis_1,U d){
    float r1=cos(d/128.0*M_PI),r2=sin(d/128.0*M_PI);
    float axis_0_t=(axis_0*r1)-(axis_1*r2);
    axis_1=axis_1*r1+axis_0*r2;
    axis_0=axis_0_t;
  }
  template<typename T,arith U> constexpr inline tri3<T> rotateT(tri3<T>& v,U d){
    rotate(v.a.x,v.a.y,d);rotate(v.b.x,v.b.y,d);rotate(v.c.x,v.c.y,d);
    return v;
  }
  template<typename T> requires std::is_signed_v<T> vec3<T>* clipTriX(const tri3<T>& t,T x){//012,230
    vec3<T>* out=(vec3<T>*)malloc(sizeof(vec3<T>)*4);
    #define p0 t.a
    #define p1 t.b
    #define p2 t.c
    #define o0 out[0]
    #define o1 out[1]
    #define o2 out[2]
    #define o3 out[3]
    o3={0,0,0};
    char v=(p0.x>=x)+(p1.x>=x)+(p2.x>=x);
    switch(v){
      case 0:break;
      case 1:{
        if(p0.x>=x){
          o0=p0;
          o1={
            x,
            (p0.y-p1.y)/(p0.x-p1.x)*(x-p1.x)+p1.y,
            (p0.z-p1.z)/(p0.x-p1.x)*(x-p1.x)+p1.z
          };
          o2={
            x,
            (p0.y-p2.y)/(p0.x-p2.x)*(x-p2.x)+p2.y,
            (p0.z-p2.z)/(p0.x-p2.x)*(x-p2.x)+p2.z
          };
        }else if(p1.x>=x){
          o0={
            x,
            (p1.y-p0.y)/(p1.x-p0.x)*(x-p0.x)+p0.y,
            (p1.z-p0.z)/(p1.x-p0.x)*(x-p0.x)+p0.z
          };
          o1=p1;
          o2={
            x,
            (p1.y-p2.y)/(p1.x-p2.x)*(x-p2.x)+p2.y,
            (p1.z-p2.z)/(p1.x-p2.x)*(x-p2.x)+p2.z
          };
        }else{
          o0={
            x,
            (p2.y-p0.y)/(p2.x-p0.x)*(x-p0.x)+p0.y,
            (p2.z-p0.z)/(p2.x-p0.x)*(x-p0.x)+p0.z
          };
          o1={
            x,
            (p2.y-p1.y)/(p2.x-p1.x)*(x-p1.x)+p1.y,
            (p2.z-p1.z)/(p2.x-p1.x)*(x-p1.x)+p1.z
          };
          o2=p2;
        }
        break;
      }
      case 2:{
        if(p0.x<x){
          o0={
            x,
            (p1.y-p0.y)/(p1.x-p0.x)*(x-p0.x)+p0.y,
            (p1.z-p0.z)/(p1.x-p0.x)*(x-p0.x)+p0.z
          };
          o1=p1;
          o2=p2;
          o3={
            x,
            (p0.y-p2.y)/(p0.x-p2.x)*(x-p2.x)+p2.y,
            (p0.z-p2.z)/(p0.x-p2.x)*(x-p2.x)+p2.z
          };
        }else if(p1.x<x){
          o0=p0;
          o1={
            x,
            (p0.y-p1.y)/(p0.x-p1.x)*(x-p1.x)+p1.y,
            (p0.z-p1.z)/(p0.x-p1.x)*(x-p1.x)+p1.z
          };
          o2={
            x,
            (p1.y-p2.y)/(p1.x-p2.x)*(x-p2.x)+p2.y,
            (p1.z-p2.z)/(p1.x-p2.x)*(x-p2.x)+p2.z
          };
          o3=p2;
        }else{
          o0=p0;
          o1=p1;
          o2={
            x,
            (p1.y-p2.y)/(p1.x-p2.x)*(x-p2.x)+p2.y,
            (p1.z-p2.z)/(p1.x-p2.x)*(x-p2.x)+p2.z
          };
          o3={
            x,
            (p0.y-p2.y)/(p0.x-p2.x)*(x-p2.x)+p2.y,
            (p0.z-p2.z)/(p0.x-p2.x)*(x-p2.x)+p2.z
          };
        }
        break;
      }
      case 3:{
        o0=t.a;o1=t.b;o2=t.c;
      }
    }
    return out;
    #undef p0
    #undef p1
    #undef p2
    #undef o0
    #undef o1
    #undef o2
    #undef o3
  }
  struct textured_vertex{
    vec3<mesh_size> p;
    vec2<float> uv;
  };
  textured_vertex lerpTexturedVertex(const textured_vertex& a,const textured_vertex& b,float t){
    return {
      {
        a.p.x+(b.p.x-a.p.x)*t,
        a.p.y+(b.p.y-a.p.y)*t,
        a.p.z+(b.p.z-a.p.z)*t,
      },{
        a.uv.x+(b.uv.x-a.uv.x)*t,
        a.uv.y+(b.uv.y-a.uv.y)*t,
      }
    };
  }
  template<typename F> void clipTexturedPoly(std::vector<textured_vertex>& poly,F plane){
    if(poly.empty()){return;}
    std::vector<textured_vertex> in=poly;
    poly.clear();
    textured_vertex prev=in.back();
    float prev_d=plane(prev);
    for(textured_vertex cur:in){
      float cur_d=plane(cur);
      if(cur_d>=0){
        if(prev_d<0){poly.push_back(lerpTexturedVertex(prev,cur,prev_d/(prev_d-cur_d)));}
        poly.push_back(cur);
      }else if(prev_d>=0){
        poly.push_back(lerpTexturedVertex(prev,cur,prev_d/(prev_d-cur_d)));
      }
      prev=cur;
      prev_d=cur_d;
    }
  }
  void clipTexturedTriToFrustum(std::vector<textured_vertex>& poly){
    float edge_scale=1.0f/fovScale();
    clipTexturedPoly(poly,[](const textured_vertex& v){return v.p.x-nearplanex;});
    clipTexturedPoly(poly,[](const textured_vertex& v){return (mesh_size)farplanex-v.p.x;});
    clipTexturedPoly(poly,[edge_scale](const textured_vertex& v){return v.p.y+v.p.x*edge_scale;});
    clipTexturedPoly(poly,[edge_scale](const textured_vertex& v){return v.p.x*edge_scale-v.p.y;});
    clipTexturedPoly(poly,[edge_scale](const textured_vertex& v){return v.p.z+v.p.x*edge_scale;});
    clipTexturedPoly(poly,[edge_scale](const textured_vertex& v){return v.p.x*edge_scale-v.p.z;});
  }
  vec3<mesh_size> camera_position{-5.0f,0.0f,0.0f};
  vec3<int>       camera_rotation{0,0,0};//roll pitch yaw
}
namespace gui {
  using namespace mesh;
  inline vec2<scoord> toSSPV(vec3<mesh_size> v){
    float scale=fovScale();
    return (vec2<scoord>){(scoord)((v.y/v.x*scale+1.0f)*term_dims.ws_col/2.0f),(scoord)((v.z/v.x*scale+1.0f)*term_dims.ws_row/2.0f)};
  }
  inline tri2<scoord> toSSPT(tri3<mesh_size> t){
    return (tri2<scoord>){toSSPV(t.a),toSSPV(t.b),toSSPV(t.c)};
  }
  void drawTri(const tri3<mesh_size>& t1, vec2<float> uv0, vec2<float> uv1, vec2<float> uv2, assets::texture_t& tex, float light){
    mesh_size z0=t1.a.x,z1=t1.b.x,z2=t1.c.x;
    printd("triangle((%f,%f,%f),(%f,%f,%f),(%f,%f,%f));polygon((%f,%f),(%f,%f),(%f,%f))",t1.a.x,t1.a.y,t1.a.z,t1.b.x,t1.b.y,t1.b.z,t1.c.x,t1.c.y,t1.c.z,uv0.x,uv0.y,uv1.x,uv1.y,uv2.x,uv2.y);
    float scale=fovScale();
    float
      x0=(t1.a.y/z0*scale+1.0f)*gui::term_dims.ws_col/2.0f,y0=(t1.a.z/z0*scale+1.0f)*gui::term_dims.ws_row/2.0f,
      x1=(t1.b.y/z1*scale+1.0f)*gui::term_dims.ws_col/2.0f,y1=(t1.b.z/z1*scale+1.0f)*gui::term_dims.ws_row/2.0f,
      x2=(t1.c.y/z2*scale+1.0f)*gui::term_dims.ws_col/2.0f,y2=(t1.c.z/z2*scale+1.0f)*gui::term_dims.ws_row/2.0f;
    scoord minx=max((int)floor(min(x0,x1,x2)),0),
           miny=max((int)floor(min(y0,y1,y2)),0),
           maxx=min((int)ceil(max(x0,x1,x2)),gui::term_dims.ws_col),
           maxy=min((int)ceil(max(y0,y1,y2)),gui::term_dims.ws_row);
    float area=triarea(
      SCAST(float,x0),SCAST(float,y0),
      SCAST(float,x1),SCAST(float,y1),
      SCAST(float,x2),SCAST(float,y2));
    if(fabs(area)<0.1){return;}
    float iz0=1.0f/z0,iz1=1.0f/z1,iz2=1.0f/z2;
    float uiz0=uv0.x*iz0,uiz1=uv1.x*iz1,uiz2=uv2.x*iz2;
    float viz0=uv0.y*iz0,viz1=uv1.y*iz1,viz2=uv2.y*iz2;
    float near_depth=min(z0,z1,z2),depth_span=max(z0,z1,z2)-near_depth;
    for(scoord x=minx;x<maxx;x++){
      for(scoord y=miny;y<maxy;y++){
        scoord sy=gui::term_dims.ws_row-1-y;
        scoord si=toSSPI(x,sy);
        vec3<float> barycentric;
        barycentric.x=triarea(
          x+0.5f, y+0.5f,
          x1,y1,
          x2,y2
        );
        barycentric.y=triarea(
          x0,y0,
          x+0.5f, y+0.5f,
          x2,y2
        );
        barycentric.z=triarea(
          x0,y0,
          x1,y1,
          x+0.5f, y+0.5f
        );
        if(((area>0)&&(barycentric.x>=-0.001f)&&(barycentric.y>=-0.001f)&&(barycentric.z>=-0.001f))||
           ((area<0)&&(barycentric.x<=0.001f)&&(barycentric.y<=0.001f)&&(barycentric.z<=0.001f))){
          barycentric=barycentric/area;
          float inv_depth=barycentric.x*iz0+barycentric.y*iz1+barycentric.z*iz2;
          if(inv_depth<=0){continue;}
          float depth=1.0f/inv_depth;
          float d=(depth/farplanex);
          if((depth_buffer[si]) > d){
            depth_buffer[si]=d;
            if((0<depth)&&(depth<farplanex)){
              float u=(uiz0*barycentric.x+uiz1*barycentric.y+uiz2*barycentric.z)/inv_depth;
              float v=(viz0*barycentric.x+viz1*barycentric.y+viz2*barycentric.z)/inv_depth;
              u*=tex.width; 
              v*=tex.height;
              int iu=(((int)u%tex.width+tex.width)%tex.width);
              int iv=tex.height-1-(((int)v%tex.height+tex.height)%tex.height);
              int idx=(iv*tex.width+iu)*3;
              float shade=max(0.15f,min(1.0f,light));
              unsigned char r=tex.pixels[idx],g=tex.pixels[idx+1],b=tex.pixels[idx+2];
              float density_depth=max(0.0f,min(1.0f,depth/(farplanex*1.18f)));
              float close_weight=max(0.0f,min(1.0f,1.0f-density_depth*1.35f));
              float rf=r,gf=g,bf=b;
              float maxc=max(rf,gf,bf),minc=min(rf,gf,bf);
              char hue=0;
              if(maxc>12.0f){
                if((maxc-minc)<18.0f){hue=7;}
                else{
                  float cutoff=maxc*0.58f;
                  hue=(rf>=cutoff)|((gf>=cutoff)<<1)|((bf>=cutoff)<<2);
                }
              }
              float texture_brightness=(0.2126f*rf+0.7152f*gf+0.0722f*bf)/255.0f;
              float lit_brightness=texture_brightness+close_weight*0.35f+(shade-0.5f)*0.35f;
              if((hue==0)&&(maxc>4.0f)){hue=7;}
              bool bright=(hue!=0)&&(lit_brightness>0.38f);
              char colorIdx = hue|(bright<<3);
              float angle_cue=(depth_span>0.1f)?(((depth-near_depth)/depth_span)-0.5f)*0.30f*close_weight:0.0f;
              float density=max(0.0f,min(1.0f,density_depth+(1.0f-shade)*0.18f+angle_cue));
              char c = charsbyopacity[min((int)(density*opacitylength),opacitylength-1)];
              putChar(x,sy,c);
              putColor(x,sy,colors::col((colors::color)colorIdx,colors::black));
            }
            // if(logmisc){
              // fprintf(debug,"(%u,%u,%f),",x,y,depth);
              // fprintf(debug,"(%u,%u,%u),",x,y,depth_buffer[toSSPI(x,y)]);
            // }
          }
        }
      }
    }
    // fflush(debug);
  }
  void drawMTri(const meshtri& t, assets::texture_t& tex){
    meshtri t1=t-camera_position;
    rotateT(t1,camera_rotation.z);
    vec3<mesh_size> n=(t1.c-t1.a).cross(t1.b-t1.a);
    if((n*t1.a).total()>=0){return;}
    float normal_length=n.magnitude();
    vec3<float> light_dir{-0.45f,-0.35f,-0.82f};
    float light=normal_length>0.0f?0.55f+0.45f*((n*light_dir).total()/normal_length):1.0f;
    std::vector<textured_vertex> poly{{t1.a,t1.uv0},{t1.b,t1.uv1},{t1.c,t1.uv2}};
    clipTexturedTriToFrustum(poly);
    if(poly.size()<3){return;}
    for(size_t i=1;i+1<poly.size();i++){
      drawTri({poly[0].p,poly[i].p,poly[i+1].p},poly[0].uv,poly[i].uv,poly[i+1].uv,tex,light);
    }
  }
}
#endif
