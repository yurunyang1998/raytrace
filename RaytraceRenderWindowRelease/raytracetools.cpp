#include "raytracetools.h"

#include "fakegl.h"

#include <random>
std:: default_random_engine generator;
std::uniform_real_distribution<double> distr(0.0,1.0);
double erand48(int X){
    return distr(generator);
}


//bool triangle::hit(Ray r, float min_t, float max_t, hit_record &rec){

//    auto P0 = facePosition[0].position;
//    auto P1 = facePosition[1].position;
//    auto P2 = facePosition[2].position;

//    auto E1 = P1-P0;
//    auto E2 = P2-P0;
//    auto S  = r.origin()-P0;
//    auto S1 = r.direction().cross(E2);
//    auto S2 = S.cross(E1);

//    float beta = 1/S1.dot(E1);

//    float t = beta*(S2.dot(E2));
//    float b1 = beta*(S1.dot(S));
//    float b2 = beta*(S2.dot(r.direction()));

//    if(b1>=0 && b1<=1 && b2>=0 && b2<=1 && (1-b1-b2)>=0 && (1-b1-b2)<=1){ // hitted
//        if(t<min_t || t > max_t)   //the hit is not the closest point
//            return false;
//        rec.hitPoint = r.at(t);
//        rec.t = t;
//        rec.objptr = this;
//        return true;
//    }else // not histted
//        return false;
//}


bool hitList::hit(Ray r, float min_t, float max_t, hit_record &rec){

    bool hittedAnything = false;
    for(auto obj : this->objects){
        if(obj->hit(r, min_t, max_t, rec)){
           max_t = rec.t; // max_t means the closest obj so far
           hittedAnything = true;

        }
    }
    return hittedAnything;

}

Cartesian3 RayTrace::radiance(Ray &r, int depth, hitList &world, int Xi){

    hit_record rec;
    if(!world.hit(r,0,INT_MAX,rec)){
        return Cartesian3();
    }
    auto hittedObj = rec.objptr;
    if(++depth>1){
        //TODO::russian roulette
        return hittedObj->emitte;
    }
    Cartesian3 n,nl, color; //n means normal, nl means to inside or outside
    Cartesian3 hitPoint = rec.hitPoint;
    if(hittedObj->objType==0) {        // triangle
      n = hittedObj->normal;
      nl = hittedObj->normal.dot(r.direction())?hittedObj->normal:hittedObj->normal*-1;
    }
    else if(hittedObj->objType==1){ // sphere
        Sphere  * sphereobj = dynamic_cast<Sphere*>(hittedObj);
        Cartesian3 x=r.origin()+r.direction()*rec.t;
        n=(x-sphereobj->position).unit();
        nl=n.dot(r.direction())<0?n:n*-1;
        color = sphereobj->color;
    }

    if(hittedObj->reflectType==DIFF){

        double r1 = 2*M_PI*erand48(1);
        double r2 = erand48(1);
        double r2s = sqrt(r2);
        Cartesian3 w=nl;
        Cartesian3 u=((fabs(w.x)>.1?Cartesian3(0,1,0):Cartesian3(1,-w.z,w.y))).unit();
        Cartesian3 v = Cartesian3(w.y*u.z-w.z*u.y, w.z*u.x-w.x*u.z, w.x*u.y-w.y*u.x);
        Cartesian3 d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).unit();
        Ray outputRay(rec.hitPoint, d);
        Cartesian3 a;
        auto pixelColor = radiance(outputRay, depth, world, Xi);
        return  hittedObj->emitte+ color.mult(pixelColor);

    }
    else if(hittedObj->reflectType == SPEC){

        Ray specularReflectRay(hitPoint, r.direction()-n*2*n.dot(r.direction()));
        auto pixelColor = radiance(specularReflectRay, depth, world, Xi);
        return hittedObj->emitte + color.mult(pixelColor);

    }
    Ray reflRay(hitPoint, r.direction()-n*2*n.dot(r.direction()));
    bool into = n.dot(nl)>0;
    double nc=1 ,nt =1.5, nnt = into?nc/nt:nt/nc, ddn=r.direction().dot(nl), cos2t;
    if((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)
        return hittedObj->emitte + color.mult(radiance(reflRay, depth,world, Xi));
    Cartesian3 tdir = (r.direction()*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).unit();
       double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
       double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
       auto tempRay = Ray(hitPoint,tdir);
       return hittedObj->emitte + color.mult(depth>2 ? (erand48(1)<P ?   // Russian roulette
         radiance(reflRay,depth, world,Xi)*RP:radiance(tempRay,depth, world, Xi)*TP) :
         radiance(reflRay,depth, world,Xi)*Re+radiance(tempRay,depth, world, Xi)*Tr);

}

