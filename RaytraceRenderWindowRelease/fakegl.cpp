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

#include "FakeGL.h"
#include <math.h>

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

        std::copy(std::begin(this->ambientMaterial), std::end(this->ambientMaterial), std::begin(vertex.ambientMaterial));
        std::copy(std::begin(this->emissionMaterial), std::end(this->emissionMaterial), std::begin(vertex.emissionMaterial));
        std::copy(std::begin(this->specularMaterial), std::end(this->specularMaterial), std::begin(vertex.specularMaterial));
        std::copy(std::begin(this->diffuseMaterial), std::end(this->diffuseMaterial), std::begin(vertex.diffuseMaterial));
        vertex.shinessMaterial = this->shinessMaterial;



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
        auto vertex = this->vertexQueue.front();
        vertexQueue.pop_front();

        // implement matrix
        Homogeneous4 hg4(vertex.position.x, vertex.position.y, vertex.position.z);
        auto temp =  this->modelViewMat* hg4;
        Homogeneous4 screenResult = this->projectionMat* temp;
        Cartesian3 ndcs = screenResult.Point();
        Homogeneous4 ndcsHg4 = Homogeneous4(ndcs.x, ndcs.y, ndcs.z);
        screenResult = this->viewPortMat * ndcsHg4;

        Homogeneous4 hg4Normal = this->modelViewMat * vertex.normal;



        screenVertexWithAttributes  screenVertex(screenResult.x, screenResult.y, screenResult.z);
        screenVertex.colour = this->colorf;
        screenVertex.normal = hg4Normal;

        if(this->enable_texture_2D){
            screenVertex.u = this->textureU;
            screenVertex.v = this->textureV;
        }
        if(this->enable_lighting){
            std::copy(std::begin(vertex.ambientMaterial), std::end(vertex.ambientMaterial),
                      std::begin(screenVertex.ambientMaterial));
            std::copy(std::begin(vertex.diffuseMaterial), std::end(vertex.diffuseMaterial),
                      std::begin(screenVertex.diffuseMaterial));
            std::copy(std::begin(vertex.specularMaterial), std::end(vertex.specularMaterial),
                      std::begin(screenVertex.specularMaterial));
            std::copy(std::begin(vertex.emissionMaterial), std::end(vertex.emissionMaterial),
                      std::begin(screenVertex.emissionMaterial));
           screenVertex.shinessMaterial = vertex.shinessMaterial;

        }


        this->rasterQueue.push_back(screenVertex);

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


                RasteriseTriangle(vertex1, vertex2, vertex3);
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




















////////////////////////////////////////////////////////////////////////
////
////  University of Leeds
////  COMP 5812M Foundations of Modelling & Rendering
////  User Interface for Coursework
////
////  September, 2020
////
////  ------------------------
////  FakeGL.cpp
////  ------------------------
////
////  A unit for implementing OpenGL workalike calls
////
/////////////////////////////////////////////////////

//#include "FakeGL.h"
//#include <math.h>

////-------------------------------------------------//
////                                                 //
//// CONSTRUCTOR / DESTRUCTOR                        //
////                                                 //
////-------------------------------------------------//

//// constructor
//FakeGL::FakeGL()
//    : modelViewStack(1, Matrix4()),
//      projectionStack(1, Matrix4()),
//      lightPosition(0.0, 0.0, 1.0, 0.0),
//      attributeColour((float)1.0, (float)1.0, (float)1.0, (float)1.0),
//      depthColour((float)0.0, (float)0.0, (float)0.0, (float)255.0),
//      attributeNormal(0, 0, 1)
//    { // constructor

//    // init matrix stacks
//    modelViewStack[0].SetIdentity();
//    projectionStack[0].SetIdentity();

//    // matrix state
//    viewPortSize = xPixelOrigin = yPixelOrigin = 0;

//    // init state variables
//    lighting = texture = depthTest = 0;
//    phongShading = 1;

//    // raster state
//    primitive = -1; // TODO CHANGE FROM -1
//    pointSize = lineWidth = 1;

//    // set lighting properties to general defaults, we don't init for light0
//    ambientLight[0] = ambientLight[1] = ambientLight[2] = 0.0;
//    diffuseLight[0] = diffuseLight[1] = diffuseLight[2] = 0.0;
//    specularLight[0] = specularLight[1] = specularLight[2] = 0.0;
//    ambientLight[3] = diffuseLight[3] = specularLight[3] = 1.0;
//    exponent = 0;

//    // set material properties to defaults
//    ambientMat[0] = ambientMat[1] = ambientMat[2] = 0.2;
//    diffuseMat[0] = diffuseMat[1] = diffuseMat[2] = 0.8;
//    specularMat[0] = specularMat[1] = specularMat[2] = 0.0;
//    emissiveMat[0] = emissiveMat[1] = emissiveMat[2] = 0.0;
//    // don't forget alphas
//    ambientMat[3] = diffuseMat[3] = specularMat[3] = emissiveMat[3] = 1.0;

//    // texture state
//    attributeU = attributeV = 0;

//    // buffer state, update at each projection call
//    // initialise depth values
//    nearVal = 1.0;
//    farVal = -1.0;

//    } // constructor

//// destructor
//FakeGL::~FakeGL()
//    { // destructor
//    } // destructor

////-------------------------------------------------//
////                                                 //
//// GEOMETRIC PRIMITIVE ROUTINES                    //
////                                                 //
////-------------------------------------------------//

//// starts a sequence of geometric primitives
//void FakeGL::Begin(unsigned int PrimitiveType)
//    { // Begin()
//    primitive = PrimitiveType;
//    } // Begin()

//// ends a sequence of geometric primitives
//void FakeGL::End()
//    { // End()
//    primitive = -1;
//    } // End()

//// sets the size of a point for drawing
//void FakeGL::PointSize(float size)
//    { // PointSize()
//    pointSize = (round(size) > 0) ? round(size) : 1;
//    } // PointSize()

//// sets the width of a line for drawing purposes
//void FakeGL::LineWidth(float width)
//    { // LineWidth()
//    lineWidth = (round(width) > 0) ? round(width) : 1;
//    } // LineWidth()

////-------------------------------------------------//
////                                                 //
//// MATRIX MANIPULATION ROUTINES                    //
////                                                 //
////-------------------------------------------------//

