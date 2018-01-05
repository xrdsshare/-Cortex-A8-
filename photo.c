#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
//unsigned char lcd_buf[800*480*4];
#include <jpeglib.h>
#include <setjmp.h>
int p ;

struct buffer {
	void* start;
	size_t length;
};

struct buffer* buffers = NULL;
static unsigned int n_buffers = 0;

int lcd_fd;
unsigned long *fb_mem;

int cam_fd;

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;




int show_jpg(int index)
{
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr jerr;
  /* More stuff */
  FILE * infile;		/* source file */
  char * buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = NULL;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);

    return 0;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */
 // jpeg_mem_src();
  jpeg_mem_src (&cinfo,buffers[index].start,buffers[index].length);
  //jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.txt for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  //buffer = (*cinfo.mem->alloc_sarray)
	//	((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
  buffer = malloc(row_stride);
  
  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
 //printf(" cinfo.output_width : %d\n ",cinfo.output_width);
 // printf(" cinfo.output_height : %d\n ",cinfo.output_height);
 // printf(" cinfo.output_components : %d\n ",cinfo.output_components);

  int i=0,x,y,l,r;
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
    // 将24bit RGB 刷新到LCD
	//将bmp_buf 24bit的图像数据写入32bit的fb_mem
        //printf("cinfo.output_scanline: %d\n ",cinfo.output_scanline);
	 for(i=0,x=0;x<cinfo.output_width;x++)
			 {
				 *(fb_mem+(cinfo.output_scanline-1)*800+x) =  buffer[i]<<16|buffer[i+1]<<8|buffer[i+2];
				  i+=3;
			 }

  }
    
	
	/*左边区域：当识别到左区为色块区时，l=0，否则，l=1*/
        p = 0;
	for(y=0;y<cinfo.output_height;y++)
	{
	for(x=0;x<cinfo.output_width/3;x++)
	{	
		//printf("%x\n",*(fb_mem+y*800+x));
		if(*(fb_mem+y*800+x) <=0x5B0000||*(fb_mem+y*800+x)>0xFFF000)
		{
			p = 1;
			break;
		}
			
	}
	
		if(p==1)
			break;
	}
	l=p;

    /*右边区域：当识别到右区为色块区时，r=3，否则，r=1*/
	p=3;
	for(y=0;y<cinfo.output_height;y++)
	{
	for(x=2*cinfo.output_width/3;x<cinfo.output_width;x++)
	{	
		//printf("%x\n",*(fb_mem+y*800+x));
		if(*(fb_mem+y*800+x) <=0x5B0000||*(fb_mem+y*800+x)>0xFFF000)
		{
			p = 1;
		}
		
	}
		if(p==1)
			break;
	}
	r=p;


	
   p=l+r;
   if(p==1)
	    printf("left!\n");          //l=0,r=1  
   else if(p==4) printf("right!\n");  //l=1,r=3
   else if(p==3) printf("stop!\n");    //l=0,r=3
   else printf("move\n");              //l=1,r=1
  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */
  return 1;
}


int camera_take(int index)
{
	struct v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = index;	

	//  取出一帧图像
	if( -1 == ioctl(cam_fd, VIDIOC_DQBUF, &buf))
	{
		printf("camera VIDIOC_DQBUF	Failed!\n ");
		return -1;
	}

/*

	//  将图像写入文件
	int jpg_fd;
	jpg_fd = open("take.jpg",O_RDWR|O_CREAT,0666);
	if(jpg_fd == -1)
	{
		printf("open jpg Failed!\n ");
		return -1;		
	}

	write(jpg_fd,buffers[buf.index].start,buffers[buf.index].length);


	close(jpg_fd);	
*/
	
	return 0;
	
}

int lcd_init()
{

	int ret;
	printf("test Project!\n");

	lcd_fd = open("/dev/fb0",O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open LCD");
		return -1;
	}


	//显存映射
	fb_mem = (unsigned long *)mmap(NULL,		800*480*4,PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);
								 
	if(fb_mem == MAP_FAILED)
		{
			printf("mmap LCD Failed!\n ");
			return -1;
		}

	return 0;
}


void lcd_uninit(void)
{
	munmap(fb_mem,800*480*4);
	close(lcd_fd);

}



