#ifndef _MZK_HPP_INCLUDED_
#define _MZK_HPP_INCLUDED_

#include "../mzk/4klang.h"

#define STFT_WINDOW_SIZE 1024

void InitSound();

int GetCurrentSample();

void GetSpectrum(float *out);

#endif  // _MZK_HPP_INCLUDED_