//// set the matrix mode (i.e. which one we change)
//void FakeGL::MatrixMode(unsigned int whichMatrix)
//    { // MatrixMode()
//    matrixMode = whichMatrix;
//    } // MatrixMode()

//// pushes a matrix on the stack
//void FakeGL::PushMatrix()
//    { // PushMatrix()
//    switch (matrixMode)
//        {
//        case FAKEGL_MODELVIEW:
//            // add a copy of the current modelView matrix to the top of the stack
//            modelViewStack.push_back(modelViewStack.back());
//            break;
//        case FAKEGL_PROJECTION:
//            // add a copy of the current projection matrix to the top of the stack
//            projectionStack.push_back(projectionStack.back());
//            break;
//        default:
//            break;
//        }
//    } // PushMatrix()

//// pops a matrix off the stack
//void FakeGL::PopMatrix()
//    { // PopMatrix()
//    switch (matrixMode)
//        {
//        case FAKEGL_MODELVIEW:
//            // remove the current modelView matrix from the top of the stack
//            modelViewStack.pop_back();
//            break;
//        case FAKEGL_PROJECTION:
//            // remove the current projection matrix from the top of the stack
//            projectionStack.pop_back();
//            break;
//        default:
//            break;
//        }
//    } // PopMatrix()

//// load the identity matrix
//void FakeGL::LoadIdentity()
//    { // LoadIdentity()
//    switch (matrixMode)
//        {
//        case FAKEGL_MODELVIEW:
//            // set the current modelView matrix to identity
//            modelViewStack.back().SetIdentity();
//            break;
//        case FAKEGL_PROJECTION:
//            // set the current projection matrix to identity
//            projectionStack.back().SetIdentity();
//            break;
//        default:
//            break;
//        }
//    } // LoadIdentity()

//// multiply by a known matrix in column-major format
//void FakeGL::MultMatrixf(const float *columnMajorCoordinates)
//    { // MultMatrixf()
//    // save the current matrix for multiplication
//    Matrix4 current = modelViewStack.back();

//    // reset it in stack to not duplicate values
//    modelViewStack.back().SetZero();

//    switch (matrixMode)
//        {
//        case FAKEGL_MODELVIEW:
//            // multiply the current matrix by the given column-major formatted matrix
//            for (int row = 0; row < 4; row++)
//                for (int col = 0; col < 4; col++)
//                    for (int entry = 0; entry < 4; entry++)
//                        modelViewStack.back().coordinates[row][col] += current.coordinates[row][entry] * columnMajorCoordinates[entry + col*4];

//            break;
//        case FAKEGL_PROJECTION:
//            // multiply the current matrix by the given column-major formatted matrix
//            for (int row = 0; row < 4; row++)
//                for (int col = 0; col < 4; col++)
//                    for (int entry = 0; entry < 4; entry++)
//                        projectionStack.back().coordinates[row][col] += current.coordinates[row][entry] * columnMajorCoordinates[entry + col*4];
//            break;
//        default:
//            break;
//        }
//    } // MultMatrixf()

//// sets up a perspective projection matrix
//void FakeGL::Frustum(float left, float right, float bottom, float top, float zNear, float zFar)
//    { // Frustum()
//    // GL_INVALID_VALUE is generated if nearVal or farVal is not positive, or if left = right, or bottom = top, or near = far.
//    if ((zNear <= 0.0) || (zFar <= 0.0) || (left == right) || (bottom == top) || (zNear == zFar)) return;

//    // create a matrix for the frustum
//    Matrix4 frustum;

//    // set the entries for a perspective projection
//    frustum[0][0] = (2.0 * zNear) / (right - left);
//    frustum[1][1] = (2.0 * zNear) / (top - bottom);
//    frustum[2][0] = (right + left) / (right - left);
//    frustum[2][1] = (top + bottom) / (top - bottom);
//    frustum[2][2] = -(zFar + zNear) / (zFar - zNear);
//    frustum[3][2] = -1.0;
//    frustum[2][3] = -(2.0 * zFar * zNear) / (zFar - zNear);
//    // other entries are 0

//    // use the mult matrix method to multiply current matrix
//    MultMatrixf(frustum.columnMajor().coordinates);

//    // also update the near and far values for depth testing
//    nearVal = zNear;
//    farVal = zFar;
//    } // Frustum()

//// sets an orthographic projection matrix
//void FakeGL::Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
//    { // Ortho()
//    // GL_INVALID_VALUE is generated if left = right, or bottom = top, or near = far.
//    if ((left == right) || (bottom == top) || (zNear == zFar)) return;

//    // create a matrix for the ortho
//    Matrix4 ortho;

//    // set the entries for an orthogonal projection
//    ortho[0][0] = 2.0 / (right - left);
//    ortho[1][1] = 2.0 / (top - bottom);
//    ortho[2][2] = - 2.0 / (zFar - zNear);
//    ortho[3][0] = -(right + left) / (right - left);
//    ortho[3][1] = -(top + bottom) / (top - bottom);
//    ortho[3][2] = -(zFar + zNear) / (zFar - zNear);
//    ortho[3][3] = 1.0;
//    // other entries are 0

//    // use the mult matrix method to multiply current matrix
//    MultMatrixf(ortho.columnMajor().coordinates);

//    // also update the near and far values for depth testing
//    nearVal = zNear;
//    farVal = zFar;
//    } // Ortho()

//// rotate the matrix
//void FakeGL::Rotatef(float angle, float axisX, float axisY, float axisZ)
//    { // Rotatef()
//    // general rotation matrix
//    Cartesian3 axis = Cartesian3(axisX, axisY, axisZ);

//    // normalise axis if not unit
//    if(axis.length() > 1.0)
//        axis = axis.unit();

//    // get the values we will use to build the right rotation matrix
//    float c = cos((angle * 180.0) / PI);
//    float s = sin((angle * 180.0) / PI);
//    float x = axis.x;
//    float y = axis.y;
//    float z = axis.z;

//    // create a matrix for rotation
//    Matrix4 rotation;

