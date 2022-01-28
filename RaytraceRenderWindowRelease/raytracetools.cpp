#include "raytracetools.h"

#include "fakegl.h"

#include <random>
std:: default_random_engine generator;
std::uniform_real_distribution<double> distr(0.0,1.0);
double erand48(int X){
    return distr(generator);
}

triangle::triangle(std::vector<screenVertexWithAttributes> vertexsList_){
    vertexsList = vertexsList_;
    this->objType = 0;
    this->emitte = Cartesian3(12,12,12);
    this->normal = vertexsList[0].normal.Vector();
    this->reflectType = DIFF;
}

double triangle::intersect(Ray &r) {

    auto P0 = vertexsList[0].position;
    auto P1 = vertexsList[1].position;
    auto P2 = vertexsList[2].position;

    auto E1 = P1-P0;
    auto E2 = P2-P0;
    auto S  = r.origin()-P0;
    auto S1 = r.direction().cross(E2);
    auto S2 = S.cross(E1);

    float beta = 1/S1.dot(E1);

    float t = beta*(S2.dot(E2));
    float b1 = beta*(S1.dot(S));
    float b2 = beta*(S2.dot(r.direction()));

    if(b1>=0 && b1<=1 && b2>=0 && b2<=1 && (1-b1-b2)>=0 && (1-b1-b2)<=1){ // hitted
        return t;
    }else // not histted
        return 0;


}