//struct Sphere {
//  double rad;       // radius
//  Cartesian3 p, e, c;      // position, emission, color
//  Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive)
//  Sphere(double rad_, Cartesian3 p_, Cartesian3 e_, Cartesian3 c_, Refl_t refl_):
//    rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}

//  double intersect(Ray &r) const { // returns distance, 0 if nohit
//    Cartesian3 op = p-r.origin() ; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
//    double t, eps=1e-4, b=op.dot(r.direction()), det=b*b-op.dot(op)+rad*rad;
//    if (det<0) return 0; else det=sqrt(det);
//    return (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
//  }
//};


Sphere spheres[] = {//Scene: radius, position, emission, color, material
   Sphere(1e5, Cartesian3( 1e5+1,40.8,81.6), Cartesian3(),Cartesian3(.75,.25,.25),DIFF),//Left
   Sphere(1e5, Cartesian3(-1e5+99,40.8,81.6),Cartesian3(),Cartesian3(.25,.25,.75),DIFF),//Rght
   Sphere(1e5, Cartesian3(50,40.8, 1e5),     Cartesian3(),Cartesian3(.75,.75,.75),DIFF),//Back
   Sphere(1e5, Cartesian3(50,40.8,-1e5+170), Cartesian3(),Cartesian3(),           DIFF),//Frnt
   Sphere(1e5, Cartesian3(50, 1e5, 81.6),    Cartesian3(),Cartesian3(.75,.75,.75),DIFF),//Botm
   Sphere(1e5, Cartesian3(50,-1e5+81.6,81.6),Cartesian3(),Cartesian3(.75,.75,.75),DIFF),//Top
   Sphere(16.5,Cartesian3(27,16.5,47),       Cartesian3(),Cartesian3(1,1,1)*.999, SPEC),//Mirr
   Sphere(16.5,Cartesian3(73,16.5,78),       Cartesian3(),Cartesian3(1,1,1)*.999, REFR),//Glas
   Sphere(600, Cartesian3(50,681.6-.27,81.6),Cartesian3(12,12,12),  Cartesian3(23,34,32), DIFF) //Lite
 };



//bool intersect(Ray &r, double &t, int &id){
//   double n=sizeof(spheres)/sizeof(Sphere), d, inf=t=1e20;
//   for(int i=int(n);i--;) if((d=spheres[i].intersect(r))&&d<t){t=d;id=i;}
//   return t<inf;
// }

//Cartesian3 radiance(Ray &r, int depth, int Xi){
//   double t;                               // distance to intersection
//   int id=0;                               // id of intersected object
//   if (!intersect(r, t, id)) return Cartesian3(); // if miss, return black
//   const Sphere &obj = spheres[id];        // the hit object
//   Cartesian3 x=r.origin()+r.direction()*t, n=(x-obj.p).unit(), nl=n.dot(r.direction())<0?n:n*-1, f=obj.c;
//   double p = f.x>f.y && f.x>f.z ? f.x : f.y>f.z ? f.y : f.z; // max refl
//   if (++depth>5)  return obj.e; //R.R.
//   if (obj.refl == DIFF){                  // Ideal DIFFUSE reflection
//     double r1=2*M_PI*erand48(Xi), r2=erand48(Xi), r2s=sqrt(r2);
//     Cartesian3 w=nl, u=((fabs(w.x)>.1?Cartesian3(0,1,0):Cartesian3(1,0,0))%w).unit(), v=w%u;
//     Cartesian3 d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).unit();
//     Ray tempr(x,d);
//     return obj.e + f.mult(radiance(tempr,depth,Xi));
//   } else if (obj.refl == SPEC)            // Ideal SPECULAR reflection
//     {
//       Ray tempr(x,r.direction()-n*2*n.dot(r.direction()));
//       return obj.e + f.mult(radiance(tempr,depth,Xi));
//    }
//   Ray reflRay(x, r.direction()-n*2*n.dot(r.direction()));     // Ideal dielectric REFRACTION
//   bool into = n.dot(nl)>0;                // Ray from outside going in?
//   double nc=1, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=r.direction().dot(nl), cos2t;
//   if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)    // Total internal reflection
//     return obj.e + f.mult(radiance(reflRay,depth,Xi));
//   Cartesian3 tdir = (r.direction()*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).unit();
//   double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
//   double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
//   Ray tempr(x,tdir);
//   return obj.e + f.mult(depth>2 ? (erand48(Xi)<P ?   // Russian roulette
//     radiance(reflRay,depth,Xi)*RP:radiance(tempr,depth,Xi)*TP) :
//     radiance(reflRay,depth,Xi)*Re+radiance(tempr,depth,Xi)*Tr);
// }