int camera_init()
{
    int i;
	
	
 // 获取摄像头许可，申请
    cam_fd = open("/dev/video3",O_RDWR);
 	if(cam_fd == -1)
	{
		printf("open camera Failed!\n ");
		return -1;
	}  

   //  获取摄像头的性能参数
    struct v4l2_capability  cap;
	if (ioctl(cam_fd,VIDIOC_QUERYCAP,&cap) == -1)
		{
			printf("camera VIDIOC_QUERYCAP  Failed!\n ");
			return -1;		
		}

   printf("camera VIDIOC_QUERYCAP  version:%d  \n ",cap.version);
   printf("VIDIOC_QUERYCAP -> driver : %s \n",cap.driver);

 	struct v4l2_fmtdesc fmtd;
	memset(&fmtd, 0, sizeof(fmtd));
	fmtd.index = 0;
	fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while ((ioctl(cam_fd, VIDIOC_ENUM_FMT, &fmtd)) == 0) 
	{
		fmtd.index++;
		printf("{ pixelformat = ''%c%c%c%c'', description = ''%s'' }\n",
		          fmtd.pixelformat & 0xFF, (fmtd.pixelformat >> 8) & 0xFF, (fmtd.pixelformat >> 16) & 0xFF, 
		          (fmtd.pixelformat >> 24) & 0xFF, fmtd.description);
}  
   
   

    //摄像头的通道设置
	int index;
	index = 0;
	if (-1 == ioctl (cam_fd, VIDIOC_S_INPUT, &index)) {
			printf("camera VIDIOC_S_INPUT  Failed!\n ");
			return -1;	;
	}  


    // 配置摄像头参数
    struct v4l2_format fmt;
	memset (&fmt, 0, sizeof (fmt)); 

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 640;
	fmt.fmt.pix.height = 480;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;
	
	if (-1 == ioctl (cam_fd, VIDIOC_S_FMT, &fmt)) {
		printf("camera VIDIOC_S_FMT  Failed!\n ");
		return -1;	;
	}

    //  获取摄像头的采集参数
    
	memset (&fmt, 0, sizeof (fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl (cam_fd, VIDIOC_G_FMT, &fmt)) {
		printf("camera VIDIOC_G_FMT  Failed!\n ");
		return -1;	;
	}
	
    printf("Stream Format Informations:\n");
    printf(" type: %d\n", fmt.type);
    printf(" width: %d\n", fmt.fmt.pix.width);
    printf(" height: %d\n", fmt.fmt.pix.height);
    char fmtstr[8];
    memset(fmtstr, 0, 8);
    printf(" pixelformat: %s\n", &fmt.fmt.pix.pixelformat);
    printf(" field: %d\n", fmt.fmt.pix.field);
    printf(" bytesperline: %d\n", fmt.fmt.pix.bytesperline);
    printf(" sizeimage: %d\n", fmt.fmt.pix.sizeimage);
    printf(" colorspace: %d\n", fmt.fmt.pix.colorspace);
   //  申请 多缓冲空间

    struct v4l2_requestbuffers req;
    memset (&req, 0, sizeof (req)); 

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	
	if (-1 == ioctl (cam_fd, VIDIOC_REQBUFS, &req)) {
		printf("camera VIDIOC_REQBUFS  Failed!\n ");
		return -1;	;
	} 

    buffers = calloc (req.count, sizeof (*buffers));
   
	if (!buffers) {
		printf("camera calloc  Failed!\n ");
		return -1;	;
	}


	struct v4l2_buffer buf;
	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {

		//  一个缓冲区对应一个struct v4l2_buffer
		memset (&buf, 0, sizeof (buf)); 
		
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		//  查询对应缓冲区的参数
		if (-1 == ioctl (cam_fd, VIDIOC_QUERYBUF, &buf))
			{
				printf("camera VIDIOC_QUERYBUF  Failed!\n ");
				return -1;		
			}

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap (NULL,
										 buf.length,
										PROT_READ | PROT_WRITE,
										MAP_SHARED,
										cam_fd, 
										buf.m.offset);
		
		if (MAP_FAILED == buffers[n_buffers].start)
			{
				printf("camera mmap  Failed!\n ");
				return -1;		
			}

        //  入队
        if( -1 == ioctl(cam_fd , VIDIOC_QBUF, &buf))
        {
			printf("camera VIDIOC_QBUF	Failed!\n ");
			return -1;	
        }		

}

	//  开启摄像头
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl (cam_fd, VIDIOC_STREAMON, &type))
	{
			printf("camera VIDIOC_STREAMON  Failed!\n ");
			return -1;		
	}


	return 0;

}


void camera_uninit()
{
    int i;

    for(i=0;i<4;i++)
		munmap(buffers[i].start, buffers[i].length);

	close(cam_fd);	

}


int camera_inqueue(int index)
{
	struct v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = index;	

	if( -1 == ioctl(cam_fd, VIDIOC_QBUF, &buf))
	{
		printf("camera VIDIOC_QBUF	Failed!\n ");
		return -1;
	}
	return 0;
}


int dev_init()
{
	lcd_init();
	camera_init();
}


void dev_uninit()
{
	lcd_uninit();
	camera_uninit();

}

int main()
{
	int i;
	dev_init();

	while(1)
	{
		for(i=0;i<4;i++)
		{
			camera_take(i);
			show_jpg(i);
			camera_inqueue(i);
		}
	}


	dev_uninit();
	

	return 0;	
}