Cartesian3 RayTrace::radiance(Ray &r, int depth, hitList &world, int Xi){

    Sphere  * sphereobj = NULL;
    triangle * triObj = NULL;
    box * boxObj = NULL;
    double t;
    int id=0;
    if(!world.intersect(r, t, id))
        return Cartesian3();
    hitable * hittedObj = world.objects[id];
    if(++depth>5){
        //TODO::russian roulette
        return Cartesian3();
    }
    Cartesian3 n,nl, color; //n means normal, nl means to inside or outside
    Cartesian3 hitPoint = r.origin()+r.direction()*t;
    if(hittedObj->objType==0) {        // triangle



        triObj = dynamic_cast<triangle*>(hittedObj);
        n = triObj->normal;
        nl = triObj->normal.dot(r.direction())?triObj->normal:triObj->normal*-1;
        Cartesian3 x= hitPoint;
//        n=(x-triObj->position).unit();
//        nl=n.dot(r.direction())<0?n:n*-1;
        color = Cartesian3(255,0,0);

        if(triObj->reflectType==DIFF){

            double r1 = 2*M_PI*erand48(1);
            double r2 = erand48(1);
            double r2s = sqrt(r2);
            Cartesian3 w=nl;
            Cartesian3 u=((fabs(w.x)>.1?Cartesian3(0,1,0):Cartesian3(1,0,0))%w).unit();
            Cartesian3 v = w%u;
            Cartesian3 d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).unit();
            Ray outputRay(hitPoint, d);
            Cartesian3 a;
            auto pixelColor = radiance(outputRay, depth, world, Xi);
            return  triObj->emitte + color.mult(pixelColor);

        }
        else if(triObj->reflectType == SPEC){

            Ray specularReflectRay(hitPoint, r.direction()-n*2*n.dot(r.direction()));
            auto pixelColor = radiance(specularReflectRay, depth, world, Xi);
            return triObj->emitte + color.mult(pixelColor);

        }
        Ray reflRay(hitPoint, r.direction()-n*2*n.dot(r.direction()));
        bool into = n.dot(nl)>0;
        double nc=1 ,nt =1.5, nnt = into?nc/nt:nt/nc, ddn=r.direction().dot(nl), cos2t;
        if((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)
            return triObj->emitte + color.mult(radiance(reflRay, depth,world, Xi));
        Cartesian3 tdir = (r.direction()*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).unit();
           double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
           double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
           auto tempRay = Ray(hitPoint,tdir);
           return triObj->emitte + color.mult(depth>2 ? (erand48(1)<P ?   // Russian roulette
             radiance(reflRay,depth, world,Xi)*RP:radiance(tempRay,depth, world, Xi)*TP) :
             radiance(reflRay,depth, world,Xi)*Re+radiance(tempRay,depth, world, Xi)*Tr);





    }
    else if(hittedObj->objType==1){ // sphere



        sphereobj = dynamic_cast<Sphere*>(hittedObj);
        Cartesian3 x= hitPoint;
        n=(x-sphereobj->position).unit();
        nl=n.dot(r.direction())<0?n:n*-1;
        color = sphereobj->color;

        if(sphereobj->reflectType==DIFF){

            double r1 = 2*M_PI*erand48(1);
            double r2 = erand48(1);
            double r2s = sqrt(r2);
            Cartesian3 w=nl;
            Cartesian3 u=((fabs(w.x)>.1?Cartesian3(0,1,0):Cartesian3(1,0,0))%w).unit();
            Cartesian3 v = w%u;
            Cartesian3 d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).unit();
            Ray outputRay(hitPoint, d);
            Cartesian3 a;
            auto pixelColor = radiance(outputRay, depth, world, Xi);
            return  sphereobj->emitte+ color.mult(pixelColor);

        }
        else if(sphereobj->reflectType == SPEC){

            Ray specularReflectRay(hitPoint, r.direction()-n*2*n.dot(r.direction()));
            auto pixelColor = radiance(specularReflectRay, depth, world, Xi);
            return sphereobj->emitte + color.mult(pixelColor);

        }
        Ray reflRay(hitPoint, r.direction()-n*2*n.dot(r.direction()));
        bool into = n.dot(nl)>0;
        double nc=1 ,nt =1.5, nnt = into?nc/nt:nt/nc, ddn=r.direction().dot(nl), cos2t;
        if((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)
            return sphereobj->emitte + color.mult(radiance(reflRay, depth,world, Xi));
        Cartesian3 tdir = (r.direction()*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).unit();
           double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
           double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
           auto tempRay = Ray(hitPoint,tdir);
           return sphereobj->emitte + color.mult(depth>2 ? (erand48(1)<P ?   // Russian roulette
             radiance(reflRay,depth, world,Xi)*RP:radiance(tempRay,depth, world, Xi)*TP) :
             radiance(reflRay,depth, world,Xi)*Re+radiance(tempRay,depth, world, Xi)*Tr);

    }
    else if(hittedObj->objType==2) {        // box



        boxObj = dynamic_cast<box*>(hittedObj);
        n = boxObj->normal;
        nl = boxObj->normal.dot(r.direction())?boxObj->normal:boxObj->normal*-1;
        Cartesian3 x= hitPoint;
//        n=(x-triObj->position).unit();
//        nl=n.dot(r.direction())<0?n:n*-1;
        color = boxObj->color;

        if(boxObj->reflectType==DIFF){

            double r1 = 2*M_PI*erand48(1);
            double r2 = erand48(1);
            double r2s = sqrt(r2);
            Cartesian3 w=nl;
            Cartesian3 u=((fabs(w.x)>.1?Cartesian3(0,1,0):Cartesian3(1,0,0))%w).unit();
            Cartesian3 v = w%u;
            Cartesian3 d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).unit();
            Ray outputRay(hitPoint, d);
            Cartesian3 a;
            auto pixelColor = radiance(outputRay, depth, world, Xi);
            if(pixelColor == a)
                int b;
            else{
               color = color.mult(pixelColor);
            }

            return  boxObj->emitte + color;//.mult(pixelColor);
//            return Cartesian3(1,0,0);

        }
        else if(boxObj->reflectType == SPEC){

            Ray specularReflectRay(hitPoint, r.direction()-n*2*n.dot(r.direction()));
            auto pixelColor = radiance(specularReflectRay, depth, world, Xi);
            return boxObj->emitte + color.mult(pixelColor);

        }
        Ray reflRay(hitPoint, r.direction()-n*2*n.dot(r.direction()));
        bool into = n.dot(nl)>0;
        double nc=1 ,nt =1.5, nnt = into?nc/nt:nt/nc, ddn=r.direction().dot(nl), cos2t;
        if((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)
            return boxObj->emitte + color.mult(radiance(reflRay, depth,world, Xi));
        Cartesian3 tdir = (r.direction()*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).unit();
           double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
           double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
           auto tempRay = Ray(hitPoint,tdir);
           return boxObj->emitte + color.mult(depth>2 ? (erand48(1)<P ?   // Russian roulette
             radiance(reflRay,depth, world,Xi)*RP:radiance(tempRay,depth, world, Xi)*TP) :
             radiance(reflRay,depth, world,Xi)*Re+radiance(tempRay,depth, world, Xi)*Tr);

        }

}




inline double clamp(double x){ return x<0 ? 0 : x>1 ? 1 : x; }

inline int toInt(double x){ return int(pow(clamp(x),1/2.2)*255+.5); }