inline double clamp(double x){ return x<0 ? 0 : x>1 ? 1 : x; }

inline int toInt(double x){ return int(pow(clamp(x),1/2.2)*255+.5); }

int RayTrace::run(){
//    auto viewport_height = fakegl->frameBuffer.height;
//    auto viewport_width = fakegl->frameBuffer.width;
//    auto focal_length = -1.0;

    int image_width = fakegl->frameBuffer.width;
    int image_height = fakegl->frameBuffer.height;

//    auto origin = Cartesian3(0, 0, 0);
//    auto horizontal = Cartesian3(viewport_width, 0, 0);
//    auto vertical = Cartesian3(0, viewport_height, 0);
//    auto lower_left_corner = origin - horizontal/2 - vertical/2 - Cartesian3(0, 0, focal_length);

    hitList world;


    for(int i=0;i<9;i++){
        world.add(&spheres[i]);
    }


//    std::Cartesian3tor<std::Cartesian3tor<screenVertexWithAttributes>> meshLish = this->fakegl->meshListInworldCS;
//    for(int i=0;i<meshLish.size();i++){
//       triangle tri(this->fakegl->meshListInworldCS[i]);
//       world.add(tri);

//    }
    std::cout<<"render"<<std::endl;
    int samps = 8;
    int w = image_width, h = image_height;
//    int w=1024, h=768; // # samples
    Ray cam(Cartesian3(50,52,295.6), Cartesian3(0,-0.042612,-1).unit()); // cam pos, dir
     Cartesian3 cx=Cartesian3(w*.5135/h, 0, 0), cy=(cx%cam.direction()).unit()*.5135, r,*c=new Cartesian3[w*h];
     #pragma omp parallel for schedule(dynamic, 1) private(r)       // OpenMP
     for (int y=0; y<h; y++){                       // Loop over image rows
          fprintf(stderr,"\rRendering (%d spp) %5.2f%%",samps*4,100.*y/(h-1));
          for (int x=0; x<w; x++){  // Loop cols
            for (int sy=0, i=(h-y-1)*w+x; sy<2; sy++)     // 2x2 subpixel rows
              for (int sx=0; sx<2; sx++, r=Cartesian3()){        // 2x2 subpixel cols
                for (int s=0; s<samps; s++){
                  double r1=2*erand48(1), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
                  double r2=2*erand48(1), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
                  Cartesian3 d = cx*( ( (sx+.5 + dx)/2 + x)/w - .5) +
                          cy*( ( (sy+.5 + dy)/2 + y)/h - .5) + cam.direction();
                  Ray tempr(cam.origin()+d*140,d.unit());
                  r = r + radiance(tempr,0,world, 1)*(1./samps);
//                  std::cout<<x<<" "<<y<<"  "<<sy<<" "<<sx<<" "<<s<<std::endl;
                } // Camera rays are pushed ^^^^^ forward to start in interior
                c[i] = c[i] + Cartesian3(clamp(r.x),clamp(r.y),clamp(r.z))*.25;
                fakegl->frameBuffer[y][x] = RGBAValue(c[i].x*255, c[i].y*255, c[i].z*255);
//                fakegl->frameBuffer[y][x] = RGBAValue(1, 34, 1);

              }
          }
        }
            FILE *f = fopen("image.ppm", "w");         // Write image to PPM file.
            fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
            for (int i=0; i<w*h; i++)
                fprintf(f,"%d %d %d ", toInt(c[i].x), toInt(c[i].y), toInt(c[i].z));

//#pragma omp parallel
//    {
//#pragma omp for
//    for (int j = image_height-1; j >= 0; --j) {
////            std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
//            for (int i = 0; i < image_width; ++i) {
//                auto u = double(i) / (image_width-1);
//                auto v = double(j) / (image_height-1);
//                Cartesian3 pixel_color;
//                for(int k=0;k<8;k++){
//                    Ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
//                    pixel_color = pixel_color + rayColor(r, 0, world);
//                }
//                pixel_color = pixel_color/8.0;
//                RGBAValue color = RGBAValue(pixel_color.x, pixel_color.y, pixel_color.z);
//                this->fakegl->frameBuffer[j][i] = color;
//            }
//        }
//}

    return 1;
}
