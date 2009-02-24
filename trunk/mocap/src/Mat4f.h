#ifndef _MAT4F_H
#define _MAT4F_H

class Mat4f {
  
    private:
        float data[16];  
        
  
    public:
        Mat4f();
        Mat4f(float val[16]);
        Mat4f(const Mat4f &copie);
        ~Mat4f();
        
        float* operator&();
        Mat4f operator+(Mat4f &mat);
        Mat4f operator*(Mat4f &mat);
        float& operator[](int index);
        float operator[](int index) const;
        
    
};


#endif
