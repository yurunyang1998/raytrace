//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  FakeGL.cpp
//  ------------------------
//
//  A unit for implementing OpenGL workalike calls
//
///////////////////////////////////////////////////

#include "fakegl.h"
#include <math.h>
#include "raytracetools.h"
//-------------------------------------------------//
//                                                 //
// CONSTRUCTOR / DESTRUCTOR                        //
//                                                 //
//-------------------------------------------------//

// constructor
FakeGL::FakeGL()
    { // constructor
        std::cout<<"construct FakeGL"<<std::endl;

    } // constructor

// destructor
FakeGL::~FakeGL()
    { // destructor
    } // destructor

//-------------------------------------------------//
//                                                 //
// GEOMETRIC PRIMITIVE ROUTINES                    //
//                                                 //
//-------------------------------------------------//

// starts a sequence of geometric primitives
void FakeGL::Begin(unsigned int PrimitiveType)
    { // Begin()

       this->currentPrimitive = PrimitiveType; // set the primitive type

    } // Begin()

// ends a sequence of geometric primitives
void FakeGL::End()
    { // End()
        this->currentPrimitive = -1;
    } // End()

// sets the size of a point for drawing
void FakeGL::PointSize(float size)
    { // PointSize()
        this->pointSize = size;    // set the print size
    } // PointSize()

// sets the width of a line for drawing purposes
void FakeGL::LineWidth(float width)
    { // LineWidth()
        this->lineWidth = width;
    } // LineWidth()

//-------------------------------------------------//
//                                                 //
// MATRIX MANIPULATION ROUTINES                    //
//                                                 //
//-------------------------------------------------//



auto FakeGL::reflect(const Cartesian3 & vec,const Cartesian3 & normal) -> Cartesian3
{
    float dn = 2 * vec.dot(normal);
    return vec - normal * dn;
}





// set the matrix mode (i.e. which one we change)
void FakeGL::MatrixMode(unsigned int whichMatrix)
    { // MatrixMode()
        this->currentMatMode = whichMatrix;
    } // MatrixMode()

// pushes a matrix on the stack
void FakeGL::PushMatrix()
    { // PushMatrix()
//    std::cout<<"Push"<<std::endl;
        if(this->currentMatMode==FAKEGL_MODELVIEW){
            this->matStack.push(this->modelViewMat);
        }else if(this->currentMatMode == FAKEGL_PROJECTION){
            this->matStack.push(this->projectionMat);
        }
    } // PushMatrix()

// pops a matrix off the stack
void FakeGL::PopMatrix()
    { // PopMatrix()
    std::cout<<"pop"<<std::endl;
        if(this->currentMatMode==FAKEGL_MODELVIEW){
            this->modelViewMat = this->matStack.top();
            this->matStack.pop();
        }else if(this->currentMatMode == FAKEGL_PROJECTION){
            this->projectionMat = this->matStack.top();
            this->matStack.pop();
        }
    } // PopMatrix()

// load the identity matrix
void FakeGL::LoadIdentity()
    { // LoadIdentity()
        if(this->currentMatMode==FAKEGL_MODELVIEW){
            this->modelViewMat.SetIdentity();
        }else if(this->currentMatMode == FAKEGL_PROJECTION){
            this->projectionMat.SetIdentity();
        }
    } // LoadIdentity()

// multiply by a known matrix in column-major format
void FakeGL::MultMatrixf(const float *columnMajorCoordinates)
    { // MultMatrixf()
    std::cout<<"MultMatrixf"<<std::endl;

    Matrix4 mat;
//    memcpy(&mat.coordinates[0],columnMajorCoordinates,sizeof(float) * 16);
//    mat = mat.transpose();

    for(int i=0;i<16;i++){
        int x = i%4;
        int y = i/4;
        mat[x][y] = columnMajorCoordinates[i];
    }

    if(this->currentMatMode==FAKEGL_MODELVIEW){
        this->modelViewMat =  this->modelViewMat *mat;
    }else if(this->currentMatMode == FAKEGL_PROJECTION){
        this->projectionMat = this->projectionMat * mat;
    }



    } // MultMatrixf()

// sets up a perspective projection matrix
void FakeGL::Frustum(float left, float right, float bottom, float top, float zNear, float zFar)
    { // Frustum()
//       std::cout<<"Frustum"<<std::endl;
//        if(this->currentMatMode==FAKEGL_MODELVIEW){
       Matrix4 mat;
       mat.SetZero();
       float A = (right+left)/(right-left);
       float B = (top+bottom)/(top-bottom);
       float C = -(zFar+zNear)/(zFar-zNear);
       float D = -(2*zFar*zNear)/(zFar-zNear);

       mat[0][0] = (2.f*zNear)/(right-left);
       mat[1][1] = (2.f*zNear)/(top-bottom);
       mat[0][2] = A;
       mat[1][2] = B;
       mat[2][2] = C;
       mat[2][3] = D;
       mat[3][2] = -1;

        if(this->currentMatMode == FAKEGL_MODELVIEW){
            this->modelViewMat = this->modelViewMat * mat;
        }else if(this->currentMatMode == FAKEGL_PROJECTION){
            this->projectionMat = this->projectionMat * mat;
        }



    } // Frustum()

