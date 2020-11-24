//
// Created by nickolay on 23.11.2020.
//
#include <QDebug>
#include "TextureImage.h"
#include <QCoreApplication>

TextureImage::TextureImage()
        : width(0),
          height(0),
          databuf(NULL)
{
}

TextureImage::~TextureImage()
{
    if (databuf)
        delete[] databuf;
}

void TextureImage::loadTGAImage(const char *filename)
{
    FILE    *file          = NULL;
    GLubyte TGAheader[12]  = {0,0,2,0,0,0,0,0,0,0,0,0}; // Uncompressed TGA Header
    GLubyte TGAcompare[12] = {0};                       // Used To Compare TGA Header
    GLubyte header[6]      = {0};                       // First 6 Useful Bytes From The Header
    GLuint  bytesPerPixel  = 0;                         // Holds Number Of Bytes Per Pixel Used In The TGA File
    GLuint  imageSize      = 0;                         // Used To Store The Image Size When Setting Aside Ram
    GLuint  bitsPerPixel   = 0;

    isTextureLoaded = false;
    if (!(file = fopen(filename, "rb"))) {
        qDebug() << "Error opening file " << filename;
    }

    if ( fread( TGAcompare, 1, sizeof(TGAcompare), file ) != sizeof( TGAcompare ) ||
         memcmp( TGAheader, TGAcompare, sizeof(TGAheader) ) != 0 ||
         fread( header, 1, sizeof(header), file ) != sizeof( header ) ) {
        fclose(file);
        qDebug() << "TGA format error " << filename;

        return;
    }

    width   = header[1] * 256 + header[0];          // Determine The TGA Width  (highbyte*256+lowbyte)
    height  = header[3] * 256 + header[2];          // Determine The TGA Height  (highbyte*256+lowbyte)
    bitsPerPixel  = header[4];

    if( width <= 0 || height <= 0 || bitsPerPixel != 32 ) {
        fclose(file);
        qDebug() << "TGA format error " << filename;

        return;
    }

    if (databuf)
        delete[] databuf;

    bytesPerPixel = bitsPerPixel / 8;
    imageSize = width * height * bytesPerPixel;
    databuf = new GLubyte[imageSize];

    if ( fread(databuf, 1, imageSize, file) != imageSize ) {
        delete[] databuf;
        fclose(file);
        qDebug() << "TGA format error " << filename;

        return;
    }

    // Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
    for(GLuint i = 0; i < imageSize; i += bytesPerPixel) {
        databuf[i] ^= databuf[i + 2];
        databuf[i + 2] ^= databuf[i];
        databuf[i] ^= databuf[i + 2];
    }
    fclose(file);

    isTextureLoaded = true;
}

void TextureImage::flip()
{
    for( int h = 0; h < height; h++ ) {
        for( int w = 0; w < width/2; w++ ) {
            int index_l = h * width * BPP + w * BPP;
            int index_r = h * width * BPP + BPP * (width - w);
            for( int d = 0; d < 4; d++ ) {
                databuf[index_l + d] ^= databuf[index_r + d];
                databuf[index_r + d] ^= databuf[index_l + d];
                databuf[index_l + d] ^= databuf[index_r + d];
            }
        }
    }
}

GLuint TextureImage::buildTexture()
{
    if(!isTextureLoaded) {
        qDebug() << "glFont: BUILD TEXTURE FAILED!";

        return 0;
    }

    GLuint tex_id = 0;
    glGenTextures(1, &tex_id);

    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, databuf);

    return tex_id;
}

oglFont::oglFont() {
//    oglFont(256, 256, 19, 38); // init font
}

oglFont::oglFont(GLuint init_m_width, GLuint init_m_height, GLuint init_c_width, GLuint init_c_height) :
        m_width(init_m_width),
        m_height(init_m_height),
        c_width(init_c_width),
        c_height(init_c_height)
{
    c_per_row = m_width / c_width;
    if(!textureImage.isLoaded()) {
        QString texFile = QCoreApplication::applicationDirPath() + "/../font.tga";//"/res/font.tga";
        textureImage.loadTGAImage(texFile.toLocal8Bit());
        textureId = textureImage.buildTexture();
    }
}

int oglFont::getCharIndex(char *c) {
    int retVal = -1;
    switch (c[0]) {
        case '0': retVal = 0; break;
        case '1': retVal = 1; break;
        case '2': retVal = 2; break;
        case '3': retVal = 3; break;
        case '4': retVal = 4; break;
        case '5': retVal = 5; break;
        case '6': retVal = 6; break;
        case '7': retVal = 7; break;
        case '8': retVal = 8; break;
        case '9': retVal = 9; break;
        case ',':
        case '.': retVal = 10; break;
        case '/': retVal = 11; break;
        case ' ': retVal = 42; break;
    }

    return retVal;
}

void oglFont::drawText(GLfloat cw, GLfloat ch, char *txt)
{
    glBegin(GL_TRIANGLES);

    //character location and dimensions
    GLfloat cx = 10.0f;
    GLfloat cy = 10.0f;

    //calculate how wide each character is in term of texture coords
    GLfloat dtx = float(c_width) / float(m_width);
    GLfloat dty = float(c_height) / float(m_height);

    for (char * c = txt; *c != 0; c++, cx += cw) {
        int index = getCharIndex(c);
        int row = index / c_per_row;
        int col = index % c_per_row;

        if (index < 0) {
            qDebug() << "glFont: Character outside of font! char: " << c;
        }

        // find the texture coords
        GLfloat tx = float(col * c_width) / float(m_width);
        GLfloat ty = float(row * c_height) / float(m_height);

        glTexCoord2f(tx, ty);
        glVertex2f(cx, cy);
        glTexCoord2f(tx + dtx, ty);
        glVertex2f(cx + cw, cy);
        glTexCoord2f(tx + dtx, ty + dty);
        glVertex2f(cx + cw, cy + ch);
        glTexCoord2f(tx, ty + dty);
        glVertex2f(cx, cy + ch);
    }
    glEnd();
}