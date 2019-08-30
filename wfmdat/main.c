/**********************************/
//
// wfmファイルから直接、間抜きなどしながら
// txt形式で出力する
//
//
// Creation   2010.10.18
// LastUpDate 2011.08.08
//
/**********************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

// deskewあり、なしでのポイントサイズ用構造体
typedef union quantum{
  char c;
  short int si;
}QUANTUM;

typedef enum format_type{ VERIFY, US, C, SI, I, LI, F, D, UL, ULL } FTYPE;
typedef union binary{
  unsigned short us;
  char c;
  short int si;
  int i;
  long int li;
  float f;
  double d;
  unsigned long ul;
  unsigned long long ull;
}BIN;

BIN r_data;  // 読み込みデータ
char r_array[1024];  // 読み込みデータ(char array)
int cnt = 0;

int checkPointCount(char *fname);
int wfmReader(char *fname, double *wave, QUANTUM *qarray, double *offset, double *scale, int *deskewState, int readsize, int readmode);

int main(int argc, char *argv[])
{
  int i, j;
  FILE *ofp;
  char *argstr;
  char checkstr[] = ".wfm";
  int pos;
  char *filename;
  char ofname[1024];
  char ofnamebuf[1024];
  double *wave;  // 波形データ
  QUANTUM *digit;   // 量子化データ
  
  int readsize, manuki, outsize;
  int mnk_cnt = 0, out_cnt= 0;
  int readmode, outputmode;
  double offset, scale;
  int deskewState;
  double samplingDiv;
  char readmode_str[1024];
  char outputmode_str[1024];


  if(argc != 7){
    fprintf(stderr, "Usage: WfmDirectOut WfmFile Manuki Num_of_Output_Line ReadMode OutputMode SamplingFreq[GHz]\n");
    exit(1);
  }

  // arg parameter set
  argstr = &argv[1][0];
  pos = strlen(argstr) - strlen(checkstr);
  if(pos >= 0 && strcmp(&argstr[pos], checkstr) == 0){
    filename = argv[1];
  }else{
    fprintf(stderr, "Please input wfm format file.\n");
    exit(1);
  }
  manuki = atoi(argv[2]) - 1;
  outsize = atoi(argv[3]);
  readmode = atoi(argv[4]);
  outputmode = atoi(argv[5]);
  samplingDiv = 1.00 / (double)atoi(argv[6]);

  // output File open
  if(readmode == 0)
    sprintf(readmode_str, "_Normalread");
  else
    sprintf(readmode_str, "_8bitread");
  if(outputmode == 0)
    sprintf(outputmode_str, "_wave");
  else
    sprintf(outputmode_str, "_quantum");

  sprintf(ofnamebuf, "%s%s_M%dL%d.txt", readmode_str, outputmode_str, manuki+1, outsize);
  strcpy(ofname, filename);
  strcat(ofname, ofnamebuf); // 応急処置
  if((ofp = fopen(ofname, "w")) == NULL){
    fprintf(stderr, "output file open error.\n");
    exit(1);
  }

  // wfmに格納されているポイント数のチェック
  if((readsize = checkPointCount(filename)) == -1){
    fprintf(stderr,"file check error.\n");
    exit(1);
  }

  // 領域確保
  wave = (double *)malloc(sizeof(double) * readsize);
  digit = (QUANTUM *)malloc(sizeof(QUANTUM) * readsize);
  if((wave == NULL) || (digit == NULL)){
    fprintf(stderr,"partitioning error!\n");
    exit(1);
  }

  // wfm読込
  if(wfmReader(filename, wave, digit, &offset, &scale, &deskewState, readsize, readmode) == -1){
    fprintf(stderr, "wfm file read error.\n");
    return 1;
  }


  // 全部出力するかのチェック
  if(outsize == -1)   outsize = readsize;

  fprintf(ofp, "Time [ns]\tData [arb. units]\n");

  // 量子化データ バイナリ& txt ファイル出力
  // deskew あり時の処理
  if(deskewState == 0){
    for(i = 0; i < readsize; i++){
      if(out_cnt >= outsize)  break;

      mnk_cnt++;

      if((i == 0) || (mnk_cnt > manuki)){
	if(outputmode == 0)
     	  fprintf(ofp, "%e\t%e\n", i*samplingDiv, wave[i]);
	else
	  fprintf(ofp, "%e\t%d\n", i*samplingDiv, digit[i].si);

	mnk_cnt = 0;
	out_cnt++;
      }
    }
  }
  // deskew なし時の処理
  else if(deskewState == 7){
    for(i = 0; i < readsize; i++){
      if(out_cnt >= outsize)  break;

      mnk_cnt++;

      if((i == 0) || (mnk_cnt > manuki)){
	if(outputmode == 0)
	  fprintf(ofp, "%e\t%e\n", i*samplingDiv, wave[i]);
	else
	  fprintf(ofp, "%e\t%d\n", i*samplingDiv, digit[i].c);

	mnk_cnt = 0;
	out_cnt++;
      }
    }
  }

  free(wave);
  free(digit);
  fclose(ofp);


  return 0;
}
//Q.2byteのズレはドコから？
// あるwfmデータの保存点数を調べる関数
int checkPointCount(char *fname)
{
  FILE *fp;
  unsigned long data_start, post_start;
  int format, unit;
  int i;

  if((fp = fopen(fname, "rb")) == NULL){
    fprintf(stderr, "inputfile open error. %d\n", i);
    return 1;
  }

  // DataStartの相対先頭アドレス位置を取得
  if(fseek(fp, 822, SEEK_SET) != 0){
    fprintf(stderr, "scale seek error.\n");
    return -1;
  }
  
  if(fread(&data_start, sizeof(unsigned long), 1, fp) < 1){
    fprintf(stderr, "scale read error.\n");
    return -1;
  }
  printf("%lu\n", data_start);
  // postBufferの相対先頭アドレス位置を取得
  if(fseek(fp, 826, SEEK_SET) != 0){
    fprintf(stderr, "post buffer seek error.\n");
    return -1;
  }
  if(fread(&post_start, sizeof(unsigned long), 1, fp) < 1){
    fprintf(stderr, "post buffer read error.\n");
    return -1;
  }
  printf("%lu\n", post_start);
  // フォーマットを取得
  if(fseek(fp, 240, SEEK_SET) != 0){//238 
    fprintf(stderr, "format seek error.\n");
    return -1;
  }
  if(fread(&format, sizeof(int), 1, fp) < 1){
    fprintf(stderr, "format read error.\n");
    return -1;
  }
  printf("%d\n", format);
  // (postBuf - curBuf)/sizeof(フォーマット)計算（分岐必要）
  switch(format){
  case 0:
    unit = sizeof(short int);
    break;
  case 7:
    unit = sizeof(char);
    break;
  default:
    fprintf(stderr, "format check error.\n");
    exit(1);
  }

  fclose(fp);
  printf("%lu\n", (post_start - data_start) / unit);
  return (post_start - data_start) / unit;
}

// wfmを直接読み込む関数
int wfmReader(char *fname, double *wave, QUANTUM *qarray, double *offset, double *scale, int *deskewState, int readsize, int readmode)
{
  FILE *fp;
  int i;
  int format;
  double scale_y, off, volt;
  int preskip, unitsize;
  short int si_data;
  char c_data;

  if((fp = fopen(fname, "rb")) == NULL){
    fprintf(stderr, "inputfile open error. %d\n", i);
    return -1;
  }

  if(fseek(fp, 168, SEEK_SET) != 0){//166?
    fprintf(stderr, "scale seek error.\n");
    return -1;
  }
  if(fread(&scale_y, sizeof(double), 1, fp) < 1){
    fprintf(stderr, "scale read error.\n");
    return -1;
  }
  if(fseek(fp, 176, SEEK_SET) != 0){//174?
    fprintf(stderr, "offset seek error.\n");
    return -1;
  }
  if(fread(&off, sizeof(double), 1, fp) < 1){
    fprintf(stderr, "offset read error.\n");
    return -1;
  }
  if(fseek(fp, 240, SEEK_SET) != 0){//238?
    fprintf(stderr, "format seek error.\n");
    return -1;
  }
  if(fread(&format, sizeof(int), 1, fp) < 1){
    fprintf(stderr, "format read error.\n");
    return -1;
  }

  *offset = off;
  *scale = scale_y;
  *deskewState = format;

  switch(format){
  case 0:
    unitsize = sizeof(short int);
    break;
  case 7:
    unitsize = sizeof(char);
    break;
  default:
    fprintf(stderr, "curve buffer format error.\n");
    return -1;
  }
  preskip = unitsize*16;

  if(fseek(fp, 838+preskip, SEEK_SET) != 0){
    fprintf(stderr, "curve buffer seek error.\n");
    return -1;
  }

  if(readmode == 0){
    if(format == 0){
      for(i = 0; i < readsize; i++){
	if(fread(&si_data, unitsize, 1, fp) < 1){
	  fprintf(stderr, "curve buffer read error.\n");
	  return -1;
	}
	wave[i] = (si_data * scale_y) + off;
	qarray[i].si = si_data;
      }
    }
    else if(format == 7){
      for(i = 0; i < readsize; i++){
	if(fread(&c_data, unitsize, 1, fp) < 1){
	  fprintf(stderr, "curve buffer read error.\n");
	  return -1;
	}
	wave[i] = (c_data * scale_y) + off;
	qarray[i].c = c_data;
      }
    }
  }else{
    if(format == 0){
      char data8bit;
      scale_y = scale_y * 256;
      *scale  = scale_y;
      *deskewState = 7;
      for(i = 0; i < readsize; i++){
	if(fread(&si_data, unitsize, 1, fp) < 1){
	  fprintf(stderr, "curve buffer read error.\n");
	  return -1;
	}
	data8bit = (char)(si_data >> 8);
	wave[i] = (data8bit * scale_y) + off;
	qarray[i].si = data8bit;
      }
    }
    else if(format == 7){
      for(i = 0; i < readsize; i++){
	if(fread(&c_data, unitsize, 1, fp) < 1){
	  fprintf(stderr, "curve buffer read error.\n");
	  return -1;
	}
	wave[i] = (c_data * scale_y) + off;
	qarray[i].c = c_data;
      }
    }
  }

  fclose(fp);


  return 0;
}