// sets an orthographic projection matrix
void FakeGL::Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
    { // Ortho()
//        std::cout<<"ortho"<<std::endl;
        Matrix4 mat;
        float tx = -(right+left)/(right-left);
        float ty = -(top+bottom)/(top-bottom);
        float tz = -(zFar+zNear)/(zFar-zNear);

        mat[0][0] = 2.f/(right-left);
        mat[0][3] = tx;
        mat[1][1] = 2/(top-bottom);
        mat[1][3] = ty;
        mat[2][2] = -2/(zFar-zNear);
        mat[2][3] = tz;
        mat[3][3] = 1;

        if(this->currentMatMode == FAKEGL_MODELVIEW){
            this->modelViewMat =  this->modelViewMat * mat;
        }else if(this->currentMatMode == FAKEGL_PROJECTION){
            this->projectionMat = this->projectionMat * mat;
        }


    } // Ortho()

// rotate the matrix
void FakeGL::Rotatef(float angle, float axisX, float axisY, float axisZ)
    { // Rotatef()
//        std::cout<<"rotation mat"<<std::endl;
        Matrix4 rotationMat;
        float theta = angle * 3.14 / 180.f;
        rotationMat.SetRotation(Cartesian3(axisX, axisY, axisZ),theta);
        if(this->currentMatMode==FAKEGL_MODELVIEW){
            this->modelViewMat =  this->modelViewMat * rotationMat;
        }else if(this->currentMatMode == FAKEGL_PROJECTION){
            this->projectionMat = this->projectionMat * rotationMat;
        }

    } // Rotatef()

// scale the matrix
void FakeGL::Scalef(float xScale, float yScale, float zScale)
    { // Scalef()
//        std::cout<<"scalef"<<std::endl;
        Matrix4 scaleMat;
        scaleMat.SetScale(xScale, yScale, zScale);
        if(this->currentMatMode==FAKEGL_MODELVIEW){
            this->modelViewMat =  this->modelViewMat *scaleMat;
        }else if(this->currentMatMode == FAKEGL_PROJECTION){
            this->projectionMat =  this->projectionMat *scaleMat;
        }


    } // Scalef()

// translate the matrix
void FakeGL::Translatef(float xTranslate, float yTranslate, float zTranslate)
    { // Translatef()
//    std::cout<<"translatef"<<std::endl;
    Matrix4 translateMat;
    translateMat.SetTranslation(Cartesian3(xTranslate, yTranslate, zTranslate));

    if(this->currentMatMode==FAKEGL_MODELVIEW){
        this->modelViewMat =   this->modelViewMat * translateMat ;
    }else if(this->currentMatMode == FAKEGL_PROJECTION){
        this->projectionMat =  this->projectionMat * translateMat;
    }
    } // Translatef()

// sets the viewport
void FakeGL::Viewport(int x, int y, int width, int height)
    { // Viewport()
           viewPortMat.SetZero();
           viewPortMat[0][3] = (x+width/2.f);
           viewPortMat[1][3] = (y+height/2.f);
           viewPortMat[2][3] = 1;

           viewPortMat[0][0] = width/2.f;
           viewPortMat[1][1] = height/2.f;
           viewPortMat[2][2] = 1;
           viewPortMat[3][3] = 1;


    } // Viewport()

//-------------------------------------------------//
//                                                 //
// VERTEX ATTRIBUTE ROUTINES                       //
//                                                 //
//-------------------------------------------------//

// sets colour with floating point
void FakeGL::Color3f(float red, float green, float blue)
    { // Color3f()
        this->colorf.red = red*255;
        this->colorf.green = green*255;
        this->colorf.blue = blue*255;


    } // Color3f()

// sets material properties
//void FakeGL::Materialf(unsigned int parameterName, const float parameterValue)
//    { // Materialf()
//        if(parameterName & FAKEGL_SHININESS){
//            shinessMaterial = parameterValue;
//        }

//    } // Materialf()

void FakeGL::Materialf(unsigned int parameterName, const float parameterValue)
    { // Materialf()
    if (parameterName & FAKEGL_AMBIENT)
        ambientMaterial[0] = ambientMaterial[1] = ambientMaterial[2] = parameterValue;

    if (parameterName & FAKEGL_DIFFUSE)
        diffuseMaterial[0] = diffuseMaterial[1] = diffuseMaterial[2] = parameterValue;

    if (parameterName & FAKEGL_SPECULAR)
        specularMaterial[0] = specularMaterial[1] = specularMaterial[2] = parameterValue;

    if (parameterName & FAKEGL_EMISSION)
        emissionMaterial[0] = emissionMaterial[1] = emissionMaterial[2] = parameterValue;

    if (parameterName & FAKEGL_SHININESS)
        shinessMaterial = parameterValue;
    } // Materialf()


