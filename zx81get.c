/* zx81get - read ZX81 tapes via /dev/dsp, writing to one or more .P files.
 * based on my `zxgettap', which does much the same for ZX Spectrum tapes.
 *
 * PD by RJM
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>


unsigned char databuf[16384];  /* as much as you could possibly save */
				/* (slightly more in fact :-)) */

/* from z81's common.c, needed for stripname()... */

/* the ZX81 char is used to index into this, to give the ascii.
 * awkward chars are mapped to '_' (underscore), and the ZX81's
 * all-caps is converted to lowercase.
 * The mapping is also valid for the ZX80 for alphanumerics.
 * WARNING: this only covers 0<=char<=63!
 */
static char zx2ascii[64]={
/*  0- 9 */ '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', 
/* 10-19 */ '_', '\'','#', '$', ':', '?', '(', ')', '>', '<', 
/* 20-29 */ '=', '+', '-', '*', '/', ';', ',', '.', '0', '1', 
/* 30-39 */ '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 
/* 40-49 */ 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 
/* 50-59 */ 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
/* 60-63 */ 'w', 'x', 'y', 'z'
};





/* The ZX81 tape format deals in noise (= a bit) and silence.
 * Given what tapes are like (crap), the best approach is then to
 * see how different the current sample is to the last one.
 * If it's not very different, it's `silence', else it's noise.
 * This is a pretty kludgey approach, but it's the only one
 * I've had any luck with.
 */
int getaudio(int audio)
{
static unsigned char last=128;
unsigned char c,diff;

read(audio,&c,1);

diff=((c>last)?c-last:last-c);
last=c;

return(diff>30);
}


int findtone(int audio)
{
int count;

/* The ZX81's tape format is *horrible*.
 * It doesn't have any lead-in tone, just silence. (Do *what!?*)
 * So this `find tone' routine just waits for 0.5 sec of silence,
 * followed by any, er, non-silence. :-)
 */

fprintf(stderr,"<looking for start of program>\n");

do
  {
  count=0;
  while(getaudio(audio)==0) count++;
  }
while(count<11000);

fprintf(stderr,"<loading program...>\n");

return(1);
}


/* Again, the horrible ZX81 tape format has to be dealt with.
 * Each bit is the same frequency (I used 4kHz in zx81send, for example),
 * with a 1 being twice as long as a 0. There's then a silent gap
 * until the next bit. Sigh. Who designed this crap, eh?
 */
int timeswitch(int audio)
{
int time_taken,timeout;
int dropout_timer;
int max_dropout=10;

/* absorb any remaining between-bit delay */
timeout=11000;
while(getaudio(audio)==0)
  {
  timeout--;
  if(timeout==0) return(-1);
  }

timeout=11000;
time_taken=0;
dropout_timer=max_dropout;

while(1)
  {
  time_taken++;
  timeout--;
  if(timeout==0) return(-1);
  
  if(getaudio(audio)!=0)
    dropout_timer=max_dropout;
  else
    {
    /* no signal - but allow a certain dropout time since
     * it'll inevitably go through our `zero' when switching levels.
     */
    dropout_timer--;
    
    /* if the dropout persists, it must (well, should :-)) be the
     * between-bit delay.
     */
    if(dropout_timer==0)
      return(time_taken-max_dropout);
    }
  }
}


int getbyte(int audio)
{
int t,f,dat;

dat=0;
for(f=0;f<8;f++)
  {
  dat<<=1;
  
  /* sometimes we get a bogus one at the start of a bit, these can safely
   * be ignored. (Or rather, we used to - not sure if I need this any more,
   * but it can't hurt...)
   */
  while((t=timeswitch(audio))<=0) ;
  
  /* zero is ~32, one is ~64.
   * I'm sorry, are we hardcoded yet? :-)
   */
  if(t>48 && t<100) dat|=1;
  if(t<16 || t>=100)
    return(EOF);
  }

return(dat);
}


/* strip off ZX81 filename from header-and-file at buf, *lenp bytes long.
 * *lenp is modified to reflect the actual file's length.
 *
 * The filename is restricted to quite a short length, even though
 * ZX81 filenames can have arbitrary length (well, up to around 16k ;-)),
 * in order to reduce problems caused by corrupt files.
 */
