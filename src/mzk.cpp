#include "common.hpp"
#include "mzk.hpp"
#include "math.hpp"

SAMPLE_TYPE	lpSoundBuffer[MAX_SAMPLES*2];  
HWAVEOUT	hWaveOut;

WAVEFORMATEX WaveFMT =
{
#ifdef FLOAT_32BIT	
	WAVE_FORMAT_IEEE_FLOAT,
#else
	WAVE_FORMAT_PCM,
#endif		
	2, // channels
	SAMPLE_RATE, // samples per sec
	SAMPLE_RATE*sizeof(SAMPLE_TYPE)*2, // bytes per sec
	sizeof(SAMPLE_TYPE)*2, // block alignment;
	sizeof(SAMPLE_TYPE)*8, // bits per sample
	0 // extension not needed
};

WAVEHDR WaveHDR = 
{
	(LPSTR)lpSoundBuffer, 
	MAX_SAMPLES*sizeof(SAMPLE_TYPE)*2,			// MAX_SAMPLES*sizeof(float)*2(stereo)
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};

MMTIME MMTime = 
{ 
	TIME_SAMPLES,
	0
};

void InitSound()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, lpSoundBuffer, 0, 0);
	Sleep(100);
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL);
	waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));	
}

int GetCurrentSample()
{
	waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));
	return MMTime.u.sample;
}


unsigned int bit_reverse(unsigned int x)
{
	x = (x & 0x55555555) <<  1 | (x & 0xAAAAAAAA) >>  1; 
	x = (x & 0x33333333) <<  2 | (x & 0xCCCCCCCC) >>  2; 
	x = (x & 0x0F0F0F0F) <<  4 | (x & 0xF0F0F0F0) >>  4; 
	x = (x & 0x00FF00FF) <<  8 | (x & 0xFF00FF00) >>  8; 
	x = (x & 0x0000FFFF) << 16 | (x & 0xFFFF0000) >> 16;
	return x;
}

void FFT(complex *in_out, int n)
{	
	int log2n = 0; for (int i = 1; i < n; i *= 2) ++log2n;
	for (int i = 0; i < n; ++i)
	{
		int j = bit_reverse(i) >> (32 - log2n);
		if (i < j)
		{
			complex tmp = in_out[i];
			in_out[i] = in_out[j];
			in_out[j] = tmp;
		}
	}

	for (int i = 0; i < log2n; ++i)
	{
		int mh = i << i;
		int m = mh * 2;
		for (int j = 0; j < mh; ++j)
		{
			complex e = make_complex(1, 2 * PI * j / m);
			for (int k = 0; k <= n - m; k += m)
			{
				complex u = in_out[k + j];
				complex v = in_out[k + j + mh] * e;
				in_out[k + j]	   = u + v;
				in_out[k + j + mh] = u - v;
			}
		}
	}
}


complex signal[STFT_WINDOW_SIZE];

void GetSpectrum(float *out)
{
	int cur = GetCurrentSample();
	for (int i = 0; i != STFT_WINDOW_SIZE; ++i)
	{
		signal[i].real = 0;
		signal[i].imag = 0;
		int j = cur - STFT_WINDOW_SIZE / 2 + i;
		if (0 <= j && j < MAX_SAMPLES)
		{
			signal[i].real = (lpSoundBuffer[2 * j] + lpSoundBuffer[2 * j + 1]) / 2;
		}
	}

	FFT(signal, STFT_WINDOW_SIZE);

	for (int i = 0; i != STFT_WINDOW_SIZE; ++i)
	{
		float mag = signal[i].mag();
		out[i] = mag > 1 ? 20 * log10(mag) : 0;
	}
}