void FakeGL::Materialfv(unsigned int parameterName, const float *parameterValues)
    { // Materialfv()
        if(parameterName & FAKEGL_EMISSION){
            this->emissionMaterial[0] = parameterValues[0];
            this->emissionMaterial[1] = parameterValues[1];
            this->emissionMaterial[2] = parameterValues[2];
            this->emissionMaterial[3] = parameterValues[3];
        }
        if(parameterName&FAKEGL_DIFFUSE){
            this->diffuseMaterial[0] = 1.0;//parameterValues[0];
            this->diffuseMaterial[1] = 1.0;//parameterValues[1];
            this->diffuseMaterial[2] = 1.0;// parameterValues[2];
            this->diffuseMaterial[3] = 1.0;//parameterValues[3];
        }
        if(parameterName&FAKEGL_SPECULAR){
            this->specularMaterial[0] = 1.0;// parameterValues[0];
            this->specularMaterial[1] = 1.0;//parameterValues[1];
            this->specularMaterial[2] = 1.0;//parameterValues[2];
            this->specularMaterial[3] = 1.0;//parameterValues[3];
           }
        if(parameterName & FAKEGL_AMBIENT){
//            this->ambientMaterial[0] = parameterValues[0];
//            this->ambientMaterial[1] = parameterValues[1];
//            this->ambientMaterial[2] = parameterValues[2];
//            this->ambientMaterial[3] = parameterValues[3];

            this->ambientMaterial[0] = 1.0;// parameterValues[0];
            this->ambientMaterial[1] = 1.0;//parameterValues[1];
            this->ambientMaterial[2] = 1.0;//parameterValues[2];
            this->ambientMaterial[3] = 1.0;//parameterValues[3];

            std::cout<<"ambientMaterial "<<ambientMaterial[0]<<" "<<ambientMaterial[1]<<" "<<ambientMaterial[2]<<" "<<ambientMaterial[3]<<std::endl;

        }


    } // Materialfv()

// sets the normal vector
void FakeGL::Normal3f(float x, float y, float z)
    { // Normal3f()
        this->normal = Homogeneous4(x,y,z,0);
    } // Normal3f()

// sets the texture coordinates
void FakeGL::TexCoord2f(float u, float v)
    { // TexCoord2f()

        this->textureU = u;
        this->textureV = v;

    } // TexCoord2f()

// sets the vertex & launches it down the pipeline
void FakeGL::Vertex3f(float x, float y, float z)
    { // Vertex3f()
        vertexWithAttributes vertex(x,y,z);
        vertex.u = this->textureU;
        vertex.v = this->textureV;
        vertex.normal = this->normal;




        this->vertexQueue.push_back(vertex);
        TransformVertex();
    } // Vertex3f()

//-------------------------------------------------//
//                                                 //
// STATE VARIABLE ROUTINES                         //
//                                                 //
//-------------------------------------------------//

// disables a specific flag in the library
void FakeGL::Disable(unsigned int property)
    { // Disable()
            if(property == FAKEGL_DEPTH_TEST)
                this->enable_depth_test = false;
            if(property == FAKEGL_LIGHTING)
                this->enable_lighting = false;
            if(property == FAKEGL_TEXTURE_2D)
                this->enable_texture_2D = false;
            if(property == FAKEGL_PHONG_SHADING)
                this->enable_phonh_shading = false;

    } // Disable()

// enables a specific flag in the library
void FakeGL::Enable(unsigned int property)
    { // Enable()
//        std::cout<<"enable"<<std::endl;
        if(property == FAKEGL_DEPTH_TEST){
            this->enable_depth_test = true;
            this->depthBuffer.Resize(frameBuffer.width, frameBuffer.height);
        }
        if(property ==  FAKEGL_LIGHTING)
            this->enable_lighting = true;
        if(property == FAKEGL_TEXTURE_2D)
            this->enable_texture_2D = true;
        if(property == FAKEGL_PHONG_SHADING)
            this->enable_phonh_shading = true;

    } // Enable()

//-------------------------------------------------//
//                                                 //
// LIGHTING STATE ROUTINES                         //
//                                                 //
//-------------------------------------------------//