//    // set the entries for a rotation around an arbitrary axis
//    rotation[0][0] = x * x * (1 - c) + c;
//    rotation[0][1] = x * y * (1 - c) - z * s;
//    rotation[0][2] = x * z * (1 - c) + x * y;
//    rotation[1][0] = y * x * (1 - c) + z * s;
//    rotation[1][1] = y * y * (1 - c) + c ;
//    rotation[1][2] = y * z * (1 - c) - x * s;
//    rotation[2][0] = z * x * (1 - c) - y * s;
//    rotation[2][0] = z * y * (1 - c) + x * s;
//    rotation[2][0] = z * z * (1 - c) + c;
//    rotation[3][3] = 1.0;

//    // use the mult matrix method to multiply current matrix
//    MultMatrixf(rotation.columnMajor().coordinates);

//    } // Rotatef()

//// scale the matrix
//void FakeGL::Scalef(float xScale, float yScale, float zScale)
//    { // Scalef()
//    // create a matrix for scaling
//    Matrix4 scale;

//    // set entries for scaling along x, y and z
//    scale[0][0] = xScale;
//    scale[1][1] = yScale;
//    scale[2][2] = zScale;
//    scale[3][3] = 1.0;

//    // multiply current matrix by scale
//    MultMatrixf(scale.columnMajor().coordinates);

//    } // Scalef()

//// translate the matrix
//void FakeGL::Translatef(float xTranslate, float yTranslate, float zTranslate)
//    { // Translatef()
//    // create matrix for translation
//    Matrix4 translate;

//    // set entries for a translation along a vector (x, y, z)
//    for (int row = 0; row < 4; row++)
//            translate[row][row] = 1.0;
//    translate[0][3] = xTranslate;
//    translate[1][3] = yTranslate;
//    translate[2][3] = zTranslate;

//    // multiply current matrix by scale
//    MultMatrixf(translate.columnMajor().coordinates);
//    } // Translatef()

//// sets the viewport
//void FakeGL::Viewport(int x, int y, int width, int height)
//    { // Viewport()
//    // GL_INVALID_VALUE is generated if either width or height is negative.
//    if ((width < 0.0) || (height < 0.0)) return;

//    // get the smallest value between width and height
//    viewPortSize = (width < height) ? width : height;

//    // set pixel origin accordingly
//    xPixelOrigin = x + (width / 2.0 - viewPortSize / 2.0);
//    yPixelOrigin = y + (height / 2.0 - viewPortSize / 2.0);
//    } // Viewport()

////-------------------------------------------------//
////                                                 //
//// VERTEX ATTRIBUTE ROUTINES                       //
////                                                 //
////-------------------------------------------------//

//// sets colour with floating point
//void FakeGL::Color3f(float red, float green, float blue)
//    { // Color3f()
//    // clamp the values in range [0, 1]
//    float clampedR = (red < 0.0)   ? 0.0 : (1.0 < red)   ? 1.0 : red;
//    float clampedG = (green < 0.0) ? 0.0 : (1.0 < green) ? 1.0 : green;
//    float clampedB = (blue < 0.0)  ? 0.0 : (1.0 < blue)  ? 1.0 : blue;

//    // set the current colour, converts to char
//    attributeColour.red = clampedR * 255;
//    attributeColour.green = clampedG * 255;
//    attributeColour.blue = clampedB * 255;
//    } // Color3f()

//// sets material properties
//void FakeGL::Materialf(unsigned int parameterName, const float parameterValue)
//    { // Materialf()
//    if (parameterName & FAKEGL_AMBIENT)
//        ambientMat[0] = ambientMat[1] = ambientMat[2] = parameterValue;

//    if (parameterName & FAKEGL_DIFFUSE)
//        diffuseMat[0] = diffuseMat[1] = diffuseMat[2] = parameterValue;

//    if (parameterName & FAKEGL_SPECULAR)
//        specularMat[0] = specularMat[1] = specularMat[2] = parameterValue;

//    if (parameterName & FAKEGL_EMISSION)
//        emissiveMat[0] = emissiveMat[1] = emissiveMat[2] = parameterValue;

//    if (parameterName & FAKEGL_SHININESS)
//        exponent = parameterValue;
//    } // Materialf()

//void FakeGL::Materialfv(unsigned int parameterName, const float *parameterValues)
//    { // Materialfv()
//    // copy parameter values into corresponding arrays
//    if (parameterName & FAKEGL_AMBIENT)
//        {
//        ambientMat[0] = parameterValues[0];
//        ambientMat[1] = parameterValues[1];
//        ambientMat[2] = parameterValues[2];
//        ambientMat[3] = parameterValues[3];
//        }
//    if (parameterName & FAKEGL_DIFFUSE)
//        {
//        diffuseMat[0] = parameterValues[0];
//        diffuseMat[1] = parameterValues[1];
//        diffuseMat[2] = parameterValues[2];
//        diffuseMat[3] = parameterValues[3];
//        }
//    if (parameterName & FAKEGL_SPECULAR)
//        {
//        specularMat[0] = parameterValues[0];
//        specularMat[1] = parameterValues[1];
//        specularMat[2] = parameterValues[2];
//        specularMat[3] = parameterValues[3];
//        }
//    if (parameterName & FAKEGL_EMISSION)
//        {
//        emissiveMat[0] = parameterValues[0];
//        emissiveMat[1] = parameterValues[1];
//        emissiveMat[2] = parameterValues[2];
//        emissiveMat[3] = parameterValues[3];
//        }
//    } // Materialfv()

//// sets the normal vector
//void FakeGL::Normal3f(float x, float y, float z)
//    { // Normal3f()
//    // assume normal is unit
//    attributeNormal.x = x;
//    attributeNormal.y = y;
//    attributeNormal.z = z;
//    } // Normal3f()

//// sets the texture coordinates
//void FakeGL::TexCoord2f(float u, float v)
//    { // TexCoord2f()
//    attributeU = u;
//    attributeV = v;
//    } // TexCoord2f()

//// sets the vertex & launches it down the pipeline
//void FakeGL::Vertex3f(float x, float y, float z)
//    { // Vertex3f()

//    // create the new attribute vertex
//    vertexWithAttributes vertex;

//    // set its attributes
//    vertex.position = Homogeneous4(x, y, z, 1);
//    vertex.colour = attributeColour;
//    vertex.normal = Homogeneous4(attributeNormal);
//    vertex.normal.w = 0.0;
//    vertex.u = attributeU;
//    vertex.v = attributeV;
//    vertex.ambient = ambientMat;
//    vertex.diffuse = diffuseMat;
//    vertex.specular = specularMat;
//    vertex.emissive = emissiveMat;
//    vertex.exponent = exponent;


