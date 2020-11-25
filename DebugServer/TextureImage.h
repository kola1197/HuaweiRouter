//
// Created by nickolay on 23.11.2020.
//

#ifndef GL_FONT_H
#define GL_FONT_H

#include <GL/gl.h>

#define BPP 4 // Byptes per pixel (32 bit = 4 bytes)


class TextureImage
{
public:
    TextureImage();
    ~TextureImage();

    void loadTGAImage(const char * filename);
    void flip();
    bool isLoaded() { return isTextureLoaded; };

    GLuint buildTexture();

private:
    bool isTextureLoaded = false;
    GLuint   width;
    GLuint   height;
    GLubyte* databuf;
};

class oglFont
{
public:
    oglFont();
    oglFont(GLuint init_m_width, GLuint init_m_height,
            GLuint init_c_width, GLuint init_c_height);
    void drawText(GLfloat w, GLfloat h, char * txt);
    GLuint getTextureID() { return textureId; };

private:
    TextureImage textureImage;
    GLuint textureId;
    GLint c_per_row;
    // bitmap setting
    GLuint m_width;
    GLuint m_height;
    // character settings
    GLuint c_width;
    GLuint c_height;

    int getCharIndex(char *c);
};

#endif // GL_FONT_H