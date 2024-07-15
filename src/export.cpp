#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "config.h"
#include "ogl.h"
#include "stb_image_write.h"


void export_image(string file_path) {
    file_path += ".png";

    int width = g_get_screen_w();
    int height = g_get_screen_h();

    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei bufferSize = stride * height;
    std::vector<char> buffer(bufferSize);

    glBindFramebuffer(GL_FRAMEBUFFER, g_get_active_buffer()->id);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(g_get_active_buffer()->texture);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    stbi_flip_vertically_on_write(true);
    stbi_write_png(file_path.c_str(), width, height, nrChannels, buffer.data(), stride);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