//    //std::cout << vertex.u << " " << vertex.v << std::endl;

//    // push it to the back of the queue
//    vertexQueue.push_back(vertex);

//    // launch vertex down pipeline starting at vertexQueue
//    TransformVertex();

//    // trigger the rasterisation stage of the pipeline
//    if (RasterisePrimitive())
//        // then we need to process the fragments in the queue, here for a single primitive
//        while(!fragmentQueue.empty())
//            ProcessFragment();

//    } // Vertex3f()

////-------------------------------------------------//
////                                                 //
//// STATE VARIABLE ROUTINES                         //
////                                                 //
////-------------------------------------------------//

//// disables a specific flag in the library
//void FakeGL::Disable(unsigned int property)
//    { // Disable()
//    // switch desired property off by setting it to off (0)
//    switch (property)
//        {
//        case FAKEGL_LIGHTING:
//            lighting = 0;
//            break;
//        case FAKEGL_TEXTURE_2D:
//            texture = 0;
//            break;
//        case FAKEGL_DEPTH_TEST:
//            depthTest = 0;
//            break;
//        case FAKEGL_PHONG_SHADING:
//            phongShading = 0;
//            break;
//        default:
//            break;
//        }
//    } // Disable()

//// enables a specific flag in the library
//void FakeGL::Enable(unsigned int property)
//    { // Enable()
//    // switch desired property off by setting it to on (1)
//    switch (property)
//        {
//        case FAKEGL_LIGHTING:
//            lighting = 1;
//            break;
//        case FAKEGL_TEXTURE_2D:
//            texture = 1;
//            break;
//        case FAKEGL_DEPTH_TEST:
//            depthTest = 1;
//            break;
//        case FAKEGL_PHONG_SHADING:
//            phongShading = 1;
//            break;
//        default:
//            break;
//        }
//    } // Enable()

////-------------------------------------------------//
////                                                 //
//// LIGHTING STATE ROUTINES                         //
////                                                 //
////-------------------------------------------------//

//// sets properties for the one and only light
//void FakeGL::Light(int parameterName, const float *parameterValues)
//    { // Light()
//    if(parameterName & FAKEGL_POSITION)
//        {
//        // place the light at the desired position and transform it by the current modelview matrix


//        Homogeneous4 transformedLight(parameterValues[0], parameterValues[1], parameterValues[2], parameterValues[3]);
//        std::cout<<transformedLight<<std::endl;
//        lightPosition = modelViewStack.back() * transformedLight;
//        std::cout<<transformedLight<<" "<<lightPosition<<std::endl;

//        }
//    if(parameterName & FAKEGL_AMBIENT)
//        {
//        // we don't worry about integer inputs and mapping linearly to most and least representable values, floats are mapped directly
//        ambientLight[0] = parameterValues[0];
//        ambientLight[1] = parameterValues[1];
//        ambientLight[2] = parameterValues[2];
//        ambientLight[3] = parameterValues[3];
//        }
//    if(parameterName & FAKEGL_DIFFUSE)
//        {
//        diffuseLight[0] = parameterValues[0];
//        diffuseLight[1] = parameterValues[1];
//        diffuseLight[2] = parameterValues[2];
//        diffuseLight[3] = parameterValues[3];
//        }
//    if(parameterName & FAKEGL_SPECULAR)
//        {
//        specularLight[0] = parameterValues[0];
//        specularLight[1] = parameterValues[1];
//        specularLight[2] = parameterValues[2];
//        specularLight[3] = parameterValues[3];
//        }
//    } // Light()
////-------------------------------------------------//
////                                                 //
//// TEXTURE PROCESSING ROUTINES                     //
////                                                 //
//// Note that we only allow one texture             //
//// so glGenTexture & glBindTexture aren't needed   //
////                                                 //
////-------------------------------------------------//

//// sets whether textures replace or modulate
//void FakeGL::TexEnvMode(unsigned int textureMode)
//    { // TexEnvMode()
//    if (textureMode == FAKEGL_MODULATE)
//        texMode = FAKEGL_MODULATE;
//    else if (textureMode == FAKEGL_REPLACE)
//        texMode = FAKEGL_REPLACE;
//    } // TexEnvMode()

//// sets the texture image that corresponds to a given ID
//void FakeGL::TexImage2D(const RGBAImage &textureImage)
//    { // TexImage2D()
//    // resize image buffer,swapping height and width to match OpenGL output
//    textureData.Resize(textureImage.height, textureImage.width);

//    // next copy values in input to texture data, iterating row-wise through
//    // input image but assigning to swapped row and col in texture keeps the same origin whilst mirroring the image
//    for (int row = 0; row < textureImage.height; row++)
//        for (int col = 0; col < textureImage.width; col++)
//            textureData[col][row] = textureImage[row][col];
//    } // TexImage2D()

////-------------------------------------------------//
////                                                 //
//// FRAME BUFFER ROUTINES                           //
////                                                 //
////-------------------------------------------------//

//// clears the frame buffer
//void FakeGL::Clear(unsigned int mask)
//    { // Clear()
//    // set the frame buffer to be the desired colour stored in clearColour
//    if(mask & FAKEGL_COLOR_BUFFER_BIT)
//        for(unsigned int row = 0; row < frameBuffer.height; row++)
//            for(unsigned int col = 0; col < frameBuffer.width; col++)
//                frameBuffer[row][col] = clearColour;

//    // set the depth buffer to be the desired colour stored in depth
//    if(mask & FAKEGL_DEPTH_BUFFER_BIT)
//        for(unsigned int row = 0; row < depthBuffer.height; row++)
//            for(unsigned int col = 0; col < depthBuffer.width; col++)
//                depthBuffer[row][col] = depthColour;

//    } // Clear()

