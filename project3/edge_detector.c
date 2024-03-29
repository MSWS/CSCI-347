#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define LAPLACIAN_THREADS 32 // change the number of threads as you run your concurrency experiment

/* Laplacian filter is 3 by 3 */
#define FILTER_WIDTH 3
#define FILTER_HEIGHT 3

#define RGB_COMPONENT_COLOR 255

typedef struct {
  unsigned char r, g, b;
} PPMPixel;

struct parameter {
  PPMPixel *image;         // original image pixel data
  PPMPixel *result;        // filtered image pixel data
  unsigned long int w;     // width of image
  unsigned long int h;     // height of image
  unsigned long int start; // starting point of work
  unsigned long int size;  // equal share of work (almost equal if odd)
};

struct file_name_args {
  char *input_file_name;     // e.g., file1.ppm
  char output_file_name[20]; // will take the form laplaciani.ppm, e.g.,
                             // laplacian1.ppm
};

double total_elapsed_time = 0;

/*This is the thread function. It will compute the new values for the region of
   image specified in params (start to start+size) using convolution. For each
   pixel in the input image, the filter is conceptually placed on top ofthe
   image with its origin lying on that pixel. The  values  of  each  input image
   pixel  under  the  mask  are  multiplied  by the corresponding filter values.
        Truncate values smaller than zero to zero and larger than 255 to 255.
        The results are summed together to yield a single output value that is
   placed in the output image at the location of the pixel being processed on
   the input.
        */
void *compute_laplacian_threadfn(void *params) {
  struct parameter *obj = (struct parameter *)params;
  PPMPixel *image = obj->image;
  PPMPixel *result = obj->result;
  unsigned long int width = obj->w;
  unsigned long int h = obj->h;
  int startWork = obj->start;
  int endWork = startWork + (obj->size);

  int laplacian[FILTER_WIDTH][FILTER_HEIGHT] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};

  int red = 0;
  int green = 0;
  int blue = 0;
  int x_coordinate = 0;
  int y_coordinate = 0;

  for (int row_iter = startWork; row_iter < endWork;
       row_iter++) { // Each row we are supposed to do 
    if (row_iter == h)
      break;
    for (int iteratorImageWidth = 0; iteratorImageWidth < width;
         iteratorImageWidth++) { // Each column
      for (int iteratorImageHeight = 0; iteratorImageHeight < FILTER_HEIGHT;
           iteratorImageHeight++) { // Each row
        for (int iteratorFilterWidth = 0; iteratorFilterWidth < FILTER_WIDTH;
             iteratorFilterWidth++) {
					// We love triple for loops
					// I love QUADRUPLE for loop
          x_coordinate = (iteratorImageWidth - FILTER_WIDTH / 2 + iteratorFilterWidth + width) % width;
          y_coordinate = (row_iter - FILTER_HEIGHT / 2 + iteratorImageHeight + h) % h;
          red += image[y_coordinate * width + x_coordinate].r *
                 laplacian[iteratorImageHeight][iteratorFilterWidth];
          green += image[y_coordinate * width + x_coordinate].g *
                   laplacian[iteratorImageHeight][iteratorFilterWidth];
          blue += image[y_coordinate * width + x_coordinate].b *
                  laplacian[iteratorImageHeight][iteratorFilterWidth];
        }
      }

      // Clamp values to be [0, 255]
			red = red > 255 ? 255 : red < 0 ? 0 : red;
			green = green > 255 ? 255 : green < 0 ? 0 : green;
			blue = blue > 255 ? 255 : blue < 0 ? 0 : blue;

      // Output filtered pixels to result image
      result[row_iter * width + iteratorImageWidth].r = red;
      result[row_iter * width + iteratorImageWidth].g = green;
      result[row_iter * width + iteratorImageWidth].b = blue;

      red = 0;
      green = 0;
      blue = 0;
    }
  }

  return NULL;
}

/* Apply the Laplacian filter to an image using threads.
         Each thread shall do an equal share of the work, i.e.
work=height/number of threads. If the size is not even, the last thread shall
take the rest of the work. Compute the elapsed time and store it in *elapsedTime
(Read about gettimeofday). Return: result (filtered image)
*/
PPMPixel *apply_filters(PPMPixel *image, unsigned long w, unsigned long h, double *elapsedTime) {
  struct timeval begin, end;
  gettimeofday(&begin, 0);

  int start = 0;
  int work = h / LAPLACIAN_THREADS; // work each thread handles
  pthread_t t[LAPLACIAN_THREADS];   // create thread array
  struct parameter data[LAPLACIAN_THREADS];
  PPMPixel *result = malloc(w * h * sizeof(PPMPixel));

  for (int i = 0; i < LAPLACIAN_THREADS; i++) {
    data[i].w = w;
    data[i].h = h;
    data[i].start = start;
    data[i].result = result;
    data[i].image = image;
    data[i].size = work;
    if (i == LAPLACIAN_THREADS - 1)  // set the work of last thread to equal height - last starting point
      data[i].size = h - start;
    if (pthread_create(&t[i], NULL, compute_laplacian_threadfn,
                       (void *)&data[i]) != 0)
			// create thread and each thread calls compute_laplacian_threadfn
      printf("Unable to create thread %d\n", i);
    start = start + work;
  }

  for (int i = 0; i < LAPLACIAN_THREADS; i++)
    pthread_join(t[i], NULL); // join threads

  gettimeofday(&end, 0);
  long sec = end.tv_sec - begin.tv_sec;
  long microsec = end.tv_usec - begin.tv_usec;
  *elapsedTime += sec + microsec / 1000000.0;

  return result;
}

