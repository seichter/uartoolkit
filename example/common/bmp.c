typedef struct {
	int width;
	int height;
	unsigned char *frame;
	int framesize;
} ARFrame;

int bmpLoad(const char* filename, ARFrame* bitmap) {

	unsigned int _uival;
	unsigned short _usval;
	unsigned int _res;
	unsigned int i;
	unsigned int temp;

	FILE* _file = fopen(filename,"rb");

	if (!_file) {
		printf("File %s does not exist",filename);
		return -1;
	};

	fseek(_file,18,SEEK_SET);

	fread((char*)&_uival,4,1,_file);	
	bitmap->width = _uival;

	fread((char*)&_uival,4,1,_file);	
	bitmap->height = _uival;

	bitmap->framesize = 3 * bitmap->width * bitmap->height;
	bitmap->frame = malloc(bitmap->framesize);

	fread((char*)&_usval,1,2,_file);

	if (_usval != 1)
	{
		printf("File has %s unsupported %d planes (%dx%d)\n",
			filename,
			_usval,
			bitmap->width,
			bitmap->height);

		fclose(_file);

		return 0;
	};


	fread((char*)&_usval,1,2,_file);

	if (24 != _usval) {

		printf("File has %s unsupported %d bpp (%dx%d)\n",
			filename,
			_usval,
			bitmap->width,
			bitmap->height);
		
		fclose(_file);
		return 0;
	};

	fseek(_file,24,SEEK_CUR);

	_res = fread(bitmap->frame,1,bitmap->framesize,_file);

	if (_res != bitmap->framesize) 
	{
		fclose(_file);
		return 0;
	};

	for (i = 0; i < bitmap->framesize; i += 3) {
		temp = bitmap->frame[i];
		bitmap->frame[i] = bitmap->frame[i+2];
		bitmap->frame[i+2] = temp;
	}

	printf("File has %s : %dx%d with %d bpp\n",
		filename,bitmap->width,bitmap->height,
		_usval);
	
	fclose(_file);
	return -1;
};