//// sets the clear colour for the frame buffer
//void FakeGL::ClearColor(float red, float green, float blue, float alpha)
//    { // ClearColor()
//    // clamp rgba values
//    float clampedR = (red < 0.0)   ? 0.0 : (1.0 < red)   ? 1.0 : red;
//    float clampedG = (green < 0.0) ? 0.0 : (1.0 < green) ? 1.0 : green;
//    float clampedB = (blue < 0.0)  ? 0.0 : (1.0 < blue)  ? 1.0 : blue;
//    float clampedA = (alpha < 0.0) ? 0.0 : (1.0 < alpha) ? 1.0 : alpha;

//    // input are values ranging from 0 to 1
//    clearColour = RGBAValue(clampedR * 255, clampedG * 255, clampedB * 255, clampedA * 255);
//    } // ClearColor()

////-------------------------------------------------//
////                                                 //
//// MAJOR PROCESSING ROUTINES                       //
////                                                 //
////-------------------------------------------------//

//// transform one vertex & shift to the raster queue
//void FakeGL::TransformVertex()
//    { // TransformVertex()
//    // get the vertex at the front of the queue
//    vertexWithAttributes vertex = vertexQueue.front();

//    //std::cout << "vertex ---------- " << std::endl;

//    // convert to view space (model view)
//    Homogeneous4 coordVCS = modelViewStack.back() * vertex.position;

//    //std::cout << coordVCS << std::endl;

//    // convert to clipping space space (projection)
//    Homogeneous4 coordCS = projectionStack.back() * coordVCS;

//    //std::cout << coordCS << std::endl;

//    // convert to normalised device coordinates (divide by w)
//    Cartesian3 coordNDS = coordCS.Point();

//    //std::cout << coordNDS << std::endl;

//    // convert to device coordinates (screen space), keeping the z in view space
//    Cartesian3 coordDCS(
//        round(coordNDS.x * (viewPortSize / 2.0) + (viewPortSize / 2.0) + xPixelOrigin),
//        round(coordNDS.y * (viewPortSize / 2.0) + (viewPortSize / 2.0) + yPixelOrigin),
//        coordVCS.z);

//    // create the screen vertex
//    screenVertexWithAttributes screenVertex;

//    // set its attributes with current state information
//    screenVertex.position = coordDCS;
//    screenVertex.normal = (modelViewStack.back() * vertex.normal).Vector();

//    // material properties
//    screenVertex.colour = vertex.colour;
//    screenVertex.ambient = vertex.ambient;
//    screenVertex.diffuse = vertex.specular;
//    screenVertex.specular = vertex.diffuse;
//    screenVertex.emissive = vertex.emissive;
//    screenVertex.exponent = vertex.exponent;

//    // texture properties
//    screenVertex.u = vertex.u;
//    screenVertex.v = vertex.v;

//    // add to the screen vertex to raster queue
//    rasterQueue.push_back(screenVertex);

//    // now remove vertex from vertex queue
//    vertexQueue.pop_front();

//    } // TransformVertex()

//// rasterise a single primitive if there are enough vertices on the queue
//bool FakeGL::RasterisePrimitive()
//    { // RasterisePrimitive()
//    if(rasterQueue.empty())
//        return false;

//    // determine what type of primitive to rasterise
//    switch(primitive)
//        {
//        case FAKEGL_POINTS:
//            // not enough vertices for the primitive
//            if(rasterQueue.size() < 1)
//                return false;
//            // rasterise it
//            RasterisePoint(*rasterQueue.begin());
//            // remove it
//            rasterQueue.pop_front();
//            return true;

//        case FAKEGL_LINES:
//            // not enough vertices for the primitive
//            if(rasterQueue.size() < 2)
//                return false;
//            // rasterise them
//            RasteriseLineSegment(*rasterQueue.begin(), *(rasterQueue.begin() + 1));
//            // remove them
//            rasterQueue.erase(rasterQueue.begin(), rasterQueue.begin() + 2);
//            return true;

//        case FAKEGL_TRIANGLES:
//            // not enough vertices for the primitive
//            if(rasterQueue.size() < 3)
//                return false;
//            // rasterise them
//            RasteriseTriangle(*rasterQueue.begin(), *(rasterQueue.begin() + 1), *(rasterQueue.begin() + 2));
//            // remove them
//            rasterQueue.erase(rasterQueue.begin(), rasterQueue.begin() + 3);
//            return true;

//        default:
//            return false;
//        }
//    } // RasterisePrimitive()

//// rasterises a single point
//void FakeGL::RasterisePoint(screenVertexWithAttributes &vertex0)
//    { // RasterisePoint()
//    // create a bounding box for the point
//    float minX = vertex0.position.x - (pointSize / 2.0), maxX = vertex0.position.x + (pointSize / 2.0);
//    float minY = vertex0.position.y - (pointSize / 2.0), maxY = vertex0.position.y + (pointSize / 2.0);

//    // create a fragment for reuse
//    fragmentWithAttributes rasterFragment;

//    // loop over all fragments within the bounding box
//    for (rasterFragment.row = minY; rasterFragment.row <= maxY; rasterFragment.row++)
//        { // per row
//        // clip fragment if row values outside frame buffer
//        if (rasterFragment.row < 0) continue;
//        if (rasterFragment.row >= frameBuffer.height) continue;
//        for (rasterFragment.col = minX; rasterFragment.col <= maxX; rasterFragment.col++)
//            { // per pixel
//            // clip fragment if col values outside frame buffer
//            if (rasterFragment.col < 0) continue;
//            if (rasterFragment.col >= frameBuffer.width) continue;

//            // fragment distance to vertex centre
//            Cartesian3 frag2Vertex(vertex0.position.x - rasterFragment.col, vertex0.position.y - rasterFragment.row , 0.0);

//            // if pixel centre is not within pointsize (radius) then skip
//            if (frag2Vertex.dot(frag2Vertex) >= pointSize * pointSize)
//                continue;

//            // otherwise we set its colour to the vertex's
//            rasterFragment.colour = vertex0.colour;

//            // we set the depth (ranging between 0 and 1 using near and far from projection)
//            rasterFragment.depth = vertex0.position.z;

//            // then we add the fragment to the queue
//            fragmentQueue.push_back(rasterFragment);
//            } // per pixel
//        } // per row
//    } // RasterisePoint()

//// rasterises a single line segment
//void FakeGL::RasteriseLineSegment(screenVertexWithAttributes &vertex0, screenVertexWithAttributes &vertex1)
//    { // RasteriseLineSegment()