/*Create a new P6 file to save the filtered image in. Write the header block
        e.g. P6
        Width Height
        Max color value
        then write the image data.
        The name of the new file shall be "filename" (the second argument).
        */
void write_image(PPMPixel *image, char *filename, unsigned long int width,
                 unsigned long int height) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s' for writing\n", filename);
    exit(1);
  }
  fprintf(fp, "P6\n");
  fprintf(fp, "%ld %ld\n", width, height);
  fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);
  for (int i = 0; i < width * height; i++) {
    fwrite(&image[i].r, sizeof(char), sizeof(char), fp);
    fwrite(&image[i].g, sizeof(char), sizeof(char), fp);
    fwrite(&image[i].b, sizeof(char), sizeof(char), fp);
  }
  fclose(fp);
}

/* Open the filename image for reading, and parse it.
         Example of a ppm header: //http://netpbm.sourceforge.net/doc/ppm.html
         P6                  -- image format
# comment           -- comment lines begin with
## another comment  -- any number of comment lines
200 300             -- image width & height
255                 -- max color value

Check if the image format is P6. If not, print invalid format error message.
If there are comments in the file, skip them. You may assume that comments exist
only in the header block. Read the image size information and store them in
width and height. Check the rgb component, if not 255, display error message.
Return: pointer to PPMPixel that has the pixel data of the input image
(filename). The pixel data is stored in scanline order from left to right (up to
bottom) in 3-byte chunks (r g b values for each pixel) encoded as binary
numbers.
*/
PPMPixel *read_image(const char *filename, unsigned long int *width, unsigned long int *height) {
  FILE *fp;
  PPMPixel *img;
  unsigned long int w, h;
  char buf[1024];
  int component;

  fp = fopen(filename, "rb");

  if (fp == NULL) {
    fprintf(stderr, "Unable to open file '%s' for reading\n", filename);
    exit(1);
  }

  // get image format
  if (fgets(buf, sizeof(buf), fp) == NULL) {
    perror("fgets error\n");
    fclose(fp);
		exit(1);
	}
  else if (buf[0] != 'P' || buf[1] != '6') {
    printf("Invalid image format (must be 'P6')\n");
    fclose(fp);
		exit(1);
	}

  // skip comments
  fgets(buf, sizeof(buf), fp);
  while (strstr(buf, "#") == NULL)
    fgets(buf, sizeof(buf), fp);

  // get height and width
  int num;
  if (fgets(buf, sizeof(buf), fp) == NULL) {
    perror("fgets error\n");
  } else if ((num = sscanf(buf, "%ld %ld", &w, &h)) == 2) {
    *width = w;
    *height = h;
  }

  // get rgb component
  if (fgets(buf, sizeof(buf), fp) == NULL) {
    perror("fgets error\n");
  } else if (sscanf(buf, "%d", &component) == 1) {
    if (component != RGB_COMPONENT_COLOR)
      printf("Invalid rgb component color\n");
  }
  // read up to sizeof(buffer) bytes
  int img_size = (*width) * (*height) * sizeof(PPMPixel);
  img = malloc(img_size);
  PPMPixel *pixel = malloc(sizeof(PPMPixel));
  for (int i = 0; i < ((*width) * (*height)); i++) {
    if (fread(pixel, sizeof(char), sizeof(PPMPixel), fp) == 0)
      perror("end of file!\n");
    img[i] = *pixel;
  }

  free(pixel);
  fclose(fp);
  return img;
}

/* The thread function that manages an image file.
         Read an image file that is passed as an argument at runtime.
         Apply the Laplacian filter.
         Save the result image in a file called laplaciani.ppm, where i is the
image file order in the passed arguments. Example: the result image of the file
passed third during the input shall be called "laplacian3.ppm".
*/
void *manage_image_file(void *args) {
  struct parameter data;
	struct file_name_args* fileNameArg = (struct file_name_args*) args;
  data.image = read_image(fileNameArg->input_file_name, &data.w, &data.h);
  data.result = apply_filters(data.image, data.w, data.h, &total_elapsed_time);

  write_image(data.result, fileNameArg->output_file_name, data.w, data.h);

  free(data.image);
  free(data.result);
  return NULL;
}

/*The driver of the program. Check for the correct number of arguments. If wrong
   print the message: "Usage ./a.out filename[s]" It shall accept n filenames as
   arguments, separated by whitespace, e.g., ./a.out file1.ppm file2.ppm
   file3.ppm It will create a thread for each input file to manage. It will
   print the total elapsed time in .4 precision seconds(e.g., 0.1234 s). The
   total elapsed time is the total time taken by all threads to compute the edge
   detection of all input images .
        */
int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("Usage: %s filename[s]\n", argv[0]);
    exit(1);
  }
  pthread_t t[argc - 1];
  struct file_name_args arguments[argc - 1]; // create an array of struct file infos to pass to
                                             // thread functions
  int err = 0;
  for (int i = 0; i < argc - 1; i++) {
    arguments[i].input_file_name = argv[i + 1];
    snprintf(arguments[i].output_file_name, sizeof(arguments[i].output_file_name) + sizeof(int),
             "laplacian%d.ppm", (i + 1)); // create the output file name
    err = pthread_create(&t[i], NULL, manage_image_file,
                         (void *)&arguments[i]); // create threads
    if (err != 0)
      perror("cannot create thread\n");
  }

  for (int i = 0; i < argc - 1; i++) // Wait for all threads to finish
    pthread_join(t[i], NULL);

  printf("elapsed time: %.4f\n", total_elapsed_time);
  return 0;
}
