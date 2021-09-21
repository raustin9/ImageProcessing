#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define BYTE_BOUND(value) value < 0 ? 0 : (value > 255 ? 255 : value)

#include "image.h"
#include "stb_image.h"
#include "stb_image_write.h"

Image::Image(const char* filename) {
	if(read(filename)) {
		printf("Read %s\n", filename);
		size = w*h*channels;
	}
	else {
		printf("Failed to read %s\n", filename);
	}
}

Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
	size = w*h*channels;
	data = new uint8_t[size];
}

Image::Image(const Image& img) : Image(img.w, img.h, img.channels) {
	memcpy(data, img.data, size);
}

Image::~Image() {
	stbi_image_free(data);
}

bool Image::read(const char* filename) {
	data = stbi_load(filename, &w, &h, &channels, 0);
	return data != NULL;
}

bool Image::write(const char* filename) {
	ImageType type = getFileType(filename);
	int success;
  switch (type) {
    case PNG:
      success = stbi_write_png(filename, w, h, channels, data, w*channels);
      break;
    case BMP:
      success = stbi_write_bmp(filename, w, h, channels, data);
      break;
    case JPG:
      success = stbi_write_jpg(filename, w, h, channels, data, 100);
      break;
    case TGA:
      success = stbi_write_tga(filename, w, h, channels, data);
      break;
  }
  return success != 0;
}

ImageType Image::getFileType(const char* filename) {
    const char* ext = strrchr(filename, '.');
    /* Find the file extension of the file */
    if (ext != nullptr) {
        if (strcmp(ext, ".png") == 0) {
            return PNG;
        } else if (strcmp(ext, ".jpg") == 0) {
            return JPG;
        } else if (strcmp(ext, ".BMP") == 0) {
            return BMP;
        } else if (strcmp(ext, ".tga") == 0) {
            return TGA;
        }
    }
    /* Return PNG if file type check fails */
        return PNG;
}

/* Grayscales the image by averaging all of the color channels and then setting the channels to that value */
Image& Image::grayscale_avg() {
  if (channels < 3) {
    printf("iamge %p has less than 3 channels, it is assumed to already be grayscale.", this);
  } else {
    for (int i = 0; i  < size; i+= channels) {
      /* (r+g+b) / 3 */
      int gray = (data[i] + data[i+1] + data[i+2]) / 3;
      memset(data+i, gray, 3);
    }
  }
  return *this;
}

/* Grayscales the image using a weighted average in order to preserve our "perceived luminance" */
Image& Image::grayscale_lum() {
  if (channels < 3) {
      printf("iamge %p has less than 3 channels, it is assumed to already be grayscale.", this);
  } else {
    for (int i = 0; i  < size; i+= channels) {
      /* weighted average of color channels */
      int gray = (0.2126 * data[i]) + (0.7152 * data[i+1]) + (0.0722 * data[i+2]);
      memset(data+i, gray, 3);
    }
  }
  return *this;
}

/* Function to mask and change color value of an image */
Image& Image::colorMask(float r, float g, float b) {
  /* Check to see if inputs are in legal range */
  bool legal = (r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1);

  if (channels < 3) {
    printf("\e[31m[ERROR] Color mask requires at least 3 channels, but this image has %d channels\e[0m\n", channels);
  } else if (legal) {
    for (int i = 0; i < size; i += channels) {
    data[i] *= r;
    data[i+1] *= g;
    data[i+2] *= b;
    }
  } else {
    printf("\nERROR: Invalid values for r, g, and b. Image unchanged\n");
  }
  return *this;
}

Image& Image::encodeMessage(std::string message) {
  uint32_t length = message.length() * 8;
  if (length + STEG_HEADER_SIZE > size) {
    printf("\e[ERROR] This message is too large (%lu bits / %zu bits)\e[0m\n", length+STEG_HEADER_SIZE, size);
    return *this;
  }

  for (uint8_t i = 0; i < STEG_HEADER_SIZE; ++i) {
    data[i] &= 0xfe;
    data[i] |= (length >> (STEG_HEADER_SIZE - 1 - i)) & 1UL;
  }

  for (uint32_t i = 0; i < length; ++i) {
    data[i+STEG_HEADER_SIZE] &= 0xfe;
    data[i+STEG_HEADER_SIZE] |= (message[i/8] >> ((length-1-i)%8)) & 1;
  }

  return *this;
}

Image& Image::decodeMessage(char* buffer, size_t* message_length) {
  uint32_t length = 0;
  for (uint8_t i = 0; i < STEG_HEADER_SIZE; ++i) {
    length = (length << 1) | (data[i] & 1);
  }
  *message_length = length / 8;

  for (uint32_t i = 0; i < length; ++i) {
    buffer[i/8] = (buffer[i/8] << 1) | (data[i+STEG_HEADER_SIZE] & 1);
  }
  return *this;
}

Image& Image::diffmap(Image& img) {
  int compare_width = fmin(w, img.w);
  int compare_height = fmin(h, img.h);
  int compare_channels = fmin(channels, img.channels);

  for (uint32_t i = 0; i < compare_height; ++i) {
    for(uint32_t j = 0; j < compare_width; ++j) {
      for (uint8_t k = 0; k < compare_channels; ++k) {
        data[(i*w+j)*channels+k] = BYTE_BOUND(abs((data[(i*w+j)*channels+k]) - (img.data[(i*img.w+j)*img.channels+k])));
      }
    }
  }
  return *this;
}

Image& Image::diffmap_scale(Image& img, uint8_t scale) {
  int compare_width = fmin(w, img.w);
  int compare_height = fmin(h, img.h);
  int compare_channels = fmin(channels, img.channels);
  uint8_t largest = 0;

  for (uint32_t i = 0; i < compare_height; ++i) {
    for(uint32_t j = 0; j < compare_width; ++j) {
      for (uint8_t k = 0; k < compare_channels; ++k) {
        data[(i*w+j)*channels+k] = BYTE_BOUND(abs((data[(i*w+j)*channels+k]) - (img.data[(i*img.w+j)*img.channels+k])));
        largest = fmax(largest, data[(i*w+j)*channels+k]);
      }
    }
  }

  scale = 255 / (fmax(1, fmax(scale, largest)));
  for (int i = 0; i < size; ++i) {
    data[i] *= scale;
  }
  return *this;
}