/******************************************************
SuperEQ written by Naoki Shibata  shibatch@users.sourceforge.net

Shibatch Super Equalizer is a graphic and parametric equalizer plugin
for winamp. This plugin uses 16383th order FIR filter with FFT algorithm.
It's equalization is very precise. Equalization setting can be done
for each channel separately.


Homepage : http://shibatch.sourceforge.net/
e-mail   : shibatch@users.sourceforge.net

Some changes are from foobar2000 (www.foobar2000.org):

Copyright © 2001-2003, Peter Pawlowski
All rights reserved.

Other changes are:

Copyright © 2003, Klaus Post

*******************************************************/

#ifndef _SUPEREQ_H_
#define _SUPEREQ_H_

#include "fftsg.h"

class supereq
{
public:
        static const int NBANDS=9;
        typedef float audio_sample;

        class paramlistelm {
        public:
                class paramlistelm *next;

                char left,right;
                float lower,upper,gain;

                paramlistelm(void):
                        left(1), right (1),
                        lower(0),upper(0), gain(0),
                        next(NULL) {}

                ~paramlistelm() {
                        delete next;
                        next = NULL;
                };
        };

        class paramlist {
        public:
                paramlistelm *elm;

                paramlist(void):elm(NULL) {}
                ~paramlist() {
                        delete elm;
                        elm = NULL;
                }
        };

        template<class T> class mem_ops
        {
        public:
                static void set(T* dst,int val,unsigned count) {memset(dst,val,count*sizeof(T));}
                static T* alloc(unsigned count) {return reinterpret_cast<T*>(malloc(count * sizeof(T)));}
                static T* alloc_zeromem(unsigned count)
                {
                        T* ptr = alloc(count);
                        if (ptr) set(ptr,0,count);
                        return ptr;
                }
                static T* realloc(T* ptr,unsigned count)
                {return ptr ? reinterpret_cast<T*>(::realloc(reinterpret_cast<void*>(ptr),count * sizeof(T))) : alloc(count);}

                static void free(T* ptr) {::free(reinterpret_cast<void*>(ptr)); }
        };

        class mem_block
        {
        public:
                enum mem_logic_t {ALLOC_DEFAULT,ALLOC_FAST,ALLOC_FAST_DONTGODOWN};
        private:
                void * data;
                unsigned size,used;
                mem_logic_t mem_logic;
        public:
                inline mem_block() {data=0;size=0;used=0;mem_logic=ALLOC_DEFAULT;}
                inline ~mem_block() {if (data) free(data);}

                inline const void * get_ptr() const {return data;}
                inline void * get_ptr() {return data;}

                void * set_size(UINT new_used)
                {
                        if (new_used==0)
                        {
                                if (mem_logic != ALLOC_FAST_DONTGODOWN)
                                {
                                        if (data!=0) {free(data);data=0;}
                                        size = 0;
                                }
                        }
                        else
                        {
                                UINT new_size;
                                if (mem_logic == ALLOC_FAST || mem_logic == ALLOC_FAST_DONTGODOWN)
                                {
                                        new_size = size;
                                        if (new_size < 1) new_size = 1;
                                        while(new_size < new_used) new_size <<= 1;
                                        if (mem_logic!=ALLOC_FAST_DONTGODOWN) while(new_size>>1 > new_used) new_size >>= 1;
                                }
                                else
                                {
                                        new_size = new_used;
                                }

                                if (new_size!=size)
                                {
                                        if (data==0)
                                        {
                                                data = malloc(new_size);
                                        }
                                        else
                                        {
                                                void * new_data;
                #if defined(_DEBUG) && 0
                                                new_data = malloc(new_size);
                                                if (new_data) memcpy(new_data,data,new_size>size ? size : new_size);
                                                if (size >= 4) *(DWORD*)data = 0xDEADBEEF;
                                                free(data);
                                                data = new_data;
                #else
                                                new_data = realloc(data,new_size);
                                                if (new_data==0) free(data);
                                                data = new_data;
                #endif
                                        }
                                        size = new_size;
                                }
                        }
                        used = new_used;
                        return data;
                }


                inline void * check_size(unsigned new_size)
                {
                        if (used<new_size) return set_size(new_size);
                        else return get_ptr();
                }

                inline operator const void * () const {return get_ptr();}
                inline operator void * () {return get_ptr();}

                inline void zeromemory() {memset(get_ptr(),0,used);}

        };

        template<class T> class mem_block_t //: public mem_block
        {
                mem_block theBlock;//msvc7 sucks
        public:
                mem_block_t() {}
                mem_block_t(unsigned s) {theBlock.set_size(s*sizeof(T));}

                inline const T* get_ptr() const {return static_cast<const T*>(theBlock.get_ptr());}
                inline T* get_ptr() {return static_cast<T*>(theBlock.get_ptr());}

                inline T* set_size(unsigned new_size) {return static_cast<T*>(theBlock.set_size(new_size*sizeof(T)));}

                inline T* check_size(unsigned new_size) {return static_cast<T*>(theBlock.check_size(new_size*sizeof(T)));}

                inline operator const T * () const {return get_ptr();}
                inline operator T * () {return get_ptr();}

                inline void zeromemory() {theBlock.zeromemory();}
        };

private:
        typedef float REAL;
        static const int M=15;

