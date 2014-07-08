#ifndef _RESAMPLE2_H_
#define _RESAMPLE2_H_

template<class sample_t> struct AVResampleContextBase;
template<class sample_t> struct TreSampleContext
{
private:
 void init(int io_channels,int output_rate, int input_rate);
 AVResampleContextBase<sample_t> *resample_context;
 sample_t *temp;
 size_t temp_len;
 float ratio;
 /* channel convert */
 int io_channels, filter_channels;
 Tbuffer bufin;
public:
 TreSampleContext (int io_channels,int output_rate, int input_rate, int filter_size, int phase_shift, int linear, double cutoff,int bits);
 ~TreSampleContext();
 int audio_resample(sample_t *output, const sample_t *input, size_t nb_samples);
};

#endif