// sets properties for the one and only light
void FakeGL::Light(int parameterName, const float *parameterValues)
    { // Light()

        if(parameterName & FAKEGL_POSITION){
            positionLight[0] = parameterValues[0];
            positionLight[1] = parameterValues[1];
            positionLight[2] = parameterValues[2];
            positionLight[3] = parameterValues[3];

            Homogeneous4 transformedLight(parameterValues[0], parameterValues[1], parameterValues[2], parameterValues[3]);
            lightPosition = this->modelViewMat*transformedLight;
            std::cout<<"light "<<transformedLight<<"  "<<lightPosition<<std::endl;
        }

        if(parameterName & FAKEGL_AMBIENT){
            ambietLight[0] = parameterValues[0];
            ambietLight[1] = parameterValues[1];
            ambietLight[2] = parameterValues[2];
            ambietLight[3] = parameterValues[3];

            std::cout<<"ambientLight "<<ambietLight[0]<<" "<<ambietLight[1]<<" "<<ambietLight[2]<<" "<<ambietLight[3]<<std::endl;

        }
        if(parameterName & FAKEGL_DIFFUSE){
            diffuseLight[0]=parameterValues[0];
            diffuseLight[1]=parameterValues[1];
            diffuseLight[2]=parameterValues[2];
            diffuseLight[3]=parameterValues[3];

        }
        if(parameterName & FAKEGL_SPECULAR){
            specularLight[0] = parameterValues[0];
            specularLight[1] = parameterValues[1];
            specularLight[2] = parameterValues[2];
            specularLight[3] = parameterValues[3];

        }




    } // Light()

//-------------------------------------------------//
//                                                 //
// TEXTURE PROCESSING ROUTINES                     //
//                                                 //
// Note that we only allow one texture             //
// so glGenTexture & glBindTexture aren't needed   //
//                                                 //
//-------------------------------------------------//

// sets whether textures replace or modulate
void FakeGL::TexEnvMode(unsigned int textureMode)
    { // TexEnvMode()
        if(textureMode == FAKEGL_MODULATE){
            this->textureMode = FAKEGL_MODULATE;
        }
        if(textureMode == FAKEGL_REPLACE){
            this->textureMode = FAKEGL_REPLACE;
        }
    } // TexEnvMode()

// sets the texture image that corresponds to a given ID
void FakeGL::TexImage2D(const RGBAImage &textureImage)
    { // TexImage2D()

        this->texture = textureImage;
    } // TexImage2D()

//-------------------------------------------------//
//                                                 //
// FRAME BUFFER ROUTINES                           //
//                                                 //
//-------------------------------------------------//

// clears the frame buffer
void FakeGL::Clear(unsigned int mask)
    { // Clear()
//    std::cout<<"clean"<<std::endl;
        int width = this->frameBuffer.width;
        int height = this->frameBuffer.height;
        for(int i=0;i<height;i++){
            for(int j=0;j<width;j++){
                this->frameBuffer[i][j] = backGroundColor;
                if(this->enable_depth_test) // clean depth buffer
                    this->depthBuffer[i][j].alpha = 255;
            }
        }

} // Clear()



// sets the clear colour for the frame buffer
void FakeGL::ClearColor(float red, float green, float blue, float alpha)
    { // ClearColor()
           this->backGroundColor=RGBAValue(red*255,green*255,blue*255, alpha*255);
    } // ClearColor()

//-------------------------------------------------//
//                                                 //
// MAJOR PROCESSING ROUTINES                       //
//                                                 //
//-------------------------------------------------//

// transform one vertex & shift to the raster queue
void FakeGL::TransformVertex()
    { // TransformVertex()
//    auto vertex = this->vertexQueue.front();
//            vertexQueue.pop_front();

//            // implement matrix
//            Homogeneous4 hg4(vertex.position.x, vertex.position.y, vertex.position.z);
//            auto wdcs =  this->modelViewMat* hg4;
//            Homogeneous4 screenResult = this->projectionMat* wdcs;
//            Cartesian3 ndcs = screenResult.Point();
//            Homogeneous4 ndcsHg4 = Homogeneous4(ndcs.x, ndcs.y, ndcs.z);
//            screenResult = this->viewPortMat * ndcsHg4;

//            Homogeneous4 hg4Normal = this->modelViewMat * vertex.normal;



//            screenVertexWithAttributes  screenVertex(screenResult.x, screenResult.y, screenResult.z);
//            screenVertex.colour = this->colorf;
//            screenVertex.normal = hg4Normal;

//            if(this->enable_texture_2D){
//                screenVertex.u = this->textureU;
//                screenVertex.v = this->textureV;
//            }


//            this->rasterQueue.push_back(screenVertex);

//            RasterisePrimitive();





        auto vertex = this->vertexQueue.front();
        vertexQueue.pop_front();

//        // implement matrix
        Homogeneous4 hg4(vertex.position.x, vertex.position.y, vertex.position.z);
        auto temp =  this->modelViewMat* hg4;
        screenVertexWithAttributes  vertexInWorldCS(temp.x, temp.y, temp.z);


        Homogeneous4 hg4Normal = this->modelViewMat * vertex.normal;



        vertexInWorldCS.colour = this->colorf;
        vertexInWorldCS.normal = hg4Normal;

        if(this->enable_texture_2D){
            vertexInWorldCS.u = this->textureU;
            vertexInWorldCS.v = this->textureV;
        }

        this->rasterQueue.push_back(vertexInWorldCS);

        RasterisePrimitive();

    } // TransformVertex()

