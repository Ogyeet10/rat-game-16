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
  struct glyph_shape_t{
    char c;
    float v[6];
  };
  inline float clamp01(float v){return max(0.0f,min(1.0f,v));}
  inline char glyphFromDensity(float density){return charsbyopacity[min((int)(clamp01(density)*opacitylength),opacitylength-1)];}
  char glyphFromShape(const float samples[6],unsigned int covered,float fallback_density){
    if(!covered){return ' ';}
    float minv=1.0f,maxv=0.0f,sum=0.0f;
    for(unsigned int i=0;i<6;i++){
      minv=min(minv,samples[i]);
      maxv=max(maxv,samples[i]);
      sum+=samples[i];
    }
    if(maxv<0.04f){return glyphFromDensity(fallback_density);}
    if(covered>=5){return glyphFromDensity(fallback_density);}
    if((covered==6)&&((maxv-minv)<0.09f)){return glyphFromDensity(fallback_density);}
    float shaped[6];
    for(unsigned int i=0;i<6;i++){
      float normalized=samples[i]/maxv;
      shaped[i]=powf(normalized,1.65f)*maxv;
    }
    static const glyph_shape_t glyphs[] = {
      {' ',{0.00f,0.00f,0.00f,0.00f,0.00f,0.00f}},
      {'.',{0.00f,0.00f,0.00f,0.00f,0.12f,0.12f}},
      {',',{0.00f,0.00f,0.00f,0.00f,0.26f,0.10f}},
      {96 ,{0.32f,0.06f,0.00f,0.00f,0.00f,0.00f}},
      {39 ,{0.06f,0.32f,0.00f,0.00f,0.00f,0.00f}},
      {34 ,{0.34f,0.34f,0.00f,0.00f,0.00f,0.00f}},
      {':',{0.28f,0.28f,0.00f,0.00f,0.28f,0.28f}},
      {';',{0.26f,0.26f,0.00f,0.00f,0.34f,0.12f}},
      {'-',{0.00f,0.00f,0.48f,0.48f,0.00f,0.00f}},
      {'_',{0.00f,0.00f,0.00f,0.00f,0.62f,0.62f}},
      {'/',{0.02f,0.74f,0.28f,0.50f,0.74f,0.02f}},
      {92 ,{0.74f,0.02f,0.50f,0.28f,0.02f,0.74f}},
      {'|',{0.34f,0.34f,0.66f,0.66f,0.34f,0.34f}},
      {'L',{0.48f,0.04f,0.62f,0.05f,0.82f,0.82f}},
      {'J',{0.04f,0.48f,0.05f,0.62f,0.82f,0.82f}},
      {'7',{0.82f,0.82f,0.05f,0.58f,0.04f,0.34f}},
      {'T',{0.86f,0.86f,0.42f,0.42f,0.18f,0.18f}},
      {'Y',{0.70f,0.70f,0.24f,0.24f,0.44f,0.44f}},
      {'F',{0.82f,0.82f,0.64f,0.10f,0.44f,0.04f}},
      {'P',{0.82f,0.80f,0.74f,0.52f,0.45f,0.04f}},
      {'+',{0.22f,0.22f,0.58f,0.58f,0.22f,0.22f}},
      {'*',{0.36f,0.36f,0.55f,0.55f,0.36f,0.36f}},
      {'O',{0.60f,0.60f,0.72f,0.72f,0.60f,0.60f}},
      {'U',{0.50f,0.50f,0.58f,0.58f,0.76f,0.76f}},
      {'K',{0.70f,0.28f,0.72f,0.62f,0.70f,0.28f}},
      {'E',{0.76f,0.76f,0.68f,0.28f,0.76f,0.76f}},
      {'M',{0.90f,0.90f,0.80f,0.80f,0.72f,0.72f}},
      {'@',{0.96f,0.96f,0.92f,0.92f,0.88f,0.88f}},
      {'$',{1.00f,1.00f,0.95f,0.95f,0.92f,0.92f}}
    };
    float avg=sum/6.0f;
    char best=glyphFromDensity(fallback_density);
    float best_dist=9999.0f;
    for(unsigned int i=0;i<sizeof(glyphs)/sizeof(glyphs[0]);i++){
      float glyph_avg=0.0f,dist=0.0f;
      for(unsigned int j=0;j<6;j++){glyph_avg+=glyphs[i].v[j];}
      glyph_avg/=6.0f;
      for(unsigned int j=0;j<6;j++){
        float diff=shaped[j]-glyphs[i].v[j];
        dist+=diff*diff;
      }
      float avg_diff=avg-glyph_avg;
      dist+=avg_diff*avg_diff*0.65f;
      if(dist<best_dist){best_dist=dist;best=glyphs[i].c;}
    }
    return best;
  }
  static const unsigned int shape_sample_count=6;
  static const float shape_sub_x[shape_sample_count]={0.28f,0.72f,0.28f,0.72f,0.28f,0.72f};
  static const float shape_sub_y[shape_sample_count]={0.78f,0.78f,0.50f,0.50f,0.22f,0.22f};
  float* shape_depth_buffer=NULL;
  float* shape_ink_buffer=NULL;
  color_t* shape_color_buffer=NULL;
  scoord shape_buffer_chars=0;
  bool shape_frame_active=false;
  color_t colorFromRgb(unsigned char r,unsigned char g,unsigned char b,float close_weight,float shade){
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
    return colors::col((colors::color)(hue|(bright<<3)),colors::black);
  }
  void ensureShapeBuffers(){
    if(shape_depth_buffer&&shape_ink_buffer&&shape_color_buffer&&(shape_buffer_chars==max_chars)){return;}
    shape_buffer_chars=max_chars;
    size_t sample_count=(size_t)max_chars*shape_sample_count;
    shape_depth_buffer=(float*)realloc(shape_depth_buffer,sizeof(float)*sample_count);
    shape_ink_buffer=(float*)realloc(shape_ink_buffer,sizeof(float)*sample_count);
    shape_color_buffer=(color_t*)realloc(shape_color_buffer,sizeof(color_t)*sample_count);
    if(!shape_depth_buffer||!shape_ink_buffer||!shape_color_buffer){perror("couldn't allocate shape sample buffers");exit(1);}
  }
  void beginShapeFrame(){
    ensureShapeBuffers();
    size_t sample_count=(size_t)max_chars*shape_sample_count;
    for(size_t i=0;i<sample_count;i++){
      shape_depth_buffer[i]=1.0f;
      shape_ink_buffer[i]=0.0f;
      shape_color_buffer[i]=default_color;
    }
    shape_frame_active=true;
  }
  void resolveShapeFrame(){
    if(!shape_frame_active){return;}
    for(scoord i=0;i<max_chars;i++){
      float samples[shape_sample_count]{};
      unsigned int covered=0;
      float density_sum=0.0f,min_depth=1.0f,best_ink=-1.0f;
      color_t best_color=default_color;
      for(unsigned int sample=0;sample<shape_sample_count;sample++){
        size_t sample_i=(size_t)i*shape_sample_count+sample;
        if(shape_depth_buffer[sample_i]>=1.0f){continue;}
        float ink=shape_ink_buffer[sample_i];
        samples[sample]=ink;
        covered++;
        density_sum+=1.0f-ink;
        min_depth=min(min_depth,shape_depth_buffer[sample_i]);
        if(ink>best_ink){best_ink=ink;best_color=shape_color_buffer[sample_i];}
      }
      if(!covered){continue;}
      depth_buffer[i]=min_depth;
      term_buffer[i]=glyphFromShape(samples,covered,density_sum/covered);
      color_buffer[i]=best_color;
    }
    shape_frame_active=false;
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
    float shade=max(0.15f,min(1.0f,light));
    if(!shape_frame_active){beginShapeFrame();}
    for(scoord x=minx;x<maxx;x++){
      for(scoord y=miny;y<maxy;y++){
        scoord sy=gui::term_dims.ws_row-1-y;
        scoord si=toSSPI(x,sy);
        for(unsigned int sample=0;sample<shape_sample_count;sample++){
          float sx=x+shape_sub_x[sample],sy1=y+shape_sub_y[sample];
          vec3<float> barycentric;
          barycentric.x=triarea(sx,sy1,x1,y1,x2,y2);
          barycentric.y=triarea(x0,y0,sx,sy1,x2,y2);
          barycentric.z=triarea(x0,y0,x1,y1,sx,sy1);
          if(!(((area>0)&&(barycentric.x>=-0.001f)&&(barycentric.y>=-0.001f)&&(barycentric.z>=-0.001f))||
             ((area<0)&&(barycentric.x<=0.001f)&&(barycentric.y<=0.001f)&&(barycentric.z<=0.001f)))){continue;}
          barycentric=barycentric/area;
          float inv_depth=barycentric.x*iz0+barycentric.y*iz1+barycentric.z*iz2;
          if(inv_depth<=0){continue;}
          float depth=1.0f/inv_depth;
          if((depth<=0)||(depth>=farplanex)){continue;}
          float d=(depth/farplanex);
          size_t sample_i=(size_t)si*shape_sample_count+sample;
          if(shape_depth_buffer[sample_i]<=d){continue;}
          float density_depth=max(0.0f,min(1.0f,depth/(farplanex*1.18f)));
          float close_weight=max(0.0f,min(1.0f,1.0f-density_depth*1.35f));
          float angle_cue=(depth_span>0.1f)?(((depth-near_depth)/depth_span)-0.5f)*0.30f*close_weight:0.0f;
          float density=max(0.0f,min(1.0f,density_depth+(1.0f-shade)*0.18f+angle_cue));
          float u=(uiz0*barycentric.x+uiz1*barycentric.y+uiz2*barycentric.z)/inv_depth;
          float v=(viz0*barycentric.x+viz1*barycentric.y+viz2*barycentric.z)/inv_depth;
          u*=tex.width;
          v*=tex.height;
          int iu=(((int)u%tex.width+tex.width)%tex.width);
          int iv=tex.height-1-(((int)v%tex.height+tex.height)%tex.height);
          int idx=(iv*tex.width+iu)*3;
          shape_depth_buffer[sample_i]=d;
          shape_ink_buffer[sample_i]=1.0f-density;
          shape_color_buffer[sample_i]=colorFromRgb(tex.pixels[idx],tex.pixels[idx+1],tex.pixels[idx+2],close_weight,shade);
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