//    // create a raster fragment
//    screenVertexWithAttributes interpolatedVertex;

//    // keep a copy of point size
//    float pSize = pointSize;
//    // cheat and set point size to linewidth
//    PointSize(lineWidth / 2.0);

//    // the max number of pixels along either width or height
//    float step = (frameBuffer.height < frameBuffer.width) ? frameBuffer.width : frameBuffer.height;
//    // iterate over the parameterised line between v0 and v1 using t (step size of 0.0015)
//    for(float t = 0.0; t <= 1.0; t += 1 / (2 * step))
//        {
//        // interpolate the position of the point
//        interpolatedVertex.position = t*vertex1.position + (1-t)*vertex0.position;
//        // interpolate the colour value
//        interpolatedVertex.colour = t*vertex1.colour + (1-t)*vertex0.colour;

//        // call the rasterise point method for the interpolated vertex
//        RasterisePoint(interpolatedVertex);
//        }

//    // set point size to original value
//    pointSize = pSize;
//    } // RasteriseLineSegment()

//// rasterises a single triangle
//void FakeGL::RasteriseTriangle(screenVertexWithAttributes &vertex0, screenVertexWithAttributes &vertex1, screenVertexWithAttributes &vertex2)
//    { // RasteriseTriangle()
//    // compute a bounding box that starts inverted to frame size
//    // clipping will happen in the raster loop proper
//    float minX = frameBuffer.width, maxX = 0.0;
//    float minY = frameBuffer.height, maxY = 0.0;

//    // test against all vertices
//    if (vertex0.position.x < minX) minX = vertex0.position.x;
//    if (vertex0.position.x > maxX) maxX = vertex0.position.x;
//    if (vertex0.position.y < minY) minY = vertex0.position.y;
//    if (vertex0.position.y > maxY) maxY = vertex0.position.y;

//    if (vertex1.position.x < minX) minX = vertex1.position.x;
//    if (vertex1.position.x > maxX) maxX = vertex1.position.x;
//    if (vertex1.position.y < minY) minY = vertex1.position.y;
//    if (vertex1.position.y > maxY) maxY = vertex1.position.y;

//    if (vertex2.position.x < minX) minX = vertex2.position.x;
//    if (vertex2.position.x > maxX) maxX = vertex2.position.x;
//    if (vertex2.position.y < minY) minY = vertex2.position.y;
//    if (vertex2.position.y > maxY) maxY = vertex2.position.y;

//    // now for each side of the triangle, compute the line vectors
//    Cartesian3 vector01 = vertex1.position - vertex0.position;
//    Cartesian3 vector12 = vertex2.position - vertex1.position;
//    Cartesian3 vector20 = vertex0.position - vertex2.position;

//    // now compute the line normal vectors
//    Cartesian3 normal01(-vector01.y, vector01.x, 0.0);
//    Cartesian3 normal12(-vector12.y, vector12.x, 0.0);
//    Cartesian3 normal20(-vector20.y, vector20.x, 0.0);

//    // we don't need to normalise them, because the square roots will cancel out in the barycentric coordinates
//    float lineConstant01 = normal01.dot(vertex0.position);
//    float lineConstant12 = normal12.dot(vertex1.position);
//    float lineConstant20 = normal20.dot(vertex2.position);

//    // and compute the distance of each vertex from the opposing side
//    float distance0 = normal12.dot(vertex0.position) - lineConstant12;
//    float distance1 = normal20.dot(vertex1.position) - lineConstant20;
//    float distance2 = normal01.dot(vertex2.position) - lineConstant01;

//    // if any of these are zero, we will have a divide by zero error
//    // but notice that if they are zero, the vertices are collinear in projection and the triangle is edge on
//    // we can render that as a line, but the better solution is to render nothing.  In a surface, the adjacent
//    // triangles will eventually take care of it
//    if ((distance0 == 0) || (distance1 == 0) || (distance2 == 0))
//        return;

//    // create a fragment for reuse
//    fragmentWithAttributes rasterFragment;

//    // get the normals of each vertex and normalise to accomodate for scaling
//    Cartesian3 normal0 = vertex0.normal.unit();
//    Cartesian3 normal1 = vertex1.normal.unit();
//    Cartesian3 normal2 = vertex2.normal.unit();

//    // light direction
//    Cartesian3 vl;
//    if (lightPosition.w != 0)
//        vl = lightPosition.Vector() / lightPosition.w;
//    else
//        vl = lightPosition.Vector();

//    // compute the light intensity at each vertex
//    float v0I[4], v1I[4], v2I[4] = {0.0, 0.0, 0.0, 0.0};
//    // only bother computing the light intensity at each vertex when lighting is enabled and we don't want phong shading
//    if (lighting && !phongShading)
//    {
//        // vertex 0
//        // cosine may be negative which causes underflow as we store values as unsigned int (-1 = 2^32)
//        float cosDif = normal0.dot(vl) / vl.length();
//        cosDif = (cosDif > 0) ? cosDif : 0;
//        float cosSpec = normal0.dot(vl / 2.0) / (vl / 2.0).length();
//        cosSpec = (cosSpec > 0) ? cosSpec : 0;
//        // loop over each colour channel (RGBA)
//        for (unsigned int channel = 0; channel < 4; channel++)
//            v0I[channel] =
//            ambientLight[channel]*vertex0.ambient[channel] +
//            diffuseLight[channel]*vertex0.diffuse[channel]*cosDif +
//            specularLight[channel]*vertex0.specular[channel]*pow(cosSpec, vertex0.exponent) +
//            vertex0.emissive[channel];
//        // repeat process for other vertices
//        // vertex 1
//        cosDif = normal1.dot(vl) / vl.length();
//        cosDif = (cosDif > 0) ? cosDif : 0;
//        cosSpec = normal1.dot(vl / 2.0) / (vl / 2.0).length();
//        cosSpec = (cosSpec > 0) ? cosSpec : 0;

//        for (unsigned int channel = 0; channel < 4; channel++)
//            v1I[channel] =
//            ambientLight[channel]*vertex1.ambient[channel] +
//            diffuseLight[channel]*vertex1.diffuse[channel]*cosDif +
//            specularLight[channel]*vertex1.specular[channel]*pow(cosSpec, vertex1.exponent) +
//            vertex1.emissive[channel];