int RayTrace::run(int command){

    Sphere spheresCustom[] = {//Scene: radius, position, emission, color, material
                       Sphere(1e5, Cartesian3( 1e5+1,40.8,81.6), Cartesian3(),Cartesian3(.75,.25,.25),DIFF),//Left
                          Sphere(1e5, Cartesian3(-1e5+99,40.8,81.6),Cartesian3(),Cartesian3(.25,.25,.75),DIFF),//Rght
                          Sphere(1e5, Cartesian3(50,40.8, 1e5),     Cartesian3(),Cartesian3(.23,.75,.75),DIFF),//Back
                          Sphere(1e5, Cartesian3(50,40.8,-1e5+170), Cartesian3(),Cartesian3(),           REFR),//Frnt
                          Sphere(1e5, Cartesian3(50, 1e5, 81.6),    Cartesian3(),Cartesian3(.75,.5,.75),DIFF),//Botm
                          Sphere(1e5, Cartesian3(50,-1e5+81.6,81.6),Cartesian3(),Cartesian3(.75,.75,.9),DIFF),//Top
                          Sphere(16.5,Cartesian3(27,16.5,47),       Cartesian3(),Cartesian3(1,1,1)*.999, REFR),//Mirr
                          Sphere(22, Cartesian3(73,32,78),       Cartesian3(),Cartesian3(1,1,1)*.999, REFR),//Glas
                          Sphere(600, Cartesian3(50,681.6-.27,81.6),Cartesian3(12,12,12),  Cartesian3(), DIFF)
    };

    Sphere spheres[] = {//Scene: radius, position, emission, color, material
                       Sphere(1e5, Cartesian3( 1e5+1,40.8,81.6), Cartesian3(),Cartesian3(.75,.25,.25),DIFF),//Left
                          Sphere(1e5, Cartesian3(-1e5+99,40.8,81.6),Cartesian3(),Cartesian3(.25,.25,.75),DIFF),//Rght
                          Sphere(1e5, Cartesian3(50,40.8, 1e5),     Cartesian3(),Cartesian3(.75,.75,.75),DIFF),//Back
                          Sphere(1e5, Cartesian3(50,40.8,-1e5+170), Cartesian3(),Cartesian3(),           DIFF),//Frnt
                          Sphere(1e5, Cartesian3(50, 1e5, 81.6),    Cartesian3(),Cartesian3(.75,.75,.75),DIFF),//Botm
                          Sphere(1e5, Cartesian3(50,-1e5+81.6,81.6),Cartesian3(),Cartesian3(.75,.75,.75),DIFF),//Top
                          Sphere(16.5,Cartesian3(27,16.5,47),       Cartesian3(),Cartesian3(1,1,1)*.999, DIFF),//Mirr
                          Sphere(32,Cartesian3(73,32,78),       Cartesian3(),Cartesian3(1,1,1)*.999, REFR),//Glas
                          Sphere(600, Cartesian3(50,681.6-.27,81.6),Cartesian3(12,12,12),  Cartesian3(), DIFF)
    };
    hitList world;
    if(command == 0){
        int n=sizeof(spheres)/sizeof(Sphere);

        for(int i=0;i<n;i++){
            world.add(&spheres[i]);
        }

    }else if( command ==1){
        int n=sizeof(spheresCustom)/sizeof(Sphere);

        for(int i=0;i<n;i++){
            world.add(&spheres[i]);
        }

    }

//    box b(Cartesian3(55,10,78), Cartesian3(80,30,90),Cartesian3(1,1,1)*.999,Cartesian3(), DIFF);
//    box b2(Cartesian3(10,43,23), Cartesian3(20,10,59),Cartesian3(1,1,1)*.999,Cartesian3(12,12,12), DIFF);

//    world.add(&b);
//    world.add(&b2);
//    auto meshLish = this->fakegl->meshListInworldCS;
//    for(int i=0;i<meshLish.size();i++){
//       triangle * tri = new triangle(meshLish[i]);
//       world.add(tri);
//    }

    std::cout<<"render"<<std::endl;
    int samps = 300;
    int w = this->fakegl->frameBuffer.width, h = this->fakegl->frameBuffer.height;
//    int w=1024, h=768; // # samples
    Ray cam(Cartesian3(50,52,295.6), Cartesian3(0,-0.042612,-1).unit()); // cam pos, dir
//     Ray cam(Cartesian3(0,0,0), Cartesian3(0,-0.042612,-1).unit()); // cam pos, dir
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
                fakegl->frameBuffer[y][x] = RGBAValue(toInt(c[i].x), toInt(c[i].y), toInt(c[i].z));
//                fakegl->frameBuffer[y][x] = RGBAValue(1, 34, 1);

              }
          }
        }
            FILE *f = fopen("image.ppm", "w");         // Write image to PPM file.
            fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
            for (int i=0; i<w*h; i++)
                fprintf(f,"%d %d %d ", toInt(c[i].x), toInt(c[i].y), toInt(c[i].z));


    return 1;
}