// rasterise a single primitive if there are enough vertices on the queue
bool FakeGL::RasterisePrimitive()
    { // RasterisePrimitive()
    switch (this->currentPrimitive) {
        case 0:{ //rasterise a vertex
            if(this->rasterQueue.size()>=1){
//                std::cout<<"rasterise point"<<std::endl;
                auto vertex = rasterQueue.front();
                rasterQueue.pop_front();
                //screenVertexWithAttributes rasterVertex(vertex.position.x, vertex.position.y, vertex.position.z);
                RasterisePoint(vertex);
                return true;
            }else
                return false;
        }
        case 1:{  //rasterise a line
            if(this->rasterQueue.size()>=2){

//                std::cout<<"rasterise line"<<std::endl;
                auto vertex1 = rasterQueue.front();rasterQueue.pop_front();
                auto vertex2 = rasterQueue.front();rasterQueue.pop_front();
//                screenVertexWithAttributes rasterVertex1(vertex1.position.x, vertex1.position.y, vertex1.position.z);
//                screenVertexWithAttributes rasterVertex2(vertex2.position.x, vertex2.position.y, vertex2.position.z);

                this->RasteriseLineSegment(vertex1, vertex2);
                return true;
            }else
                return false;
        }
        case 2:{     // rasterise a triangle

//            std::cout<<"render triangle"<<std::endl;

            if(this->rasterQueue.size()>=3){
                auto vertex1 = rasterQueue.front();rasterQueue.pop_front();
                auto vertex2 = rasterQueue.front();rasterQueue.pop_front();
                auto vertex3 = rasterQueue.front();rasterQueue.pop_front();

                std::vector<screenVertexWithAttributes> face;
                face.push_back(vertex1);
                face.push_back(vertex2);
                face.push_back(vertex3);

                this->meshListInworldCS.push_back(face);

                return true;
            }else
                return false;
        }
      }
    return false;   //can't rasterise a primitive;
    } // RasterisePrimitive()



// depth test
bool FakeGL::depthTest(int x, int y, float z){
    if(this->depthBuffer[x][y].alpha >= z/2){
        this->depthBuffer[x][y].alpha = z/2;
        return true;
    }else
        return false;
}





// rasterises a single point
void FakeGL::RasterisePoint(screenVertexWithAttributes &vertex0)
    { // RasterisePoint()
//        std::cout<<"rasterise"<<std::endl;
        int x = vertex0.position.x;
        int y = vertex0.position.y;
        int z = vertex0.position.z;
        int pointSize = this->pointSize;
        if(pointSize==0){
//            std::cout<<"pointsize is zero"<<std::endl;
            return;
        }

        for(int _x=x-(pointSize/2);_x<=x+(pointSize/2);_x++){
            for(int _y=y-(pointSize/2);_y<=y+(pointSize/2);_y++){

                //TODO: implement depth test

                fragmentWithAttributes shaderVertex(_y,_x,vertex0.colour);
                this->fragmentQueue.push_back(shaderVertex);

            }
        }

        ProcessFragment();






    } // RasterisePoint()

// rasterises a single line segment
void FakeGL::RasteriseLineSegment(screenVertexWithAttributes &vertex0, screenVertexWithAttributes &vertex1)
    { // RasteriseLineSegment()

       int x0 = vertex0.position.x, y0 = vertex0.position.y;
       int x1 = vertex1.position.x, y1 = vertex1.position.y;

//       std::cout<<x0<<" "<<y0<<std::endl;
//       std::cout<<x1<<" "<<y1<<std::endl;
       bool steep = false;
           if (std::abs(x0-x1)<std::abs(y0-y1)) {
               std::swap(x0, y0);
               std::swap(x1, y1);
               steep = true;
           }
           if (x0>x1) {
               std::swap(x0, x1);
               std::swap(y0, y1);
           }
           int dx = x1-x0;
           int dy = y1-y0;
           int derror2 = std::abs(dy)*2;
           int error2 = 0;
           int y = y0;
           for (int x=x0; x<=x1; x++) {
               if (steep) {
                   fragmentWithAttributes fragmentVertex(x, y, colorf);
                   this->fragmentQueue.push_back(fragmentVertex);
               } else {
                   fragmentWithAttributes fragmentVertex(y, x, colorf);
                   this->fragmentQueue.push_back(fragmentVertex);
               }
               error2 += derror2;
               if (error2 > dx) {
                   y += (y1>y0?1:-1);
                   error2 -= dx*2;
               }
           }




            ProcessFragment();


    } // RasteriseLineSegment()