//        // vertex 2
//        cosDif = normal2.dot(vl) / vl.length();
//        cosDif = (cosDif > 0) ? cosDif : 0;
//        cosSpec = normal2.dot(vl / 2.0) / (vl / 2.0).length();
//        cosSpec = (cosSpec > 0) ? cosSpec : 0;

//        for (unsigned int channel = 0; channel < 4; channel++)
//            v2I[channel] =
//            ambientLight[channel]*vertex2.ambient[channel] +
//            diffuseLight[channel]*vertex2.diffuse[channel]*cosDif +
//            specularLight[channel]*vertex2.specular[channel]*pow(cosSpec, vertex2.exponent) +
//            vertex2.emissive[channel];
//    }

//    // loop through the pixels in the bounding box
//    for (rasterFragment.row = minY; rasterFragment.row <= maxY; rasterFragment.row++)
//        { // per row
//        // this is here so that clipping works correctly
//        if (rasterFragment.row < 0) continue;
//        if (rasterFragment.row >= frameBuffer.height) continue;
//        for (rasterFragment.col = minX; rasterFragment.col <= maxX; rasterFragment.col++)
//            { // per pixel
//            // this is also for correct clipping
//            if (rasterFragment.col < 0) continue;
//            if (rasterFragment.col >= frameBuffer.width) continue;

//            // the pixel in cartesian format
//            Cartesian3 pixel(rasterFragment.col, rasterFragment.row, 0.0);

//            // right - we have a pixel inside the frame buffer AND the bounding box
//            // note we *COULD* compute gamma = 1.0 - alpha - beta instead
//            float alpha = (normal12.dot(pixel) - lineConstant12) / distance0;
//            float beta = (normal20.dot(pixel) - lineConstant20) / distance1;
//            float gamma = (normal01.dot(pixel) - lineConstant01) / distance2;

//            // now perform the half-plane test
//            if ((alpha < 0.0) || (beta < 0.0) || (gamma < 0.0))
//                continue;

//            // compute the depth as an interpolated sum of the depth values of the three vertices
//            float fragZ = alpha * vertex0.position.z + beta * vertex1.position.z + gamma * vertex2.position.z;
//            // we then make the fragment range between 0 and 1 using near and far from projection
//            //rasterFragment.depth = (fragZ - nearVal) / (farVal - nearVal);
//            rasterFragment.depth = (farVal - fragZ) / (farVal - nearVal);

//            // clip the fragments out of clip space pixels here to save compute time
//            if (rasterFragment.depth > 1 || rasterFragment.depth < 0)
//                continue;

//            // compute fragment colour
//            RGBAValue interpCol = alpha * vertex0.colour + beta * vertex1.colour + gamma * vertex2.colour;

//            if(lighting)
//                {
//                // custom gamma correction, tested for my laptop brightens the scene up a little
//                float scalar = 44.0;
//                float exponent = 1.065;

//                // where the color values are stored and initialised at 0;
//                float R, G, B, A = 0.0;

//                // per fragment intensity
//                if (phongShading)
//                    {
//                    // compute light intensity per fragment, need to interpolate normals
//                    Cartesian3 fragNormal = alpha * normal0 + beta * normal1 + gamma *  normal2;

//                    // interpolate fragment attributes
//                    float fragExponent = alpha * vertex0.exponent + beta * vertex1.exponent + gamma * vertex2.exponent;
//                    float fragAmbient[4], fragDiffuse[4], fragSpecular[4], fragEmissive[4];
//                    // do this for each RGBA channel
//                    for (unsigned int i = 0; i < 4; i++)
//                        {
//                        fragAmbient[i] = alpha * vertex0.ambient[i] + beta * vertex1.ambient[i] + gamma * vertex2.ambient[i];
//                        fragDiffuse[i] = alpha * vertex0.diffuse[i] + beta * vertex1.diffuse[i] + gamma * vertex2.diffuse[i];
//                        fragSpecular[i] = alpha * vertex0.specular[i] + beta * vertex1.specular[i] + gamma * vertex2.specular[i];
//                        fragEmissive[i] = alpha * vertex0.emissive[i] + beta * vertex1.emissive[i] + gamma * vertex2.emissive[i];
//                        }

//                    // compute cosines
//                    float cosDif = fragNormal.unit().dot(vl) / vl.length();
//                    cosDif = (cosDif > 0) ? cosDif : 0;
//                    float cosSpec = fragNormal.unit().dot(vl / 2.0) / (vl / 2.0).length();
//                    cosSpec = (cosSpec > 0) ? cosSpec : 0;

//                    // compute light intensity at the fragment for each channel
//                    float fragI[4];
//                    for (unsigned int channel = 0; channel < 4; channel++)
//                        fragI[channel] =
//                        ambientLight[channel]*fragAmbient[channel] +
//                        diffuseLight[channel]*fragDiffuse[channel]*cosDif +
//                        specularLight[channel]*fragSpecular[channel]*pow(cosSpec, fragExponent) +
//                        fragEmissive[channel];

//                    // compute colour for each of the fragment's channels
//                    R = pow((alpha * vertex0.colour.red * fragI[0] + beta * vertex1.colour.red * fragI[0] + gamma * vertex2.colour.red * fragI[0]), exponent) + scalar;
//                    G = pow((alpha * vertex0.colour.green * fragI[1] + beta * vertex1.colour.green * fragI[1] + gamma * vertex2.colour.green * fragI[1]), exponent) + scalar;
//                    B = pow((alpha * vertex0.colour.blue * fragI[2] + beta * vertex1.colour.blue * fragI[2] + gamma * vertex2.colour.blue * fragI[2]), exponent) + scalar;
//                    A = pow((alpha * vertex0.colour.alpha * fragI[3] + beta * vertex1.colour.alpha * fragI[3] + gamma * vertex2.colour.alpha * fragI[3]), exponent) + scalar;
//                    }
//                else
//                    {
//                    // interpolate fragment's intensity using the three vertices' intensity
//                    R = pow((alpha * vertex0.colour.red * v0I[0] + beta * vertex1.colour.red * v1I[0] + gamma * vertex2.colour.red * v2I[0]), exponent) + scalar;
//                    G = pow((alpha * vertex0.colour.green * v0I[1] + beta * vertex1.colour.green * v1I[1] + gamma * vertex2.colour.green * v2I[1]), exponent) + scalar;
//                    B = pow((alpha * vertex0.colour.blue * v0I[2] + beta * vertex1.colour.blue * v1I[2] + gamma * vertex2.colour.blue * v2I[2]), exponent) + scalar;
//                    A = pow((alpha * vertex0.colour.alpha * v0I[3] + beta * vertex1.colour.alpha * v1I[3] + gamma * vertex2.colour.alpha * v2I[3]), exponent) + scalar;
//                    }
//                // assign the computed colour channels to the fragment
//                rasterFragment.colour = RGBAValue(R, G, B, A);
//                }
//            else
//                rasterFragment.colour = interpCol;