        int rfft_ipsize,rfft_wsize;
        int *rfft_ip;
        REAL *rfft_w;


        REAL fact[M+1];
        REAL aa;
        REAL iza;
        REAL *lires,*lires1,*lires2,*irest;
        REAL *fsamples;
        REAL *ditherbuf;
        volatile int chg_ires,cur_ires;
        int winlen,winlenbit,tabsize,nbufsamples;
        int firstframe;

        mem_block_t<REAL> inbuf,outbuf;
        mem_block_t<audio_sample> done;
        int samples_done;

        void rfft(int n,int isign,REAL x[])
        {
                int newipsize,newwsize;

                if (n == 0) {
                        free(rfft_ip); rfft_ip = NULL; rfft_ipsize = 0;
                        free(rfft_w);  rfft_w  = NULL; rfft_wsize  = 0;
                        return;
                }

                newipsize = int(2+sqrtf(float(n/2)));
                if (newipsize > rfft_ipsize) {
                        rfft_ipsize = newipsize;
                        rfft_ip = mem_ops<int>::realloc(rfft_ip,rfft_ipsize);
                        rfft_ip[0] = 0;
                }

                newwsize = n/2;
                if (newwsize > rfft_wsize) {
                        rfft_wsize = newwsize;
                        rfft_w = mem_ops<REAL>::realloc(rfft_w,rfft_wsize);
                }

                rdft(n,isign,x,rfft_ip,rfft_w);
        }

        REAL izero(REAL x)
        {
                REAL ret = 1;
                int m;

                for(m=1;m<=M;m++)
                {
                        REAL t;
                        t = powf(float(x/2),float(m))/fact[m];
                        ret += t*t;
                }

                return ret;
        }

        REAL win(REAL n,int N) {return izero(alpha(aa)*sqrtf(1-4*n*n/((N-1)*(N-1)))/iza);}

        void equ_init(int wb)
        {
                  int i,j;

                  if (lires1 != NULL)   free(lires1);
                  if (lires2 != NULL)   free(lires2);
                  if (irest != NULL)    free(irest);
                  if (fsamples != NULL) free(fsamples);

                  winlen = (1 << (wb-1))-1;
                  winlenbit = wb;
                  tabsize  = 1 << wb;

                  lires1   = mem_ops<REAL>::alloc_zeromem(tabsize);
                  lires2   = mem_ops<REAL>::alloc_zeromem(tabsize);
                  irest    = mem_ops<REAL>::alloc_zeromem(tabsize);
                  fsamples = mem_ops<REAL>::alloc_zeromem(tabsize);
                  inbuf.set_size(winlen);
                  inbuf.zeromemory();
                  outbuf.set_size(tabsize);
                  outbuf.zeromemory();

                  lires = lires1;
                  cur_ires = 1;
                  chg_ires = 1;

                  for(i=0;i<=M;i++)
                        {
                          fact[i] = 1;
                          for(j=1;j<=i;j++) fact[i] *= j;
                        }

                  iza = izero(alpha(aa));

        }

        static REAL alpha(REAL a)
        {
                if (a <= 21) return 0;
                if (a <= 50) return 0.5842f*powf((a-21.0f),0.4f)+0.07886f*(a-21);
                return 0.1102f*(a-8.7f);
        }

        static REAL sinc(REAL x) {return x == 0 ? 1.0f : sinf(x)/x;}

        static REAL hn_lpf(int n,REAL f,REAL fs)
        {
                REAL t = 1/fs;
                REAL omega = 2*float(M_PI)*f;
                return 2*f*t*sinc(n*omega*t);
        }

        static REAL hn_imp(int n) {return n == 0 ? 1.0f : 0.0f;}

        static REAL hn(int n,paramlist &param2,REAL fs)
        {
                paramlistelm *e;
                REAL ret,lhn;

                lhn = hn_lpf(n,param2.elm->upper,fs);
                ret = param2.elm->gain*lhn;

                for(e=param2.elm->next;e->next != NULL && e->upper < fs/2;e = e->next)
                {
                        REAL lhn2 = hn_lpf(n,e->upper,fs);
                        ret += e->gain*(lhn2-lhn);
                        lhn = lhn2;
                }

                ret += e->gain*(hn_imp(n)-lhn);

                return ret;

        }