// rasterises a single triangle
void FakeGL::RasteriseTriangle(screenVertexWithAttributes &vertex0, screenVertexWithAttributes &vertex1, screenVertexWithAttributes &vertex2)
    { // RasteriseTriangle()



    // compute a bounding box that starts inverted to frame size
    // clipping will happen in the raster loop proper
    float minX = frameBuffer.width, maxX = 0.0;
    float minY = frameBuffer.height, maxY = 0.0;

    // test against all vertices
    if (vertex0.position.x < minX) minX = vertex0.position.x;
    if (vertex0.position.x > maxX) maxX = vertex0.position.x;
    if (vertex0.position.y < minY) minY = vertex0.position.y;
    if (vertex0.position.y > maxY) maxY = vertex0.position.y;

    if (vertex1.position.x < minX) minX = vertex1.position.x;
    if (vertex1.position.x > maxX) maxX = vertex1.position.x;
    if (vertex1.position.y < minY) minY = vertex1.position.y;
    if (vertex1.position.y > maxY) maxY = vertex1.position.y;

    if (vertex2.position.x < minX) minX = vertex2.position.x;
    if (vertex2.position.x > maxX) maxX = vertex2.position.x;
    if (vertex2.position.y < minY) minY = vertex2.position.y;
    if (vertex2.position.y > maxY) maxY = vertex2.position.y;

    // now for each side of the triangle, compute the line vectors
    Cartesian3 vector01 = vertex1.position - vertex0.position;
    Cartesian3 vector12 = vertex2.position - vertex1.position;
    Cartesian3 vector20 = vertex0.position - vertex2.position;

    // now compute the line normal vectors
    Cartesian3 normal01(-vector01.y, vector01.x, 0.0);
    Cartesian3 normal12(-vector12.y, vector12.x, 0.0);
    Cartesian3 normal20(-vector20.y, vector20.x, 0.0);

    // we don't need to normalise them, because the square roots will cancel out in the barycentric coordinates
    float lineConstant01 = normal01.dot(vertex0.position);
    float lineConstant12 = normal12.dot(vertex1.position);
    float lineConstant20 = normal20.dot(vertex2.position);

    // and compute the distance of each vertex from the opposing side
    float distance0 = normal12.dot(vertex0.position) - lineConstant12;
    float distance1 = normal20.dot(vertex1.position) - lineConstant20;
    float distance2 = normal01.dot(vertex2.position) - lineConstant01;

    // if any of these are zero, we will have a divide by zero error
    // but notice that if they are zero, the vertices are collinear in projection and the triangle is edge on
    // we can render that as a line, but the better solution is to render nothing.  In a surface, the adjacent
    // triangles will eventually take care of it
    if ((distance0 == 0) || (distance1 == 0) || (distance2 == 0))
        return;

    // create a fragment for reuse
    fragmentWithAttributes rasterFragment;

    int textureWidth = texture.width;
    int textureHeight = texture.height;

    float vertex0Light[3]={0,0,0}, vertex1Light[3]={0,0,0}, vertex2Light[3]={0,0,0};
    Cartesian3 v0Normal(vertex0.normal.x, vertex0.normal.y, vertex0.normal.z);
    Cartesian3 v1Normal(vertex1.normal.x, vertex1.normal.y, vertex1.normal.z);
    Cartesian3 v2Normal(vertex2.normal.x, vertex2.normal.y, vertex2.normal.z);


    Cartesian3 eyeDir{0,0,0};
//    auto eyeDir = this->modelViewMat * eye;

    Cartesian3 vl0, vl1, vl2;
    if (lightPosition.w != 0){ // spot light
//        vl0 = vertex0.position - lightPosition.Vector() / lightPosition.w;
//        vl1 = vertex1.position - lightPosition.Vector() / lightPosition.w;
//        vl2 = vertex2.position - lightPosition.Vector() / lightPosition.w;

          vl0 =lightPosition.Vector() / lightPosition.w -  vertex0.position;
          vl1 =lightPosition.Vector() / lightPosition.w - vertex1.position;
          vl2 =lightPosition.Vector() / lightPosition.w - vertex2.position;



    }
    else{ // parallarl light

        vl0 = lightPosition.Vector();
        vl1 = lightPosition.Vector();
        vl2 = lightPosition.Vector();
    }

    v0Normal = v0Normal.unit();
    v1Normal = v1Normal.unit();
    v2Normal = v2Normal.unit();


    auto eyeDirV0 = eyeDir-vertex0.position;
    auto eyeDirV1 = eyeDir-vertex1.position;
    auto eyeDirV2 = eyeDir-vertex2.position;





    auto v0reflectDir = reflect(vl0, v0Normal);
    auto v1reflectDir = reflect(vl1, v1Normal);
    auto v2reflectDir = reflect(vl2, v2Normal);



    for(int i=0;i<3;i++){


        float v0ambient = this->ambietLight[i]*vertex0.ambientMaterial[i];
        float v1ambient = this->ambietLight[i]*vertex1.ambientMaterial[i];
        float v2ambient = this->ambietLight[i]*vertex2.ambientMaterial[i];

        float v0diffuse = diffuseLight[i]*vertex0.diffuseMaterial[i]* std::max(v0Normal.dot(vl0), 0.0f);
        float v1diffuse = diffuseLight[i]*vertex1.diffuseMaterial[i]* std::max(v1Normal.dot(vl1), 0.0f);
        float v2diffuse = diffuseLight[i]*vertex2.diffuseMaterial[i]* std::max(v2Normal.dot(vl2), 0.0f);

        float v0specular = specularLight[i]*vertex0.specularMaterial[i]*std::pow(std::max(v0reflectDir.dot(eyeDirV0), 0.0f), this->shinessMaterial);
        float v1specular = specularLight[i]*vertex1.specularMaterial[i]*std::pow(std::max(v1reflectDir.dot(eyeDirV1), 0.0f), this->shinessMaterial);
        float v2specular = specularLight[i]*vertex2.specularMaterial[i]*std::pow(std::max(v2reflectDir.dot(eyeDirV2), 0.0f), this->shinessMaterial);

        vertex0Light[i] += v0ambient + v0diffuse + v0specular+ this->emissionMaterial[i];

        vertex1Light[i] += v1ambient + v1diffuse + v1specular+ this->emissionMaterial[i];

        vertex2Light[i] += v2ambient + v2diffuse + v2specular + this->emissionMaterial[i];

    }





    // loop through the pixels in the bounding box
    for (rasterFragment.row = minY; rasterFragment.row <= maxY; rasterFragment.row++)
        { // per row
        // this is here so that clipping works correctly
        if (rasterFragment.row < 0) continue;
        if (rasterFragment.row >= frameBuffer.height) continue;
        for (rasterFragment.col = minX; rasterFragment.col <= maxX; rasterFragment.col++)
            { // per pixel
            // this is also for correct clipping
            if (rasterFragment.col < 0) continue;
            if (rasterFragment.col >= frameBuffer.width) continue;

            // the pixel in cartesian format
            Cartesian3 pixel(rasterFragment.col, rasterFragment.row, 0.0);

            // right - we have a pixel inside the frame buffer AND the bounding box
            // note we *COULD* compute gamma = 1.0 - alpha - beta instead
            float alpha = (normal12.dot(pixel) - lineConstant12) / distance0;
            float beta = (normal20.dot(pixel) - lineConstant20) / distance1;
            float gamma = (normal01.dot(pixel) - lineConstant01) / distance2;

            // now perform the half-plane test
            if ((alpha < 0.0) || (beta < 0.0) || (gamma < 0.0))
                continue;

            float z = vertex0.position.z * alpha+ vertex1.position.z * beta + vertex2.position.z * gamma;

            Cartesian3 interplVertex = Cartesian3(vertex0.position.x*alpha + vertex1.position.x*beta + vertex2.position.x*gamma,
                                                  vertex0.position.y*alpha + vertex1.position.y*beta + vertex2.position.y*gamma,
                                                  vertex0.position.z*alpha + vertex1.position.z*beta + vertex2.position.z*gamma);

            rasterFragment.colour = alpha * vertex0.colour + beta * vertex1.colour + gamma * vertex2.colour;

            if(this->enable_depth_test){
                if(depthTest(rasterFragment.row, rasterFragment.col,z*255)){
                    // now we add it to the queue for fragment processing
                }else
                    continue;
            }



            if(this->enable_texture_2D){

                int interpU = (alpha * vertex0.u + beta * vertex1.u + gamma * vertex2.u)* textureWidth;
                int interpV = (alpha * vertex0.v + beta * vertex1.v + gamma * vertex2.v)* textureHeight;

                if(this->textureMode == FAKEGL_REPLACE){
                    rasterFragment.colour = this->texture[interpV][interpU];

                }else if( this->textureMode == FAKEGL_MODULATE){
                    rasterFragment.colour.modulate(this->texture[interpV][interpU]);
            }
          }

            if(this->enable_lighting){

               if(this->enable_phonh_shading){

                   Cartesian3 interplNormal;
                   interplNormal = alpha * v0Normal + beta*v1Normal + gamma*v2Normal;

                   float interplAmbientMaterial[3];
                   float interplDiffuseMaterial[3];
                   float interplspecularMastrial[3];
                   for(int i=0;i<3;i++){
                       interplAmbientMaterial[i] = alpha*vertex0.ambientMaterial[i] + beta* vertex1.ambientMaterial[i] + gamma*vertex2.ambientMaterial[i];
                       interplDiffuseMaterial[i] = alpha*vertex0.diffuseMaterial[i] + beta * vertex1.diffuseMaterial[i] + gamma*vertex2.diffuseMaterial[i];
                       interplspecularMastrial[i] = alpha*vertex0.specularMaterial[i] + beta *vertex1.specularMaterial[i] + gamma*vertex2.specularMaterial[i];
                   }
                   Cartesian3 vl = interplVertex - lightPosition.Vector();
                   auto lightReflect = reflect(vl, interplNormal);

                   eyeDir = {0,0,0};
                   eyeDir = eyeDir - interplVertex;


                   rasterFragment.colour.red *= ambietLight[0]*interplAmbientMaterial[0] +
                      diffuseLight[0]*interplspecularMastrial[0]*std::max(interplNormal.dot(vl), 0.0f)+
                      specularLight[0]*interplspecularMastrial[0]*std::pow(std::max(lightReflect.dot(eyeDir), 0.0f), this->shinessMaterial)+
                           this->emissionMaterial[0];

                   rasterFragment.colour.green *=  ambietLight[1]*interplAmbientMaterial[1] +
                      diffuseLight[1]*interplspecularMastrial[1]*std::max(interplNormal.dot(vl), 0.0f)+
                      specularLight[1]*interplspecularMastrial[1]*std::pow(std::max(lightReflect.dot(eyeDir), 0.0f), this->shinessMaterial)+
                           this->emissionMaterial[1];

                   rasterFragment.colour.blue *=  ambietLight[2]*interplAmbientMaterial[2] +
                      diffuseLight[2]*interplspecularMastrial[2]*std::max(interplNormal.dot(vl), 0.0f)+
                      specularLight[2]*interplspecularMastrial[2]*std::pow(std::max(lightReflect.dot(eyeDir), 0.0f), this->shinessMaterial)+
                           this->emissionMaterial[2];








                }else{ //ground shading

                    float red = (alpha*vertex0Light[0] + beta*vertex1Light[0] + gamma * vertex2Light[0]);
                    float green = (alpha*vertex0Light[1] + beta*vertex1Light[1] + gamma * vertex2Light[1]);
                    float blue = (alpha*vertex0Light[2] + beta*vertex1Light[2] + gamma * vertex2Light[2]);
                    rasterFragment.colour.red *=red;
                    rasterFragment.colour.green *=green;
                    rasterFragment.colour.blue *= blue;
                }
            }







            fragmentQueue.push_back(rasterFragment);

            } // per pixel

        } // per row
        ProcessFragment();

    } // RasteriseTriangle()