//            // compute interpolated texture coordinates and set colour
//            if (texture)
//                {
//                // get the coordinates and implicit cast to int
//                int interpU = (alpha * vertex0.u + beta * vertex1.u + gamma * vertex2.u) * textureData.height;
//                int interpV = (alpha * vertex0.v + beta * vertex1.v + gamma * vertex2.v) * textureData.width;

//                // if modulate then multiply by current fragment colour, other wise replace colour
//                if (texMode == FAKEGL_MODULATE)
//                    rasterFragment.colour = rasterFragment.colour.modulate(textureData[interpU][interpV]);
//                else if (texMode == FAKEGL_REPLACE)
//                    rasterFragment.colour = textureData[interpU][interpV];
//                }

//            // now we add it to the queue for fragment processing
//            fragmentQueue.push_back(rasterFragment);
//            } // per pixel
//        } // per row
//    } // RasteriseTriangle()

//// process a single fragment
//void FakeGL::ProcessFragment()
//    { // ProcessFragment()

//    // get fragment data from the front of the queue
//    fragmentWithAttributes fragment = fragmentQueue.front();
//    // where we do the depth checking? ... before we set the fragment in the frame buffer
//    if(depthTest)
//        {
//        // remember that RGBA values are char ranging from 0 to 255... so convert depth ([0, 1]) by multiplying by 255
//        if (fragment.depth * 255 <= (float)depthBuffer[fragment.row][fragment.col].alpha)
//            {
//            // first we update the frame buffer
//            frameBuffer[fragment.row][fragment.col].red = fragment.colour.red;
//            frameBuffer[fragment.row][fragment.col].green = fragment.colour.green;
//            frameBuffer[fragment.row][fragment.col].blue = fragment.colour.blue;
//            frameBuffer[fragment.row][fragment.col].alpha = fragment.colour.alpha;

//            // and we don't forget to update the depthBuffer (casts to unsigned char)
//            depthBuffer[fragment.row][fragment.col].alpha = fragment.depth * 255;
//            //std::cout << (int)(fragment.depth * 255) << std::endl;
//            }
//        }
//    else
//        {
//        // simplest version will just set the fragment colour in the frame buffer
//        frameBuffer[fragment.row][fragment.col].red = fragment.colour.red;
//        frameBuffer[fragment.row][fragment.col].green = fragment.colour.green;
//        frameBuffer[fragment.row][fragment.col].blue = fragment.colour.blue;
//        frameBuffer[fragment.row][fragment.col].alpha = fragment.colour.alpha;
//        }

//    // remove fragment from the queue
//    fragmentQueue.pop_front();
//    } // ProcessFragment()

//// standard routine for dumping the entire FakeGL context (except for texture / image)
//std::ostream &operator << (std::ostream &outStream, FakeGL &fakeGL)
//    { // operator <<
//    outStream << "=========================" << std::endl;
//    outStream << "Dumping FakeGL Context   " << std::endl;
//    outStream << "=========================" << std::endl;


//    outStream << "-------------------------" << std::endl;
//    outStream << "Vertex Queue:            " << std::endl;
//    outStream << "-------------------------" << std::endl;
//    for (auto vertex = fakeGL.vertexQueue.begin(); vertex < fakeGL.vertexQueue.end(); vertex++)
//        { // per matrix
//        outStream << "Vertex " << vertex - fakeGL.vertexQueue.begin() << std::endl;
//        outStream << *vertex;
//        } // per matrix


//    outStream << "-------------------------" << std::endl;
//    outStream << "Raster Queue:            " << std::endl;
//    outStream << "-------------------------" << std::endl;
//    for (auto vertex = fakeGL.rasterQueue.begin(); vertex < fakeGL.rasterQueue.end(); vertex++)
//        { // per matrix
//        outStream << "Vertex " << vertex - fakeGL.rasterQueue.begin() << std::endl;
//        outStream << *vertex;
//        } // per matrix


//    outStream << "-------------------------" << std::endl;
//    outStream << "Fragment Queue:          " << std::endl;
//    outStream << "-------------------------" << std::endl;
//    for (auto fragment = fakeGL.fragmentQueue.begin(); fragment < fakeGL.fragmentQueue.end(); fragment++)
//        { // per matrix
//        outStream << "Fragment " << fragment - fakeGL.fragmentQueue.begin() << std::endl;
//        outStream << *fragment;
//        } // per matrix


//    return outStream;
//    } // operator <<

//// subroutines for other classes
//std::ostream &operator << (std::ostream &outStream, vertexWithAttributes &vertex)
//    { // operator <<
//    std::cout << "Vertex With Attributes" << std::endl;
//    std::cout << "Position:   " << vertex.position << std::endl;
//    std::cout << "Colour:     " << vertex.colour << std::endl;

//    return outStream;
//    } // operator <<

//std::ostream &operator << (std::ostream &outStream, screenVertexWithAttributes &vertex)
//    { // operator <<
//    std::cout << "Screen Vertex With Attributes" << std::endl;
//    std::cout << "Position:   " << vertex.position << std::endl;
//    std::cout << "Colour:     " << vertex.colour << std::endl;

//    return outStream;
//    } // operator <<

//std::ostream &operator << (std::ostream &outStream, fragmentWithAttributes &fragment)
//    { // operator <<
//    std::cout << "Fragment With Attributes" << std::endl;
//    std::cout << "Row:        " << fragment.row << std::endl;
//    std::cout << "Col:        " << fragment.col << std::endl;
//    std::cout << "Colour:     " << fragment.colour << std::endl;

//    return outStream;
//    } // operator <<