char *stripname(unsigned char *buf,int *lenp)
{
static char ascname[1024];
int limit=32;	/* limit for filename length (arbitrary) */
unsigned char *fileptr=NULL;
int namelen;
int f,c;

if(*lenp<limit) limit=*lenp;

for(f=0;f<limit;f++)
  if(buf[f]>=128)
    {
    fileptr=buf+f+1;
    break;
    }

if(fileptr==NULL)
  return(NULL);		/* no filename, input must be corrupt */

/* fix length */
namelen=fileptr-buf;
(*lenp)-=namelen;

/* copy name (converting to ASCII) */
for(f=0;f<namelen;f++)
  {
  c=(buf[f]&127);
  ascname[f]=((c>63)?'_':zx2ascii[c]);
  }
ascname[namelen]=0;

/* move file up */
if(*lenp>0)
  memmove(buf,fileptr,*lenp);

return(ascname);
}


void writefile(char *name,unsigned char *buf,int length)
{
FILE *out;
    
if((out=fopen(name,"wb"))==NULL)
  fprintf(stderr,"<error - couldn't write .p file!>\n");
else
  {
  if(fwrite(buf,1,length,out)!=length)
    fprintf(stderr,"<error - couldn't write all of .p file>\n");
  else
    fprintf(stderr,"<written `%s' ok>\n",name);
  fclose(out);
  }
}


void verifyfile(char *name,unsigned char *buf,int length)
{
FILE *in;
int f,c;

if((in=fopen(name,"rb"))==NULL)
  fprintf(stderr,"<verify *FAILED*, couldn't open existing .p file!>\n");
else
  {
  int bad=0;
  
  for(f=0;f<length;f++)
    {
    if((c=fgetc(in))==EOF)
      {
      bad=1,fprintf(stderr,"<verify *FAILED*, EOF on existing .p file!>\n");
      break;
      }
    if(c!=buf[f])
      {
      bad=1,fprintf(stderr,"<verify *FAILED*, mismatch at offset %d>\n",f);
      break;
      }
    }
  
  /* check next read gives EOF */
  if(!bad && fgetc(in)!=EOF)
    bad=1,fprintf(stderr,"<verify *FAILED*, existing .p file is longer!>\n");
  
  fclose(in);
  
  if(!bad)
    fprintf(stderr,"<verified ok>\n");
  }
}




int main(int argc,char *argv[])
{
int audio,length;
int spd,c;
char *zx81name;
int verify=0;

if(argc==2 && strcmp(argv[1],"-v")==0)
  verify=1;

audio=open("/dev/dsp",O_RDONLY,0);
if(audio==-1)
  fprintf(stderr,"error: couldn't open /dev/dsp.\n"),exit(1);

spd=22050;
if(ioctl(audio,SNDCTL_DSP_SPEED,&spd)==-1)
  fprintf(stderr,"error: couldn't select 22kHz input.\n"),exit(1);

if(verify)
  fprintf(stderr,"[verifying files]\n\n");
else
  fprintf(stderr,"[copying files (existing files will be overwritten!)]\n\n");


while(1)
  {
  /* get lead-in tone */
  if(!findtone(audio))
    continue;
  
  length=0;
  while((c=getbyte(audio))!=EOF && length<sizeof(databuf))
    {
    databuf[length++]=c;
    
    if(length%1024==0 && length>0)
      fprintf(stderr,"<%dk...>\n",length/1024);
    }
  
  /* length can't be less than 130 bytes or so, but let's not be
   * too pedantic - if less than 32, forget it.
   */
  if(length<32)
    {
    fprintf(stderr,"<bad program>\n");
    continue;
    }
  
  /* strips off filename (moving the rest of the file up to databuf)
   * and fixes length to compensate.
   */
  if((zx81name=stripname(databuf,&length))==NULL)
    fprintf(stderr,"<couldn't strip off filename, file corrupt>\n");
  else
    {
    fprintf(stderr,"<program is %d bytes, filename `%s'>\n",length,zx81name);
    
    /* errr, a bit nasty, but stripname() won't mind... :-) */
    strcat(zx81name,".p");
    
    if(verify)
      verifyfile(zx81name,databuf,length);
    else
      writefile(zx81name,databuf,length);
    }
  }

/* not reached, but FWIW :-) */
close(audio);
exit(0);
}