// process a single fragment
void FakeGL::ProcessFragment()
    { // ProcessFragment()
        while(!this->fragmentQueue.empty()){
                auto fragment = this->fragmentQueue.front();
                this->fragmentQueue.pop_front();

                int x = fragment.col;
                int y = fragment.row;

                this->frameBuffer[y][x] = fragment.colour;
        }



    } // ProcessFragment()




void FakeGL::raytrace(int command){
    Clear(0);

    RayTrace rt(this);
    rt.run(command);



}









// standard routine for dumping the entire FakeGL context (except for texture / image)
std::ostream &operator << (std::ostream &outStream, FakeGL &fakeGL)
    { // operator <<
    outStream << "=========================" << std::endl;
    outStream << "Dumping FakeGL Context   " << std::endl;
    outStream << "=========================" << std::endl;


    outStream << "-------------------------" << std::endl;
    outStream << "Vertex Queue:            " << std::endl;
    outStream << "-------------------------" << std::endl;
    for (auto vertex = fakeGL.vertexQueue.begin(); vertex < fakeGL.vertexQueue.end(); vertex++)
        { // per matrix
        outStream << "Vertex " << vertex - fakeGL.vertexQueue.begin() << std::endl;
        outStream << *vertex;
        } // per matrix


    outStream << "-------------------------" << std::endl;
    outStream << "Raster Queue:            " << std::endl;
    outStream << "-------------------------" << std::endl;
    for (auto vertex = fakeGL.rasterQueue.begin(); vertex < fakeGL.rasterQueue.end(); vertex++)
        { // per matrix
        outStream << "Vertex " << vertex - fakeGL.rasterQueue.begin() << std::endl;
        outStream << *vertex;
        } // per matrix


    outStream << "-------------------------" << std::endl;
    outStream << "Fragment Queue:          " << std::endl;
    outStream << "-------------------------" << std::endl;
    for (auto fragment = fakeGL.fragmentQueue.begin(); fragment < fakeGL.fragmentQueue.end(); fragment++)
        { // per matrix
        outStream << "Fragment " << fragment - fakeGL.fragmentQueue.begin() << std::endl;
        outStream << *fragment;
        } // per matrix


    return outStream;
    } // operator <<

// subroutines for other classes
std::ostream &operator << (std::ostream &outStream, vertexWithAttributes &vertex)
    { // operator <<
    std::cout << "Vertex With Attributes" << std::endl;
    std::cout << "Position:   " << vertex.position << std::endl;
    std::cout << "Colour:     " << vertex.colour << std::endl;

    // you

    return outStream;
    } // operator <<

std::ostream &operator << (std::ostream &outStream, screenVertexWithAttributes &vertex)
    { // operator <<
    std::cout << "Screen Vertex With Attributes" << std::endl;
    std::cout << "Position:   " << vertex.position << std::endl;
    std::cout << "Colour:     " << vertex.colour << std::endl;

    return outStream;
    } // operator <<

std::ostream &operator << (std::ostream &outStream, fragmentWithAttributes &fragment)
    { // operator <<
    std::cout << "Fragment With Attributes" << std::endl;
    std::cout << "Row:        " << fragment.row << std::endl;
    std::cout << "Col:        " << fragment.col << std::endl;
    std::cout << "Colour:     " << fragment.colour << std::endl;

    return outStream;
    } // operator <<






