        static void process_param(const float *bc,paramlist &param2,float fs,int ch,const int *bandsI)
        {
                  paramlistelm **pp;
                  int i;

                  delete param2.elm;
                  param2.elm = NULL;

                  for(i=0,pp=&param2.elm;i<=NBANDS;i++,pp = &(*pp)->next)
                  {
                        (*pp) = new paramlistelm;
                        (*pp)->lower = i == 0      ?  0 : (bandsI[i-1]/100.0f);
                        (*pp)->upper = i == NBANDS ? fs : (bandsI[i  ]/100.0f);
                        (*pp)->gain  = bc[i];
                  }
        }
public:
        supereq(int wb=14)
        {
                firstframe=1;
                rfft_ipsize = 0;
                rfft_wsize=0;
                rfft_ip = NULL;
                rfft_w = NULL;

                aa = 96;
                memset(fact,0,sizeof(fact));
                iza=0;
                lires=0;
                lires1=0;
                lires2=0;
                irest=0;
                fsamples=0;
                ditherbuf=0;
                chg_ires=0;
                cur_ires=0;
                winlen=0;
                winlenbit=0;
                tabsize=0;
                nbufsamples=0;
                samples_done=0;
                equ_init(wb);
        }

        void equ_makeTable(const float *bc,float fs,const int *bandsI)
        {
                int i,cires = cur_ires;
                REAL *nires;

                if (fs <= 0) return;

                paramlist param2;

                // L

                process_param(bc,param2,fs,0,bandsI);

                for(i=0;i<winlen;i++)
                        irest[i] = hn(i-winlen/2,param2,fs)*win(float(i-winlen/2),winlen);

                for(;i<tabsize;i++)
                        irest[i] = 0;

                rfft(tabsize,1,irest);

                nires = cires == 1 ? lires2 : lires1;

                for(i=0;i<tabsize;i++)
                        nires[i] = irest[i];


                chg_ires = cires == 1 ? 2 : 1;

        }

        void equ_clearbuf()
        {
                firstframe = 1;
                samples_done = 0;
                nbufsamples = 0;
        }

        void write_samples(const audio_sample*input,int nsamples,int instep)
        {
                int i,p;
                REAL *ires;

                if (chg_ires) {
                        cur_ires = chg_ires;
                        lires = cur_ires == 1 ? lires1 : lires2;
                        chg_ires = 0;
                }

                p = 0;

                int flush_length = 0;

                if (!input)//flush
                {
                        if (nbufsamples==0) return;
                        flush_length = nbufsamples;
                        nsamples = winlen - nbufsamples;
                }

                while(nbufsamples+nsamples >= winlen)
                {
                        if (input)
                        {
                                for(i=0;i<winlen-nbufsamples;i++)
                                        inbuf[nbufsamples+i] = input[(i+p)*instep];
                        }
                        else
                        {
                                for(i=0;i<winlen-nbufsamples;i++)
                                        inbuf[nbufsamples+i]=0;
                        }

                        for(i=winlen;i<tabsize;i++)
                                outbuf[i-winlen] = outbuf[i];


                        p += winlen-nbufsamples;
                        nsamples -= winlen-nbufsamples;
                        nbufsamples = 0;

                        ires = lires;
                        for(i=0;i<winlen;i++)
                                fsamples[i] = inbuf[i];

                        for(i=winlen;i<tabsize;i++)
                                fsamples[i] = 0;

                        rfft(tabsize,1,fsamples);

                        fsamples[0] = ires[0]*fsamples[0];
                        fsamples[1] = ires[1]*fsamples[1];

                        for(i=1;i<tabsize/2;i++)
                        {
                                REAL re,im;
                                re = ires[i*2  ]*fsamples[i*2] - ires[i*2+1]*fsamples[i*2+1];
                                im = ires[i*2+1]*fsamples[i*2] + ires[i*2  ]*fsamples[i*2+1];

                                fsamples[i*2  ] = re;
                                fsamples[i*2+1] = im;
                        }
                        rfft(tabsize,-1,fsamples);

                        for(i=0;i<winlen;i++) outbuf[i] += fsamples[i]/tabsize*2;

                        for(i=winlen;i<tabsize;i++) outbuf[i] = fsamples[i]/tabsize*2;

                        int out_length = flush_length>0 ? flush_length+winlen/2 : winlen;

                        done.check_size(samples_done + out_length);

                        for(i=firstframe ? winlen/2 : 0;i<out_length;i++)
                        {
                                done[samples_done++]=outbuf[i];
                        }
                        firstframe=0;
                }

                if (input)
                {
                        for(i=0;i<nsamples;i++)
                        {
                                inbuf[nbufsamples+i] = input[(i+p)*instep];
                        }
                        p += nsamples;
                }
                nbufsamples += nsamples;
        }

        const audio_sample * get_output(int *nsamples)
        {
                *nsamples = samples_done;
                samples_done = 0;
                return done.get_ptr();
        }

        int samples_buffered() {return nbufsamples;}

        ~supereq()
        {
                if (lires1) free(lires1);
                if (lires2) free(lires2);
                if (irest) free(irest);
                if (fsamples) free(fsamples);
                rfft(0,0,NULL);
        }
};

#endif
